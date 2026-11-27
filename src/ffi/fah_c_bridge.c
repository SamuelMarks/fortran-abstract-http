/**
 * @file fah_c_bridge.c
 * @brief Complete strict ISO C89 FFI bridge implementation wrapping
 * `c-abstract-http` handles, transports, OAuth2, WebSockets, SSE, and
 * concurrency modalities.
 */

/* clang-format off */
#include <limits.h>
#include <signal.h>
#include <string.h>
#include "c_abstract_http/actor.h"
#include "c_abstract_http/c_abstract_http_tls.h"
#include "c_abstract_http/cmp_integration.h"
#include "c_abstract_http/coroutine.h"
#include "c_abstract_http/event_loop.h"
#include "c_abstract_http/http_raw.h"
#include "c_abstract_http/http_sse.h"
#include "c_abstract_http/http_types.h"
#include "c_abstract_http/http_ws.h"
#include "c_abstract_http/process.h"
#include "c_abstract_http/thread_pool.h"
#include "c_abstract_http/transport.h"
#include "fah_alloc.h"
#include "fah_c_bridge.h"
#include "fah_f77_strings.h"
/* clang-format on */

/**
 * @brief Single queued event node inside a `FahPollQueue`.
 */
struct FahPollNode {
  fah_poll_event_kind_t kind; /**< Event type */
  int code;                   /**< Associated opcode/status/error */
  char *evt_name;             /**< Optional heap event type string */
  unsigned char *data;        /**< Optional heap payload buffer */
  size_t data_len;            /**< Byte length of `data` */
  struct FahPollNode *next;   /**< Next FIFO node */
};

/**
 * @brief FIFO event queue supporting callback-less FORTRAN I-III streaming.
 */
struct FahPollQueue {
  struct FahPollNode *head; /**< Oldest queued node */
  struct FahPollNode *tail; /**< Newest queued node */
};

/**
 * @brief Boxed `HttpClient` tracking whether `http_raw` or factory transport is
 * active.
 */
struct FahClientBox {
  struct HttpClient client; /**< Embedded `c-abstract-http` client */
  int is_raw; /**< `1` if initialized via `http_raw_context_init` */
};

/**
 * @brief Boxed `HttpRequest` owning Fortran-supplied body copy, callback
 * trampolines, and cooperative cancellation token.
 */
struct FahRequestBox {
  struct HttpRequest req; /**< Embedded `c-abstract-http` request */
  void *owned_body; /**< Heap copy of body set via `fah_request_set_body` */
  FahReqCallbackCtx cb_ctx; /**< Streaming/progress callback context */
  FahWsCallbackCtx ws_cb;   /**< Active WebSocket async/sync callback context */
  FahSseCallbackCtx sse_cb; /**< Active SSE async/sync callback context */
  volatile int cancel_flag; /**< Cooperative cancellation flag */
};

/**
 * @brief Boxed `HttpResponse` tracking whether the outer `struct HttpResponse`
 * pointer was heap-allocated by `http_client_send` (`heap_res != NULL`) or
 * inline-deserialized (`heap_res == NULL`).
 */
struct FahResponseBox {
  struct HttpResponse *res;       /**< Active response pointer */
  struct HttpResponse inline_res; /**< Storage for IPC-deserialized responses */
  int owns_outer_struct; /**< `1` to `free(res)` after `http_response_free` */
};

/**
 * @brief Boxed timer callback closure owned by an event loop.
 */
struct FahTimerClosure {
  fah_handle_t loop_h;          /**< Enclosing loop handle */
  fah_f77_loop_timer_cb cb;     /**< Fortran timer callback */
  int user_id;                  /**< User integer context */
  struct FahTimerClosure *next; /**< Linked-list pointer for cleanup */
};

/**
 * @brief Boxed `ModalityEventLoop` tracking registered timer closures.
 */
struct FahLoopBox {
  struct ModalityEventLoop *loop; /**< Underlying C event loop */
  struct FahTimerClosure *timers; /**< Owned timer closures */
};

/**
 * @brief Heap closure for one-shot thread-pool task execution.
 */
struct FahTaskClosure {
  fah_f77_task_cb cb; /**< Fortran task callback */
  int user_id;        /**< User integer context */
};

/**
 * @brief Boxed `AbstractHttpCoroutine` owning its Fortran callback closure.
 */
struct FahCoroBox {
  struct AbstractHttpCoroutine *co; /**< Underlying C coroutine */
  fah_f77_task_cb cb;               /**< Fortran coroutine entry point */
  int user_id;                      /**< User integer context */
};

/**
 * @brief Boxed `AbstractHttpActor` owning its Fortran mailbox handler state.
 */
struct FahActorBox {
  struct AbstractHttpActor *actor; /**< Underlying C actor */
  fah_handle_t self_h;             /**< Registered handle of this actor */
  fah_f77_actor_cb cb;             /**< Fortran actor callback */
  int user_id;                     /**< User integer context */
  struct FahActorBox *next_in_bus; /**< Next actor registered on same bus */
};

/**
 * @brief Boxed `AbstractHttpMessageBus` tracking spawned `FahActorBox` nodes.
 */
struct FahBusBox {
  struct AbstractHttpMessageBus *bus; /**< Underlying C message bus */
  struct FahActorBox *actors;         /**< Linked list of actor closure boxes */
};

/**
 * @brief Heap integer payload sent over `AbstractHttpMessageBus`.
 */
struct FahActorMsgPayload {
  fah_handle_t sender_h; /**< Sender actor handle (`0` if external) */
  int value;             /**< Integer message value */
};

/**
 * @brief Replace a `c-abstract-http`-owned heap string (`malloc`/`free` domain)
 * using `c_abstract_http_strdup` (`NULL` leaves field unchanged; `""` sets
 * empty string).
 * @param[in,out] io_field Target `char *` struct member.
 * @param[in] val Optional replacement string (`NULL` = no-op).
 * @return `FAH_OK` on success, or `FAH_ERR_NOMEM`.
 */
static fah_error_code_t fah_replace_cah_str(char **io_field, const char *val) {
  char *dup = NULL;
  fah_error_code_t rc;

  if (val == NULL) {
    return FAH_OK;
  }
  rc = fah_strdup(val, &dup);
  if (rc != FAH_OK) {
    return rc;
  }
  if (*io_field != NULL) {
    fah_free(*io_field);
  }
  *io_field = dup;
  return FAH_OK;
}

/**
 * @brief Convert an optional string where empty `""` maps to `NULL` for
 * optional OAuth2 / multipart C parameters.
 * @param[in] s Optional C string.
 * @return `NULL` if `s == NULL || s[0] == '\0'`, else `s`.
 */
static const char *fah_opt_str(const char *s) {
  return (s == NULL || s[0] == '\0') ? NULL : s;
}

/* --- Global Lifecycle --- */

fah_error_code_t fah_global_init(void) {
  c_abstract_http_error_t rc1;
  c_abstract_http_error_t rc2;
#if defined(SIGPIPE)
  (void)signal(SIGPIPE, SIG_IGN);
#endif
  rc1 = transport_global_init();
  rc2 = http_raw_global_init();
  return fah_error_from_c((rc1 != C_ABSTRACT_HTTP_SUCCESS) ? rc1 : rc2);
}

fah_error_code_t fah_global_cleanup(void) {
  c_abstract_http_error_t rc1 = http_raw_global_cleanup();
  c_abstract_http_error_t rc2 = transport_global_cleanup();
  return fah_error_from_c((rc1 != C_ABSTRACT_HTTP_SUCCESS) ? rc1 : rc2);
}

/* --- Poll Queue --- */

fah_error_code_t fah_poll_queue_create(fah_handle_t *out_q_h) {
  void *raw = NULL;
  struct FahPollQueue *q;
  fah_error_code_t rc;

  if (out_q_h == NULL) {
    return FAH_ERR_INVAL;
  }
  rc = fah_calloc(1u, sizeof(struct FahPollQueue), &raw);
  if (rc != FAH_OK) {
    return rc;
  }
  q = (struct FahPollQueue *)raw;
  rc = fah_handle_register(FAH_TAG_POLL_QUEUE, q, out_q_h);
  if (rc != FAH_OK) {
    fah_free(q);
    return rc;
  }
  return FAH_OK;
}

/**
 * @brief Free a single `FahPollNode` and its owned strings/bytes.
 * @param[in,out] node Node to release.
 */
static void fah_poll_node_free(struct FahPollNode *node) {
  fah_free(node->evt_name);
  fah_free(node->data);
  fah_free(node);
}

fah_error_code_t fah_poll_queue_destroy(fah_handle_t q_h) {
  void *raw = NULL;
  struct FahPollQueue *q;
  struct FahPollNode *cur;
  fah_error_code_t rc = fah_handle_unregister(q_h, FAH_TAG_POLL_QUEUE, &raw);

  if (rc != FAH_OK) {
    return rc;
  }
  q = (struct FahPollQueue *)raw;
  cur = q->head;
  while (cur != NULL) {
    struct FahPollNode *nxt = cur->next;
    fah_poll_node_free(cur);
    cur = nxt;
  }
  fah_free(q);
  return FAH_OK;
}

fah_error_code_t fah_poll_queue_push(fah_handle_t q_h,
                                     fah_poll_event_kind_t kind, int code,
                                     const char *evt_name, const void *payload,
                                     size_t len) {
  void *raw_q = NULL;
  void *raw_node = NULL;
  struct FahPollQueue *q;
  struct FahPollNode *node;
  fah_error_code_t rc;

  if (len > 0u && payload == NULL) {
    return FAH_ERR_INVALID_SLICE;
  }
  rc = fah_handle_resolve(q_h, FAH_TAG_POLL_QUEUE, &raw_q);
  if (rc != FAH_OK) {
    return rc;
  }
  q = (struct FahPollQueue *)raw_q;

  rc = fah_calloc(1u, sizeof(struct FahPollNode), &raw_node);
  if (rc != FAH_OK) {
    return rc;
  }
  node = (struct FahPollNode *)raw_node;
  node->kind = kind;
  node->code = code;

  if (evt_name != NULL) {
    rc = fah_strdup(evt_name, &node->evt_name);
    if (rc != FAH_OK) {
      fah_poll_node_free(node);
      return rc;
    }
  }
  if (len > 0u) {
    void *dbuf = NULL;
    rc = fah_malloc(len, &dbuf);
    if (rc != FAH_OK) {
      fah_poll_node_free(node);
      return rc;
    }
    node->data = (unsigned char *)dbuf;
    memcpy(node->data, payload, len);
    node->data_len = len;
  }

  if (q->tail != NULL) {
    q->tail->next = node;
  } else {
    q->head = node;
  }
  q->tail = node;
  return FAH_OK;
}

fah_error_code_t fah_poll_queue_pop(fah_handle_t q_h, int *out_kind,
                                    int *out_code, char *evt_buf,
                                    size_t evt_cap, size_t *out_evt_len,
                                    void *data_buf, size_t data_cap,
                                    size_t *out_data_len) {
  void *raw_q = NULL;
  struct FahPollQueue *q;
  struct FahPollNode *node;
  fah_error_code_t rc;
  fah_error_code_t rc_evt = FAH_OK;
  fah_error_code_t rc_dat = FAH_OK;

  if (out_kind == NULL || out_code == NULL) {
    return FAH_ERR_INVAL;
  }
  *out_kind = (int)FAH_POLL_NONE;
  *out_code = 0;
  if (out_evt_len != NULL) {
    *out_evt_len = 0u;
  }
  if (out_data_len != NULL) {
    *out_data_len = 0u;
  }

  rc = fah_handle_resolve(q_h, FAH_TAG_POLL_QUEUE, &raw_q);
  if (rc != FAH_OK) {
    return rc;
  }
  q = (struct FahPollQueue *)raw_q;
  if (q->head == NULL) {
    return FAH_OK;
  }

  node = q->head;
  q->head = node->next;
  if (q->head == NULL) {
    q->tail = NULL;
  }

  *out_kind = (int)node->kind;
  *out_code = node->code;

  {
    const char *ev = (node->evt_name != NULL) ? node->evt_name : "";
    rc_evt =
        fah_cbytes_to_fstr(ev, strlen(ev), evt_buf, evt_cap, out_evt_len, 1);
  }
  rc_dat = fah_cbytes_to_fstr(node->data, node->data_len, (char *)data_buf,
                              data_cap, out_data_len, 0);

  fah_poll_node_free(node);
  if (rc_evt != FAH_OK) {
    return rc_evt;
  }
  return rc_dat;
}

/* --- HttpConfig & HttpClient --- */

fah_error_code_t fah_config_create(fah_handle_t *out_cfg_h) {
  void *raw = NULL;
  struct HttpConfig *cfg;
  fah_error_code_t rc;

  if (out_cfg_h == NULL) {
    return FAH_ERR_INVAL;
  }
  rc = fah_calloc(1u, sizeof(struct HttpConfig), &raw);
  if (rc != FAH_OK) {
    return rc;
  }
  cfg = (struct HttpConfig *)raw;
  FAH_IGNORE_RC(http_config_init(cfg));
  rc = fah_handle_register(FAH_TAG_CONFIG, cfg, out_cfg_h);
  if (rc != FAH_OK) {
    FAH_IGNORE_RC(http_config_free(cfg));
    fah_free(cfg);
    return rc;
  }
  return FAH_OK;
}

fah_error_code_t fah_config_destroy(fah_handle_t cfg_h) {
  void *raw = NULL;
  struct HttpConfig *cfg;
  c_abstract_http_error_t crc;
  fah_error_code_t rc = fah_handle_unregister(cfg_h, FAH_TAG_CONFIG, &raw);
  if (rc != FAH_OK) {
    return rc;
  }
  cfg = (struct HttpConfig *)raw;
  crc = http_config_free(cfg);
  fah_free(cfg);
  return fah_error_from_c(crc);
}

fah_error_code_t fah_config_set_timeouts(fah_handle_t cfg_h, long timeout_ms,
                                         long connect_ms, long read_ms,
                                         long write_ms) {
  void *raw = NULL;
  struct HttpConfig *cfg;
  fah_error_code_t rc;

  if (timeout_ms < 0L || connect_ms < 0L || read_ms < 0L || write_ms < 0L) {
    return FAH_ERR_INVAL;
  }
  rc = fah_handle_resolve(cfg_h, FAH_TAG_CONFIG, &raw);
  if (rc != FAH_OK) {
    return rc;
  }
  cfg = (struct HttpConfig *)raw;
  cfg->timeout_ms = timeout_ms;
  cfg->connect_timeout_ms = connect_ms;
  cfg->read_timeout_ms = read_ms;
  cfg->write_timeout_ms = write_ms;
  return FAH_OK;
}

fah_error_code_t fah_config_set_tls_flags(fah_handle_t cfg_h, int verify_peer,
                                          int verify_host, int follow_redirects,
                                          int http3_fallback) {
  void *raw = NULL;
  struct HttpConfig *cfg;
  fah_error_code_t rc = fah_handle_resolve(cfg_h, FAH_TAG_CONFIG, &raw);
  if (rc != FAH_OK) {
    return rc;
  }
  cfg = (struct HttpConfig *)raw;
  cfg->verify_peer = (verify_peer != 0) ? 1 : 0;
  cfg->verify_host = (verify_host != 0) ? 1 : 0;
  cfg->follow_redirects = (follow_redirects != 0) ? 1 : 0;
  cfg->http3_fallback = (http3_fallback != 0) ? 1 : 0;
  return FAH_OK;
}

fah_error_code_t
fah_config_set_versions_and_retry(fah_handle_t cfg_h, unsigned int version_mask,
                                  unsigned int tls_version_mask,
                                  int retry_count, int retry_policy) {
  void *raw = NULL;
  struct HttpConfig *cfg;
  fah_error_code_t rc;

  if (retry_count < 0 || retry_policy < (int)HTTP_RETRY_NONE ||
      retry_policy > (int)HTTP_RETRY_EXPONENTIAL) {
    return FAH_ERR_INVAL;
  }
  rc = fah_handle_resolve(cfg_h, FAH_TAG_CONFIG, &raw);
  if (rc != FAH_OK) {
    return rc;
  }
  cfg = (struct HttpConfig *)raw;
  cfg->version_mask = version_mask;
  cfg->tls_version_mask = tls_version_mask;
  cfg->retry_count = retry_count;
  cfg->retry_policy = (enum HttpRetryPolicy)retry_policy;
  return FAH_OK;
}

fah_error_code_t fah_config_set_modality_and_pool(
    fah_handle_t cfg_h, int modality, size_t min_threads, size_t max_threads,
    int keep_alive_enabled, long keep_alive_idle_ms, size_t max_idle_conns) {
  void *raw = NULL;
  struct HttpConfig *cfg;
  fah_error_code_t rc;

  if (modality < (int)MODALITY_SYNC ||
      modality > (int)MODALITY_MESSAGE_PASSING || keep_alive_idle_ms < 0L) {
    return FAH_ERR_INVAL;
  }
  rc = fah_handle_resolve(cfg_h, FAH_TAG_CONFIG, &raw);
  if (rc != FAH_OK) {
    return rc;
  }
  cfg = (struct HttpConfig *)raw;
  cfg->modality = (enum ExecutionModality)modality;
  cfg->min_threads = min_threads;
  cfg->max_threads = max_threads;
  cfg->keep_alive_enabled = (keep_alive_enabled != 0) ? 1 : 0;
  cfg->keep_alive_idle_ms = keep_alive_idle_ms;
  cfg->max_idle_conns = max_idle_conns;
  return FAH_OK;
}

fah_error_code_t
fah_config_set_strings(fah_handle_t cfg_h, const char *user_agent,
                       const char *proxy_url, const char *proxy_user,
                       const char *proxy_pass, const char *ca_path,
                       const char *client_cert, const char *client_key,
                       const char *cert_pin_sha256) {
  void *raw = NULL;
  struct HttpConfig *cfg;
  fah_error_code_t rc = fah_handle_resolve(cfg_h, FAH_TAG_CONFIG, &raw);
  if (rc != FAH_OK) {
    return rc;
  }
  cfg = (struct HttpConfig *)raw;

  rc = fah_replace_cah_str(&cfg->user_agent, user_agent);
  if (rc != FAH_OK)
    return rc;
  rc = fah_replace_cah_str(&cfg->proxy_url, proxy_url);
  if (rc != FAH_OK)
    return rc;
  rc = fah_replace_cah_str(&cfg->proxy_username, proxy_user);
  if (rc != FAH_OK)
    return rc;
  rc = fah_replace_cah_str(&cfg->proxy_password, proxy_pass);
  if (rc != FAH_OK)
    return rc;
  rc = fah_replace_cah_str(&cfg->ca_path, ca_path);
  if (rc != FAH_OK)
    return rc;
  rc = fah_replace_cah_str(&cfg->client_cert_path, client_cert);
  if (rc != FAH_OK)
    return rc;
  rc = fah_replace_cah_str(&cfg->client_key_path, client_key);
  if (rc != FAH_OK)
    return rc;
  return fah_replace_cah_str(&cfg->cert_pin_sha256, cert_pin_sha256);
}

fah_error_code_t fah_config_set_cookie_jar(fah_handle_t cfg_h,
                                           fah_handle_t jar_h) {
  void *raw_cfg = NULL;
  void *raw_jar = NULL;
  struct HttpConfig *cfg;
  fah_error_code_t rc = fah_handle_resolve(cfg_h, FAH_TAG_CONFIG, &raw_cfg);
  if (rc != FAH_OK) {
    return rc;
  }
  cfg = (struct HttpConfig *)raw_cfg;
  if (jar_h == 0) {
    cfg->cookie_jar = NULL;
    return FAH_OK;
  }
  rc = fah_handle_resolve(jar_h, FAH_TAG_COOKIE_JAR, &raw_jar);
  if (rc != FAH_OK) {
    return rc;
  }
  cfg->cookie_jar = (struct HttpCookieJar *)raw_jar;
  return FAH_OK;
}

/**
 * @brief Deep-copy owned string fields from `src` config into `dst` config.
 * @param[in,out] dst Destination `HttpConfig` (already initialized).
 * @param[in] src Source `HttpConfig`.
 * @return `FAH_OK` or `FAH_ERR_NOMEM`.
 */
static fah_error_code_t fah_copy_config_into(struct HttpConfig *dst,
                                             const struct HttpConfig *src) {
  fah_error_code_t rc;
  FAH_IGNORE_RC(http_config_free(dst));
  *dst = *src;
  dst->user_agent = NULL;
  dst->proxy_url = NULL;
  dst->proxy_username = NULL;
  dst->proxy_password = NULL;
  dst->ca_path = NULL;
  dst->ca_buffer = NULL;
  dst->ca_buffer_len = 0u;
  dst->client_cert_path = NULL;
  dst->client_key_path = NULL;
  dst->cert_pin_sha256 = NULL;

  rc = fah_replace_cah_str(&dst->user_agent, src->user_agent);
  if (rc != FAH_OK)
    return rc;
  rc = fah_replace_cah_str(&dst->proxy_url, src->proxy_url);
  if (rc != FAH_OK)
    return rc;
  rc = fah_replace_cah_str(&dst->proxy_username, src->proxy_username);
  if (rc != FAH_OK)
    return rc;
  rc = fah_replace_cah_str(&dst->proxy_password, src->proxy_password);
  if (rc != FAH_OK)
    return rc;
  rc = fah_replace_cah_str(&dst->ca_path, src->ca_path);
  if (rc != FAH_OK)
    return rc;
  rc = fah_replace_cah_str(&dst->client_cert_path, src->client_cert_path);
  if (rc != FAH_OK)
    return rc;
  rc = fah_replace_cah_str(&dst->client_key_path, src->client_key_path);
  if (rc != FAH_OK)
    return rc;
  return fah_replace_cah_str(&dst->cert_pin_sha256, src->cert_pin_sha256);
}

/**
 * @brief Tear down transport and client internals of a `FahClientBox`.
 * @param[in,out] box Client box to clean up.
 */
static void fah_client_box_cleanup(struct FahClientBox *box) {
  if (box->is_raw) {
    if (box->client.transport != NULL) {
      FAH_IGNORE_RC(http_raw_context_free(box->client.transport));
      box->client.transport = NULL;
    }
  } else {
    FAH_IGNORE_RC(transport_factory_cleanup_client(&box->client));
  }
  box->client.loop = NULL;
  FAH_IGNORE_RC(http_client_free(&box->client));
  fah_free(box);
}

fah_error_code_t fah_client_create(fah_handle_t cfg_h, int use_raw_transport,
                                   fah_handle_t *out_cli_h) {
  void *raw_box = NULL;
  struct FahClientBox *box;
  const struct HttpConfig *src_cfg = NULL;
  c_abstract_http_error_t crc;
  fah_error_code_t rc;

  if (out_cli_h == NULL) {
    return FAH_ERR_INVAL;
  }
  if (cfg_h != 0) {
    void *raw_cfg = NULL;
    rc = fah_handle_resolve(cfg_h, FAH_TAG_CONFIG, &raw_cfg);
    if (rc != FAH_OK) {
      return rc;
    }
    src_cfg = (const struct HttpConfig *)raw_cfg;
  }

  rc = fah_calloc(1u, sizeof(struct FahClientBox), &raw_box);
  if (rc != FAH_OK) {
    return rc;
  }
  box = (struct FahClientBox *)raw_box;
  FAH_IGNORE_RC(http_client_init(&box->client));

  if (src_cfg != NULL) {
    rc = fah_copy_config_into(&box->client.config, src_cfg);
    if (rc != FAH_OK) {
      FAH_IGNORE_RC(http_client_free(&box->client));
      fah_free(box);
      return rc;
    }
  }

  if (use_raw_transport) {
    box->is_raw = 1;
    crc = http_raw_context_init(&box->client.transport);
    if (crc == C_ABSTRACT_HTTP_SUCCESS) {
      crc = http_raw_config_apply(box->client.transport, &box->client.config);
    }
    if (crc == C_ABSTRACT_HTTP_SUCCESS) {
      box->client.send = http_raw_send;
      box->client.send_multi = http_raw_send_multi;
    }
  } else {
    crc = transport_factory_init_client(&box->client);
  }
  if (crc != C_ABSTRACT_HTTP_SUCCESS) {
    fah_client_box_cleanup(box);
    return fah_error_from_c(crc);
  }

  rc = fah_handle_register(FAH_TAG_CLIENT, box, out_cli_h);
  if (rc != FAH_OK) {
    fah_client_box_cleanup(box);
    return rc;
  }
  return FAH_OK;
}

fah_error_code_t fah_client_set_base_url(fah_handle_t cli_h,
                                         const char *base_url) {
  void *raw = NULL;
  struct FahClientBox *box;
  fah_error_code_t rc;

  if (base_url == NULL) {
    return FAH_ERR_INVAL;
  }
  rc = fah_handle_resolve(cli_h, FAH_TAG_CLIENT, &raw);
  if (rc != FAH_OK) {
    return rc;
  }
  box = (struct FahClientBox *)raw;
  return fah_replace_cah_str(&box->client.base_url, base_url);
}

fah_error_code_t fah_client_destroy(fah_handle_t cli_h) {
  void *raw = NULL;
  fah_error_code_t rc = fah_handle_unregister(cli_h, FAH_TAG_CLIENT, &raw);
  if (rc != FAH_OK) {
    return rc;
  }
  fah_client_box_cleanup((struct FahClientBox *)raw);
  return FAH_OK;
}

/* --- Headers & Cookie Jar --- */

fah_error_code_t fah_headers_create(fah_handle_t *out_hdr_h) {
  void *raw = NULL;
  struct HttpHeaders *hdrs;
  fah_error_code_t rc;

  if (out_hdr_h == NULL) {
    return FAH_ERR_INVAL;
  }
  rc = fah_calloc(1u, sizeof(struct HttpHeaders), &raw);
  if (rc != FAH_OK) {
    return rc;
  }
  hdrs = (struct HttpHeaders *)raw;
  FAH_IGNORE_RC(http_headers_init(hdrs));
  rc = fah_handle_register(FAH_TAG_HEADERS, hdrs, out_hdr_h);
  if (rc != FAH_OK) {
    FAH_IGNORE_RC(http_headers_free(hdrs));
    fah_free(hdrs);
    return rc;
  }
  return FAH_OK;
}

fah_error_code_t fah_headers_destroy(fah_handle_t hdr_h) {
  void *raw = NULL;
  struct HttpHeaders *hdrs;
  c_abstract_http_error_t crc;
  fah_error_code_t rc = fah_handle_unregister(hdr_h, FAH_TAG_HEADERS, &raw);
  if (rc != FAH_OK) {
    return rc;
  }
  hdrs = (struct HttpHeaders *)raw;
  crc = http_headers_free(hdrs);
  fah_free(hdrs);
  return fah_error_from_c(crc);
}

fah_error_code_t fah_headers_add(fah_handle_t hdr_h, const char *key,
                                 const char *val) {
  void *raw = NULL;
  c_abstract_http_error_t crc;
  fah_error_code_t rc;

  if (key == NULL || val == NULL) {
    return FAH_ERR_INVAL;
  }
  rc = fah_handle_resolve(hdr_h, FAH_TAG_HEADERS, &raw);
  if (rc != FAH_OK) {
    return rc;
  }
  crc = http_headers_add((struct HttpHeaders *)raw, key, val);
  return fah_error_from_c(crc);
}

fah_error_code_t fah_headers_get(fah_handle_t hdr_h, const char *key,
                                 char *out_buf, size_t cap, size_t *out_len,
                                 int space_pad) {
  void *raw = NULL;
  const char *found = NULL;
  c_abstract_http_error_t crc;
  fah_error_code_t rc;

  if (key == NULL) {
    return FAH_ERR_INVAL;
  }
  rc = fah_handle_resolve(hdr_h, FAH_TAG_HEADERS, &raw);
  if (rc != FAH_OK) {
    return rc;
  }
  crc = http_headers_get((const struct HttpHeaders *)raw, key, &found);
  if (crc != C_ABSTRACT_HTTP_SUCCESS || found == NULL) {
    return fah_error_from_c(crc);
  }
  return fah_cbytes_to_fstr(found, strlen(found), out_buf, cap, out_len,
                            space_pad);
}

fah_error_code_t fah_headers_remove(fah_handle_t hdr_h, const char *key) {
  void *raw = NULL;
  c_abstract_http_error_t crc;
  fah_error_code_t rc;

  if (key == NULL) {
    return FAH_ERR_INVAL;
  }
  rc = fah_handle_resolve(hdr_h, FAH_TAG_HEADERS, &raw);
  if (rc != FAH_OK) {
    return rc;
  }
  crc = http_headers_remove((struct HttpHeaders *)raw, key);
  return fah_error_from_c(crc);
}

fah_error_code_t fah_headers_count(fah_handle_t hdr_h, size_t *out_count) {
  void *raw = NULL;
  fah_error_code_t rc;

  if (out_count == NULL) {
    return FAH_ERR_INVAL;
  }
  rc = fah_handle_resolve(hdr_h, FAH_TAG_HEADERS, &raw);
  if (rc != FAH_OK) {
    return rc;
  }
  *out_count = ((const struct HttpHeaders *)raw)->count;
  return FAH_OK;
}

fah_error_code_t fah_cookie_jar_create(fah_handle_t *out_jar_h) {
  void *raw = NULL;
  struct HttpCookieJar *jar;
  fah_error_code_t rc;

  if (out_jar_h == NULL) {
    return FAH_ERR_INVAL;
  }
  rc = fah_calloc(1u, sizeof(struct HttpCookieJar), &raw);
  if (rc != FAH_OK) {
    return rc;
  }
  jar = (struct HttpCookieJar *)raw;
  FAH_IGNORE_RC(http_cookie_jar_init(jar));
  rc = fah_handle_register(FAH_TAG_COOKIE_JAR, jar, out_jar_h);
  if (rc != FAH_OK) {
    FAH_IGNORE_RC(http_cookie_jar_free(jar));
    fah_free(jar);
    return rc;
  }
  return FAH_OK;
}

fah_error_code_t fah_cookie_jar_destroy(fah_handle_t jar_h) {
  void *raw = NULL;
  struct HttpCookieJar *jar;
  c_abstract_http_error_t crc;
  fah_error_code_t rc = fah_handle_unregister(jar_h, FAH_TAG_COOKIE_JAR, &raw);
  if (rc != FAH_OK) {
    return rc;
  }
  jar = (struct HttpCookieJar *)raw;
  crc = http_cookie_jar_free(jar);
  fah_free(jar);
  return fah_error_from_c(crc);
}

fah_error_code_t fah_cookie_jar_set(fah_handle_t jar_h, const char *name,
                                    const char *val) {
  void *raw = NULL;
  c_abstract_http_error_t crc;
  fah_error_code_t rc;

  if (name == NULL || val == NULL) {
    return FAH_ERR_INVAL;
  }
  rc = fah_handle_resolve(jar_h, FAH_TAG_COOKIE_JAR, &raw);
  if (rc != FAH_OK) {
    return rc;
  }
  crc = http_cookie_jar_set((struct HttpCookieJar *)raw, name, val);
  return fah_error_from_c(crc);
}

fah_error_code_t fah_cookie_jar_get(fah_handle_t jar_h, const char *name,
                                    char *out_buf, size_t cap, size_t *out_len,
                                    int space_pad) {
  void *raw = NULL;
  const char *found = NULL;
  c_abstract_http_error_t crc;
  fah_error_code_t rc;

  if (name == NULL) {
    return FAH_ERR_INVAL;
  }
  rc = fah_handle_resolve(jar_h, FAH_TAG_COOKIE_JAR, &raw);
  if (rc != FAH_OK) {
    return rc;
  }
  crc = http_cookie_jar_get((const struct HttpCookieJar *)raw, name, &found);
  if (crc != C_ABSTRACT_HTTP_SUCCESS || found == NULL) {
    return fah_error_from_c(crc);
  }
  return fah_cbytes_to_fstr(found, strlen(found), out_buf, cap, out_len,
                            space_pad);
}

fah_error_code_t fah_cookie_jar_parse_set(fah_handle_t jar_h,
                                          const char *set_cookie) {
  void *raw = NULL;
  c_abstract_http_error_t crc;
  fah_error_code_t rc;

  if (set_cookie == NULL) {
    return FAH_ERR_INVAL;
  }
  rc = fah_handle_resolve(jar_h, FAH_TAG_COOKIE_JAR, &raw);
  if (rc != FAH_OK) {
    return rc;
  }
  crc =
      http_cookie_jar_parse_set_cookie((struct HttpCookieJar *)raw, set_cookie);
  return fah_error_from_c(crc);
}

fah_error_code_t fah_cookie_jar_build_hdr(fah_handle_t jar_h,
                                          const char *req_url, char *out_buf,
                                          size_t cap, size_t *out_len,
                                          int space_pad) {
  void *raw = NULL;
  char *built = NULL;
  fah_error_code_t rc = fah_handle_resolve(jar_h, FAH_TAG_COOKIE_JAR, &raw);
  if (rc != FAH_OK) {
    return rc;
  }
  FAH_IGNORE_RC(http_cookie_jar_build_header((const struct HttpCookieJar *)raw,
                                             fah_opt_str(req_url), &built));
  if (built == NULL) {
    return fah_cbytes_to_fstr("", 0u, out_buf, cap, out_len, space_pad);
  }
  rc = fah_cbytes_to_fstr(built, strlen(built), out_buf, cap, out_len,
                          space_pad);
  fah_free(built);
  return rc;
}

/* --- HttpRequest, Auth, OAuth2, Send & HttpResponse --- */

fah_error_code_t fah_request_create(fah_handle_t *out_req_h) {
  void *raw = NULL;
  struct FahRequestBox *box;
  fah_error_code_t rc;

  if (out_req_h == NULL) {
    return FAH_ERR_INVAL;
  }
  rc = fah_calloc(1u, sizeof(struct FahRequestBox), &raw);
  if (rc != FAH_OK) {
    return rc;
  }
  box = (struct FahRequestBox *)raw;
  FAH_IGNORE_RC(http_request_init(&box->req));
  rc = fah_handle_register(FAH_TAG_REQUEST, box, out_req_h);
  if (rc != FAH_OK) {
    FAH_IGNORE_RC(http_request_free(&box->req));
    fah_free(box);
    return rc;
  }
  return FAH_OK;
}

/**
 * @brief Clean up and release a `FahRequestBox`.
 * @param[in,out] box Request box to destroy.
 */
static void fah_request_box_free(struct FahRequestBox *box) {
  if (box->owned_body != NULL) {
    if (box->req.body == box->owned_body) {
      box->req.body = NULL;
      box->req.body_len = 0u;
    }
    fah_free(box->owned_body);
    box->owned_body = NULL;
  }
  FAH_IGNORE_RC(http_request_free(&box->req));
  fah_free(box);
}

fah_error_code_t fah_request_destroy(fah_handle_t req_h) {
  void *raw = NULL;
  fah_error_code_t rc = fah_handle_unregister(req_h, FAH_TAG_REQUEST, &raw);
  if (rc != FAH_OK) {
    return rc;
  }
  fah_request_box_free((struct FahRequestBox *)raw);
  return FAH_OK;
}

fah_error_code_t fah_request_set_target(fah_handle_t req_h, int method,
                                        const char *url) {
  void *raw = NULL;
  struct FahRequestBox *box;
  fah_error_code_t rc;

  if (url == NULL || method < (int)HTTP_GET || method > (int)HTTP_CONNECT) {
    return FAH_ERR_INVAL;
  }
  rc = fah_handle_resolve(req_h, FAH_TAG_REQUEST, &raw);
  if (rc != FAH_OK) {
    return rc;
  }
  box = (struct FahRequestBox *)raw;
  rc = fah_replace_cah_str(&box->req.url, url);
  if (rc != FAH_OK) {
    return rc;
  }
  box->req.method = (enum HttpMethod)method;
  return FAH_OK;
}

fah_error_code_t fah_request_add_header(fah_handle_t req_h, const char *key,
                                        const char *val) {
  void *raw = NULL;
  struct FahRequestBox *box;
  c_abstract_http_error_t crc;
  fah_error_code_t rc;

  if (key == NULL || val == NULL) {
    return FAH_ERR_INVAL;
  }
  rc = fah_handle_resolve(req_h, FAH_TAG_REQUEST, &raw);
  if (rc != FAH_OK) {
    return rc;
  }
  box = (struct FahRequestBox *)raw;
  crc = http_headers_add(&box->req.headers, key, val);
  return fah_error_from_c(crc);
}

fah_error_code_t fah_request_set_body(fah_handle_t req_h, const void *body,
                                      size_t body_len) {
  void *raw = NULL;
  void *copy = NULL;
  struct FahRequestBox *box;
  fah_error_code_t rc;

  if (body_len > 0u && body == NULL) {
    return FAH_ERR_INVALID_SLICE;
  }
  rc = fah_handle_resolve(req_h, FAH_TAG_REQUEST, &raw);
  if (rc != FAH_OK) {
    return rc;
  }
  box = (struct FahRequestBox *)raw;

  if (body_len > 0u) {
    rc = fah_malloc(body_len, &copy);
    if (rc != FAH_OK) {
      return rc;
    }
    memcpy(copy, body, body_len);
  }

  if (box->owned_body != NULL) {
    fah_free(box->owned_body);
  } else if (box->req.body != NULL) {
    fah_free(box->req.body);
  }
  box->owned_body = copy;
  box->req.body = copy;
  box->req.body_len = body_len;
  return FAH_OK;
}

fah_error_code_t fah_request_set_body_cfi(fah_handle_t req_h,
                                          const fah_cfi_cdesc_t *desc) {
  const void *bytes = NULL;
  void *owned = NULL;
  size_t len = 0u;
  fah_error_code_t rc = fah_cfi_extract_bytes(desc, &bytes, &len, &owned);
  if (rc != FAH_OK) {
    return rc;
  }
  rc = fah_request_set_body(req_h, bytes, len);
  fah_free(owned);
  return rc;
}

fah_error_code_t fah_request_add_part(fah_handle_t req_h, const char *name,
                                      const char *filename,
                                      const char *content_type,
                                      const void *data, size_t data_len) {
  void *raw = NULL;
  struct FahRequestBox *box;
  c_abstract_http_error_t crc;
  fah_error_code_t rc;

  if (name == NULL || (data_len > 0u && data == NULL)) {
    return FAH_ERR_INVAL;
  }
  rc = fah_handle_resolve(req_h, FAH_TAG_REQUEST, &raw);
  if (rc != FAH_OK) {
    return rc;
  }
  box = (struct FahRequestBox *)raw;
  crc = http_request_add_part(&box->req, name, fah_opt_str(filename),
                              fah_opt_str(content_type),
                              (data != NULL) ? data : "", data_len);
  return fah_error_from_c(crc);
}

fah_error_code_t fah_request_add_part_header(fah_handle_t req_h,
                                             const char *key, const char *val) {
  void *raw = NULL;
  struct FahRequestBox *box;
  c_abstract_http_error_t crc;
  fah_error_code_t rc;

  if (key == NULL || val == NULL) {
    return FAH_ERR_INVAL;
  }
  rc = fah_handle_resolve(req_h, FAH_TAG_REQUEST, &raw);
  if (rc != FAH_OK) {
    return rc;
  }
  box = (struct FahRequestBox *)raw;
  crc = http_request_add_part_header_last(&box->req, key, val);
  return fah_error_from_c(crc);
}

fah_error_code_t fah_request_flatten_parts(fah_handle_t req_h) {
  void *raw = NULL;
  struct FahRequestBox *box;
  c_abstract_http_error_t crc;
  fah_error_code_t rc = fah_handle_resolve(req_h, FAH_TAG_REQUEST, &raw);
  if (rc != FAH_OK) {
    return rc;
  }
  box = (struct FahRequestBox *)raw;
  if (box->owned_body != NULL) {
    if (box->req.body == box->owned_body) {
      box->req.body = NULL;
    }
    fah_free(box->owned_body);
    box->owned_body = NULL;
  }
  crc = http_request_flatten_parts(&box->req);
  return fah_error_from_c(crc);
}

fah_error_code_t fah_request_set_callbacks(
    fah_handle_t req_h, fah_f77_on_chunk_cb on_chunk, int chunk_uid,
    fah_f77_read_chunk_cb read_chunk, int read_uid, size_t expected_len,
    fah_f77_on_progress_cb on_prog, int prog_uid, fah_handle_t poll_q_h) {
  void *raw = NULL;
  struct FahRequestBox *box;
  fah_error_code_t rc = fah_handle_resolve(req_h, FAH_TAG_REQUEST, &raw);
  if (rc != FAH_OK) {
    return rc;
  }
  if (poll_q_h != 0) {
    void *rq = NULL;
    rc = fah_handle_resolve(poll_q_h, FAH_TAG_POLL_QUEUE, &rq);
    if (rc != FAH_OK) {
      return rc;
    }
  }
  box = (struct FahRequestBox *)raw;
  box->cb_ctx.on_chunk = on_chunk;
  box->cb_ctx.on_chunk_uid = chunk_uid;
  box->cb_ctx.read_chunk = read_chunk;
  box->cb_ctx.read_chunk_uid = read_uid;
  box->cb_ctx.on_progress = on_prog;
  box->cb_ctx.on_progress_uid = prog_uid;
  box->cb_ctx.poll_queue_h = poll_q_h;

  if (on_chunk != NULL || poll_q_h > 0) {
    box->req.on_chunk = fah_trampoline_on_chunk;
    box->req.on_chunk_user_data = &box->cb_ctx;
  } else {
    box->req.on_chunk = NULL;
    box->req.on_chunk_user_data = NULL;
  }
  if (read_chunk != NULL) {
    box->req.read_chunk = fah_trampoline_read_chunk;
    box->req.read_chunk_user_data = &box->cb_ctx;
    box->req.expected_body_len = expected_len;
  } else {
    box->req.read_chunk = NULL;
    box->req.read_chunk_user_data = NULL;
    box->req.expected_body_len = 0u;
  }
  if (on_prog != NULL) {
    box->req.on_progress = fah_trampoline_on_progress;
    box->req.on_progress_user_data = &box->cb_ctx;
  } else {
    box->req.on_progress = NULL;
    box->req.on_progress_user_data = NULL;
  }
  return FAH_OK;
}

fah_error_code_t fah_request_set_cancel(fah_handle_t req_h, int cancel_val) {
  void *raw = NULL;
  struct FahRequestBox *box;
  fah_error_code_t rc = fah_handle_resolve(req_h, FAH_TAG_REQUEST, &raw);
  if (rc != FAH_OK) {
    return rc;
  }
  box = (struct FahRequestBox *)raw;
  box->cancel_flag = (cancel_val != 0) ? 1 : 0;
  box->req.cancel_token = &box->cancel_flag;
  return FAH_OK;
}

fah_error_code_t fah_auth_bearer(fah_handle_t req_h, const char *token) {
  void *raw = NULL;
  c_abstract_http_error_t crc;
  fah_error_code_t rc;
  if (token == NULL) {
    return FAH_ERR_INVAL;
  }
  rc = fah_handle_resolve(req_h, FAH_TAG_REQUEST, &raw);
  if (rc != FAH_OK) {
    return rc;
  }
  crc =
      http_request_set_auth_bearer(&((struct FahRequestBox *)raw)->req, token);
  return fah_error_from_c(crc);
}

fah_error_code_t fah_auth_basic(fah_handle_t req_h, const char *b64_token) {
  void *raw = NULL;
  c_abstract_http_error_t crc;
  fah_error_code_t rc;
  if (b64_token == NULL) {
    return FAH_ERR_INVAL;
  }
  rc = fah_handle_resolve(req_h, FAH_TAG_REQUEST, &raw);
  if (rc != FAH_OK) {
    return rc;
  }
  crc = http_request_set_auth_basic(&((struct FahRequestBox *)raw)->req,
                                    b64_token);
  return fah_error_from_c(crc);
}

fah_error_code_t fah_auth_basic_userpwd(fah_handle_t req_h,
                                        const char *username,
                                        const char *password) {
  void *raw = NULL;
  c_abstract_http_error_t crc;
  fah_error_code_t rc;
  if (username == NULL || password == NULL) {
    return FAH_ERR_INVAL;
  }
  rc = fah_handle_resolve(req_h, FAH_TAG_REQUEST, &raw);
  if (rc != FAH_OK) {
    return rc;
  }
  crc = http_request_set_auth_basic_userpwd(&((struct FahRequestBox *)raw)->req,
                                            username, password);
  return fah_error_from_c(crc);
}

fah_error_code_t fah_oauth2_grant(fah_handle_t req_h,
                                  fah_oauth2_grant_kind_t grant_kind,
                                  const char *endpoint_url, const char *p1,
                                  const char *p2, const char *p3,
                                  const char *p4, const char *p5) {
  void *raw = NULL;
  struct FahRequestBox *box;
  c_abstract_http_error_t crc;
  fah_error_code_t rc;

  if (endpoint_url == NULL || p1 == NULL) {
    return FAH_ERR_INVAL;
  }
  rc = fah_handle_resolve(req_h, FAH_TAG_REQUEST, &raw);
  if (rc != FAH_OK) {
    return rc;
  }
  box = (struct FahRequestBox *)raw;
  if (box->owned_body != NULL) {
    if (box->req.body == box->owned_body) {
      box->req.body = NULL;
    }
    fah_free(box->owned_body);
    box->owned_body = NULL;
  }
  FAH_IGNORE_RC(http_request_free(&box->req));

  switch (grant_kind) {
  case FAH_OAUTH2_PASSWORD:
    if (p2 == NULL)
      return FAH_ERR_INVAL;
    crc = http_request_init_oauth2_password_grant(
        &box->req, endpoint_url, p1, p2, fah_opt_str(p3), fah_opt_str(p4),
        fah_opt_str(p5));
    break;
  case FAH_OAUTH2_REFRESH_TOKEN:
    crc = http_request_init_oauth2_refresh_token_grant(
        &box->req, endpoint_url, p1, fah_opt_str(p2), fah_opt_str(p3),
        fah_opt_str(p4));
    break;
  case FAH_OAUTH2_AUTH_CODE:
    crc = http_request_init_oauth2_authorization_code_grant(
        &box->req, endpoint_url, p1, fah_opt_str(p2), fah_opt_str(p3),
        fah_opt_str(p4), fah_opt_str(p5));
    break;
  case FAH_OAUTH2_CLIENT_CREDENTIALS:
    crc = http_request_init_oauth2_client_credentials_grant(
        &box->req, endpoint_url, p1, fah_opt_str(p2), fah_opt_str(p3));
    break;
  case FAH_OAUTH2_JWT_BEARER:
    crc = http_request_init_oauth2_jwt_bearer_grant(&box->req, endpoint_url, p1,
                                                    fah_opt_str(p2));
    break;
  case FAH_OAUTH2_DEVICE_AUTH:
    crc = http_request_init_oauth2_device_authorization_request(
        &box->req, endpoint_url, p1, fah_opt_str(p2));
    break;
  case FAH_OAUTH2_DEVICE_TOKEN:
    if (p2 == NULL)
      return FAH_ERR_INVAL;
    crc = http_request_init_oauth2_device_access_token_request(
        &box->req, endpoint_url, p1, p2);
    break;
  case FAH_OAUTH2_REVOKE:
    crc = http_request_init_oauth2_token_revocation(
        &box->req, endpoint_url, p1, fah_opt_str(p2), fah_opt_str(p3),
        fah_opt_str(p4));
    break;
  case FAH_OAUTH2_INTROSPECT:
    crc = http_request_init_oauth2_token_introspection(
        &box->req, endpoint_url, p1, fah_opt_str(p2), fah_opt_str(p3),
        fah_opt_str(p4));
    break;
  default:
    FAH_IGNORE_RC(http_request_init(&box->req));
    return FAH_ERR_INVAL;
  }
  return fah_error_from_c(crc);
}

fah_error_code_t fah_oauth2_build_auth_url(
    const char *auth_ep, const char *client_id, const char *resp_type,
    const char *redirect_uri, const char *scope, const char *state,
    const char *challenge, const char *challenge_method, char *out_buf,
    size_t cap, size_t *out_len, int space_pad) {
  char *built = NULL;
  fah_error_code_t rc;

  if (auth_ep == NULL || client_id == NULL || resp_type == NULL) {
    return FAH_ERR_INVAL;
  }
  FAH_IGNORE_RC(http_oauth2_build_authorization_url(
      auth_ep, client_id, resp_type, fah_opt_str(redirect_uri),
      fah_opt_str(scope), fah_opt_str(state), fah_opt_str(challenge),
      fah_opt_str(challenge_method), &built));
  rc = fah_cbytes_to_fstr(built, strlen(built), out_buf, cap, out_len,
                          space_pad);
  fah_free(built);
  return rc;
}

fah_error_code_t fah_oauth2_localhost_intercept(
    unsigned short port, const char *html_resp, char *code_buf, size_t code_cap,
    size_t *out_code_len, char *state_buf, size_t state_cap,
    size_t *out_state_len, int space_pad) {
  char *code = NULL;
  char *state = NULL;
  char *err_s = NULL;
  char *err_desc = NULL;
  c_abstract_http_error_t crc;
  fah_error_code_t rc1;
  fah_error_code_t rc2;

  if (html_resp == NULL) {
    return FAH_ERR_INVAL;
  }
  crc = http_oauth2_localhost_intercept(port, html_resp, &code, &state, &err_s,
                                        &err_desc);
  fah_free(err_s);
  fah_free(err_desc);
  if (crc != C_ABSTRACT_HTTP_SUCCESS) {
    fah_free(code);
    fah_free(state);
    return fah_error_from_c(crc);
  }
  rc1 = fah_cbytes_to_fstr((code != NULL) ? code : "",
                           (code != NULL) ? strlen(code) : 0u, code_buf,
                           code_cap, out_code_len, space_pad);
  rc2 = fah_cbytes_to_fstr((state != NULL) ? state : "",
                           (state != NULL) ? strlen(state) : 0u, state_buf,
                           state_cap, out_state_len, space_pad);
  fah_free(code);
  fah_free(state);
  return (rc1 != FAH_OK) ? rc1 : rc2;
}

/**
 * @brief Wrap a heap-allocated `struct HttpResponse *` in a `FahResponseBox`
 * and register it as `FAH_TAG_RESPONSE`.
 * @param[in] heap_res Heap `struct HttpResponse *` from `c-abstract-http`.
 * @param[out] out_res_h Receives new handle.
 * @return `FAH_OK` or error code (takes ownership and frees on error).
 */
static fah_error_code_t fah_box_heap_response(struct HttpResponse *heap_res,
                                              fah_handle_t *out_res_h) {
  void *raw = NULL;
  struct FahResponseBox *rbox;
  fah_error_code_t rc = fah_calloc(1u, sizeof(struct FahResponseBox), &raw);
  if (rc != FAH_OK) {
    FAH_IGNORE_RC(http_response_free(heap_res));
    fah_free(heap_res);
    return rc;
  }
  rbox = (struct FahResponseBox *)raw;
  rbox->res = heap_res;
  rbox->owns_outer_struct = 1;
  rc = fah_handle_register(FAH_TAG_RESPONSE, rbox, out_res_h);
  if (rc != FAH_OK) {
    FAH_IGNORE_RC(http_response_free(heap_res));
    fah_free(heap_res);
    fah_free(rbox);
    return rc;
  }
  return FAH_OK;
}

fah_error_code_t fah_send(fah_handle_t cli_h, fah_handle_t req_h,
                          fah_handle_t *out_res_h) {
  void *raw_cli = NULL;
  void *raw_req = NULL;
  struct FahClientBox *cbox;
  struct FahRequestBox *rbox;
  struct HttpResponse *c_res = NULL;
  c_abstract_http_error_t crc;
  fah_error_code_t rc;

  if (out_res_h == NULL) {
    return FAH_ERR_INVAL;
  }
  *out_res_h = 0;

  rc = fah_handle_resolve(cli_h, FAH_TAG_CLIENT, &raw_cli);
  if (rc != FAH_OK) {
    return rc;
  }
  rc = fah_handle_resolve(req_h, FAH_TAG_REQUEST, &raw_req);
  if (rc != FAH_OK) {
    return rc;
  }
  cbox = (struct FahClientBox *)raw_cli;
  rbox = (struct FahRequestBox *)raw_req;

  crc = http_client_send(&cbox->client, &rbox->req, &c_res);
  if (crc != C_ABSTRACT_HTTP_SUCCESS || c_res == NULL) {
    return fah_error_from_c(crc);
  }
  return fah_box_heap_response(c_res, out_res_h);
}

fah_error_code_t fah_response_status(fah_handle_t res_h, int *out_status) {
  void *raw = NULL;
  fah_error_code_t rc;
  if (out_status == NULL) {
    return FAH_ERR_INVAL;
  }
  rc = fah_handle_resolve(res_h, FAH_TAG_RESPONSE, &raw);
  if (rc != FAH_OK) {
    return rc;
  }
  *out_status = ((const struct FahResponseBox *)raw)->res->status_code;
  return FAH_OK;
}

fah_error_code_t fah_response_body_len(fah_handle_t res_h, size_t *out_len) {
  void *raw = NULL;
  fah_error_code_t rc;
  if (out_len == NULL) {
    return FAH_ERR_INVAL;
  }
  rc = fah_handle_resolve(res_h, FAH_TAG_RESPONSE, &raw);
  if (rc != FAH_OK) {
    return rc;
  }
  *out_len = ((const struct FahResponseBox *)raw)->res->body_len;
  return FAH_OK;
}

fah_error_code_t fah_response_copy_body(fah_handle_t res_h, void *out_buf,
                                        size_t cap, size_t *out_len,
                                        int space_pad) {
  void *raw = NULL;
  const struct HttpResponse *res;
  fah_error_code_t rc = fah_handle_resolve(res_h, FAH_TAG_RESPONSE, &raw);
  if (rc != FAH_OK) {
    return rc;
  }
  res = ((const struct FahResponseBox *)raw)->res;
  return fah_cbytes_to_fstr(res->body, res->body_len, (char *)out_buf, cap,
                            out_len, space_pad);
}

fah_error_code_t fah_response_body_ptr(fah_handle_t res_h, const void **out_ptr,
                                       size_t *out_len) {
  void *raw = NULL;
  const struct HttpResponse *res;
  fah_error_code_t rc;

  if (out_ptr == NULL || out_len == NULL) {
    return FAH_ERR_INVAL;
  }
  rc = fah_handle_resolve(res_h, FAH_TAG_RESPONSE, &raw);
  if (rc != FAH_OK) {
    return rc;
  }
  res = ((const struct FahResponseBox *)raw)->res;
  *out_ptr = res->body;
  *out_len = res->body_len;
  return FAH_OK;
}

fah_error_code_t fah_response_body_cfi(fah_handle_t res_h,
                                       fah_cfi_cdesc_t *desc) {
  void *raw = NULL;
  const struct HttpResponse *res;
  fah_error_code_t rc = fah_handle_resolve(res_h, FAH_TAG_RESPONSE, &raw);
  if (rc != FAH_OK) {
    return rc;
  }
  res = ((const struct FahResponseBox *)raw)->res;
  return fah_cfi_populate_allocatable_bytes(desc, res->body, res->body_len);
}

fah_error_code_t fah_response_get_header(fah_handle_t res_h, const char *key,
                                         char *out_buf, size_t cap,
                                         size_t *out_len, int space_pad) {
  void *raw = NULL;
  const char *val = NULL;
  const struct HttpResponse *res;
  c_abstract_http_error_t crc;
  fah_error_code_t rc;

  if (key == NULL) {
    return FAH_ERR_INVAL;
  }
  rc = fah_handle_resolve(res_h, FAH_TAG_RESPONSE, &raw);
  if (rc != FAH_OK) {
    return rc;
  }
  res = ((const struct FahResponseBox *)raw)->res;
  crc = http_headers_get(&res->headers, key, &val);
  if (crc != C_ABSTRACT_HTTP_SUCCESS || val == NULL) {
    return fah_error_from_c(crc);
  }
  return fah_cbytes_to_fstr(val, strlen(val), out_buf, cap, out_len, space_pad);
}

fah_error_code_t fah_response_header_count(fah_handle_t res_h,
                                           size_t *out_count) {
  void *raw = NULL;
  fah_error_code_t rc;
  if (out_count == NULL) {
    return FAH_ERR_INVAL;
  }
  rc = fah_handle_resolve(res_h, FAH_TAG_RESPONSE, &raw);
  if (rc != FAH_OK) {
    return rc;
  }
  *out_count = ((const struct FahResponseBox *)raw)->res->headers.count;
  return FAH_OK;
}

fah_error_code_t fah_response_header_at(fah_handle_t res_h, size_t idx,
                                        char *key_buf, size_t key_cap,
                                        size_t *out_key_len, char *val_buf,
                                        size_t val_cap, size_t *out_val_len,
                                        int space_pad) {
  void *raw = NULL;
  const struct HttpResponse *res;
  const struct HttpHeader *hdr;
  fah_error_code_t rc;
  fah_error_code_t rc1;
  fah_error_code_t rc2;

  rc = fah_handle_resolve(res_h, FAH_TAG_RESPONSE, &raw);
  if (rc != FAH_OK) {
    return rc;
  }
  res = ((const struct FahResponseBox *)raw)->res;
  if (idx >= res->headers.count) {
    return FAH_ERR_INVAL;
  }
  hdr = &res->headers.headers[idx];
  rc1 = fah_cbytes_to_fstr(hdr->key, strlen(hdr->key), key_buf, key_cap,
                           out_key_len, space_pad);
  rc2 = fah_cbytes_to_fstr(hdr->value, strlen(hdr->value), val_buf, val_cap,
                           out_val_len, space_pad);
  return (rc1 != FAH_OK) ? rc1 : rc2;
}

fah_error_code_t fah_response_save_file(fah_handle_t res_h,
                                        const char *file_path) {
  void *raw = NULL;
  c_abstract_http_error_t crc;
  fah_error_code_t rc;

  if (file_path == NULL) {
    return FAH_ERR_INVAL;
  }
  rc = fah_handle_resolve(res_h, FAH_TAG_RESPONSE, &raw);
  if (rc != FAH_OK) {
    return rc;
  }
  crc = http_response_save_to_file(((const struct FahResponseBox *)raw)->res,
                                   file_path);
  return fah_error_from_c(crc);
}

fah_error_code_t fah_response_destroy(fah_handle_t res_h) {
  void *raw = NULL;
  struct FahResponseBox *rbox;
  c_abstract_http_error_t crc;
  fah_error_code_t rc = fah_handle_unregister(res_h, FAH_TAG_RESPONSE, &raw);
  if (rc != FAH_OK) {
    return rc;
  }
  rbox = (struct FahResponseBox *)raw;
  crc = http_response_free(rbox->res);
  if (rbox->owns_outer_struct) {
    fah_free(rbox->res);
  }
  fah_free(rbox);
  return fah_error_from_c(crc);
}

/* --- Multi-Request & Futures --- */

fah_error_code_t fah_multi_create(fah_handle_t *out_multi_h) {
  void *raw = NULL;
  struct HttpMultiRequest *multi;
  fah_error_code_t rc;

  if (out_multi_h == NULL) {
    return FAH_ERR_INVAL;
  }
  rc = fah_calloc(1u, sizeof(struct HttpMultiRequest), &raw);
  if (rc != FAH_OK) {
    return rc;
  }
  multi = (struct HttpMultiRequest *)raw;
  FAH_IGNORE_RC(http_multi_request_init(multi));
  rc = fah_handle_register(FAH_TAG_MULTI_REQ, multi, out_multi_h);
  if (rc != FAH_OK) {
    FAH_IGNORE_RC(http_multi_request_free(multi));
    fah_free(multi);
    return rc;
  }
  return FAH_OK;
}

fah_error_code_t fah_multi_add(fah_handle_t multi_h, fah_handle_t req_h) {
  void *raw_m = NULL;
  void *raw_r = NULL;
  c_abstract_http_error_t crc;
  fah_error_code_t rc = fah_handle_resolve(multi_h, FAH_TAG_MULTI_REQ, &raw_m);
  if (rc != FAH_OK) {
    return rc;
  }
  rc = fah_handle_resolve(req_h, FAH_TAG_REQUEST, &raw_r);
  if (rc != FAH_OK) {
    return rc;
  }
  crc = http_multi_request_add((struct HttpMultiRequest *)raw_m,
                               &((struct FahRequestBox *)raw_r)->req);
  return fah_error_from_c(crc);
}

fah_error_code_t fah_multi_send(fah_handle_t cli_h, fah_handle_t multi_h,
                                int fail_fast, fah_handle_t *out_future_handles,
                                size_t cap_futures, size_t *out_count) {
  void *raw_c = NULL;
  void *raw_m = NULL;
  struct FahClientBox *cbox;
  struct HttpMultiRequest *multi;
  void *raw_arr = NULL;
  struct HttpFuture **fut_ptrs;
  size_t n;
  size_t i;
  c_abstract_http_error_t crc;
  fah_error_code_t rc;

  if (out_future_handles == NULL || out_count == NULL) {
    return FAH_ERR_INVAL;
  }
  *out_count = 0u;

  rc = fah_handle_resolve(cli_h, FAH_TAG_CLIENT, &raw_c);
  if (rc != FAH_OK) {
    return rc;
  }
  rc = fah_handle_resolve(multi_h, FAH_TAG_MULTI_REQ, &raw_m);
  if (rc != FAH_OK) {
    return rc;
  }
  cbox = (struct FahClientBox *)raw_c;
  multi = (struct HttpMultiRequest *)raw_m;
  n = multi->count;
  if (n == 0u) {
    return FAH_ERR_INVAL;
  }
  if (n > cap_futures) {
    *out_count = n;
    return FAH_ERR_BUF_TOO_SMALL;
  }

  rc = fah_calloc(n, sizeof(struct HttpFuture *), &raw_arr);
  if (rc != FAH_OK) {
    return rc;
  }
  fut_ptrs = (struct HttpFuture **)raw_arr;

  for (i = 0u; i < n; i++) {
    void *rf = NULL;
    rc = fah_calloc(1u, sizeof(struct HttpFuture), &rf);
    if (rc != FAH_OK) {
      size_t k;
      for (k = 0u; k < i; k++) {
        FAH_IGNORE_RC(http_future_free(fut_ptrs[k]));
        fah_free(fut_ptrs[k]);
      }
      fah_free(fut_ptrs);
      return rc;
    }
    fut_ptrs[i] = (struct HttpFuture *)rf;
    FAH_IGNORE_RC(http_future_init(fut_ptrs[i]));
  }

  crc = http_client_send_multi(&cbox->client, multi->requests, n, fut_ptrs,
                               NULL, NULL, (fail_fast != 0) ? 1 : 0);
  if (crc != C_ABSTRACT_HTTP_SUCCESS) {
    for (i = 0u; i < n; i++) {
      if (fut_ptrs[i]->response != NULL) {
        FAH_IGNORE_RC(http_response_free(fut_ptrs[i]->response));
        fah_free(fut_ptrs[i]->response);
      }
      FAH_IGNORE_RC(http_future_free(fut_ptrs[i]));
      fah_free(fut_ptrs[i]);
    }
    fah_free(fut_ptrs);
    return fah_error_from_c(crc);
  }

  for (i = 0u; i < n; i++) {
    rc = fah_handle_register(FAH_TAG_FUTURE, fut_ptrs[i],
                             &out_future_handles[i]);
    if (rc != FAH_OK) {
      size_t k;
      for (k = 0u; k < i; k++) {
        void *u = NULL;
        FAH_IGNORE_RC(
            fah_handle_unregister(out_future_handles[k], FAH_TAG_FUTURE, &u));
      }
      for (k = 0u; k < n; k++) {
        if (fut_ptrs[k]->response != NULL) {
          FAH_IGNORE_RC(http_response_free(fut_ptrs[k]->response));
          fah_free(fut_ptrs[k]->response);
        }
        FAH_IGNORE_RC(http_future_free(fut_ptrs[k]));
        fah_free(fut_ptrs[k]);
      }
      fah_free(fut_ptrs);
      return rc;
    }
  }
  fah_free(fut_ptrs);
  *out_count = n;
  return FAH_OK;
}

fah_error_code_t fah_multi_destroy(fah_handle_t multi_h) {
  void *raw = NULL;
  struct HttpMultiRequest *multi;
  c_abstract_http_error_t crc;
  fah_error_code_t rc = fah_handle_unregister(multi_h, FAH_TAG_MULTI_REQ, &raw);
  if (rc != FAH_OK) {
    return rc;
  }
  multi = (struct HttpMultiRequest *)raw;
  crc = http_multi_request_free(multi);
  fah_free(multi);
  return fah_error_from_c(crc);
}

fah_error_code_t fah_future_await(fah_handle_t fut_h,
                                  fah_error_code_t *out_xfer_err) {
  void *raw = NULL;
  c_abstract_http_error_t crc;
  fah_error_code_t rc;
  if (out_xfer_err == NULL) {
    return FAH_ERR_INVAL;
  }
  rc = fah_handle_resolve(fut_h, FAH_TAG_FUTURE, &raw);
  if (rc != FAH_OK) {
    return rc;
  }
  crc = http_future_await((struct HttpFuture *)raw);
  *out_xfer_err = fah_error_from_c(crc);
  return FAH_OK;
}

fah_error_code_t fah_future_cancel(fah_handle_t fut_h) {
  void *raw = NULL;
  c_abstract_http_error_t crc;
  fah_error_code_t rc = fah_handle_resolve(fut_h, FAH_TAG_FUTURE, &raw);
  if (rc != FAH_OK) {
    return rc;
  }
  crc = http_future_cancel((struct HttpFuture *)raw);
  return fah_error_from_c(crc);
}

fah_error_code_t fah_future_poll(fah_handle_t fut_h, int *out_is_ready,
                                 int *out_err_code) {
  void *raw = NULL;
  const struct HttpFuture *fut;
  fah_error_code_t rc;
  if (out_is_ready == NULL || out_err_code == NULL) {
    return FAH_ERR_INVAL;
  }
  rc = fah_handle_resolve(fut_h, FAH_TAG_FUTURE, &raw);
  if (rc != FAH_OK) {
    return rc;
  }
  fut = (const struct HttpFuture *)raw;
  *out_is_ready = fut->is_ready;
  *out_err_code = fut->error_code;
  return FAH_OK;
}

fah_error_code_t fah_future_take_response(fah_handle_t fut_h,
                                          fah_handle_t *out_res_h) {
  void *raw = NULL;
  struct HttpFuture *fut;
  struct HttpResponse *detached;
  fah_error_code_t rc;

  if (out_res_h == NULL) {
    return FAH_ERR_INVAL;
  }
  *out_res_h = 0;
  rc = fah_handle_resolve(fut_h, FAH_TAG_FUTURE, &raw);
  if (rc != FAH_OK) {
    return rc;
  }
  fut = (struct HttpFuture *)raw;
  if (!fut->is_ready || fut->response == NULL) {
    return FAH_ERR_INVAL;
  }
  detached = fut->response;
  fut->response = NULL;
  return fah_box_heap_response(detached, out_res_h);
}

fah_error_code_t fah_future_destroy(fah_handle_t fut_h) {
  void *raw = NULL;
  struct HttpFuture *fut;
  c_abstract_http_error_t crc;
  fah_error_code_t rc = fah_handle_unregister(fut_h, FAH_TAG_FUTURE, &raw);
  if (rc != FAH_OK) {
    return rc;
  }
  fut = (struct HttpFuture *)raw;
  if (fut->response != NULL) {
    FAH_IGNORE_RC(http_response_free(fut->response));
    fah_free(fut->response);
    fut->response = NULL;
  }
  crc = http_future_free(fut);
  fah_free(fut);
  return fah_error_from_c(crc);
}

/* --- WebSockets & SSE --- */

fah_error_code_t fah_ws_init(fah_handle_t req_h, const char *subprotocols,
                             int masking_pref) {
  void *raw = NULL;
  struct FahRequestBox *rbox;
  struct c_abstract_http_ws_config cfg;
  c_abstract_http_error_t crc;
  fah_error_code_t rc = fah_handle_resolve(req_h, FAH_TAG_REQUEST, &raw);
  if (rc != FAH_OK) {
    return rc;
  }
  rbox = (struct FahRequestBox *)raw;
  memset(&cfg, 0, sizeof(cfg));
  cfg.subprotocols = fah_opt_str(subprotocols);
  cfg.masking_preference = (masking_pref != 0) ? 1 : 0;
  crc = c_abstract_http_ws_init(&rbox->req, &cfg);
  return fah_error_from_c(crc);
}

fah_error_code_t fah_ws_sync_loop(fah_handle_t cli_h, fah_handle_t req_h,
                                  fah_f77_ws_on_msg_cb on_msg,
                                  fah_f77_ws_on_err_cb on_err,
                                  fah_f77_ws_on_close_cb on_close, int user_id,
                                  fah_handle_t poll_q_h) {
  void *raw_c = NULL;
  void *raw_r = NULL;
  struct FahClientBox *cbox;
  struct FahRequestBox *rbox;
  c_abstract_http_error_t crc;
  fah_error_code_t rc = fah_handle_resolve(cli_h, FAH_TAG_CLIENT, &raw_c);
  if (rc != FAH_OK) {
    return rc;
  }
  rc = fah_handle_resolve(req_h, FAH_TAG_REQUEST, &raw_r);
  if (rc != FAH_OK) {
    return rc;
  }
  if (poll_q_h != 0) {
    void *rq = NULL;
    rc = fah_handle_resolve(poll_q_h, FAH_TAG_POLL_QUEUE, &rq);
    if (rc != FAH_OK) {
      return rc;
    }
  }
  cbox = (struct FahClientBox *)raw_c;
  rbox = (struct FahRequestBox *)raw_r;
  rbox->ws_cb.on_msg = on_msg;
  rbox->ws_cb.on_err = on_err;
  rbox->ws_cb.on_close = on_close;
  rbox->ws_cb.user_id = user_id;
  rbox->ws_cb.poll_queue_h = poll_q_h;

  {
    struct HttpClient ws_cli = cbox->client;
    ws_cli.send = NULL;
    crc = c_abstract_http_ws_sync_read_loop(
        &ws_cli, &rbox->req, fah_trampoline_ws_on_msg, fah_trampoline_ws_on_err,
        fah_trampoline_ws_on_close, &rbox->ws_cb, &rbox->cancel_flag);
  }
  return fah_error_from_c(crc);
}

fah_error_code_t fah_ws_async_connect(fah_handle_t cli_h, fah_handle_t loop_h,
                                      fah_handle_t req_h,
                                      fah_f77_ws_on_msg_cb on_msg,
                                      fah_f77_ws_on_err_cb on_err,
                                      fah_f77_ws_on_close_cb on_close,
                                      int user_id, fah_handle_t poll_q_h) {
  void *raw_c = NULL;
  void *raw_l = NULL;
  void *raw_r = NULL;
  struct FahClientBox *cbox;
  struct FahLoopBox *lbox;
  struct FahRequestBox *rbox;
  c_abstract_http_error_t crc;
  fah_error_code_t rc = fah_handle_resolve(cli_h, FAH_TAG_CLIENT, &raw_c);
  if (rc != FAH_OK) {
    return rc;
  }
  rc = fah_handle_resolve(loop_h, FAH_TAG_EVENT_LOOP, &raw_l);
  if (rc != FAH_OK) {
    return rc;
  }
  rc = fah_handle_resolve(req_h, FAH_TAG_REQUEST, &raw_r);
  if (rc != FAH_OK) {
    return rc;
  }
  if (poll_q_h != 0) {
    void *rq = NULL;
    rc = fah_handle_resolve(poll_q_h, FAH_TAG_POLL_QUEUE, &rq);
    if (rc != FAH_OK) {
      return rc;
    }
  }
  cbox = (struct FahClientBox *)raw_c;
  lbox = (struct FahLoopBox *)raw_l;
  rbox = (struct FahRequestBox *)raw_r;
  cbox->client.loop = lbox->loop;
  rbox->ws_cb.on_msg = on_msg;
  rbox->ws_cb.on_err = on_err;
  rbox->ws_cb.on_close = on_close;
  rbox->ws_cb.user_id = user_id;
  rbox->ws_cb.poll_queue_h = poll_q_h;

  crc = c_abstract_http_ws_async_connect(
      &cbox->client, &rbox->req, fah_trampoline_ws_on_msg,
      fah_trampoline_ws_on_err, fah_trampoline_ws_on_close, &rbox->ws_cb);
  return fah_error_from_c(crc);
}

fah_error_code_t fah_ws_send(fah_handle_t req_h, int opcode,
                             const void *payload, size_t len, int is_async) {
  void *raw = NULL;
  struct FahRequestBox *rbox;
  const unsigned char *bytes = (payload != NULL)
                                   ? (const unsigned char *)payload
                                   : (const unsigned char *)"";
  c_abstract_http_error_t crc;
  fah_error_code_t rc;

  if (len > 0u && payload == NULL) {
    return FAH_ERR_INVALID_SLICE;
  }
  rc = fah_handle_resolve(req_h, FAH_TAG_REQUEST, &raw);
  if (rc != FAH_OK) {
    return rc;
  }
  rbox = (struct FahRequestBox *)raw;
  if (is_async) {
    crc = c_abstract_http_ws_send_async(
        &rbox->req, (enum c_abstract_http_ws_opcode)opcode, bytes, len);
  } else {
    crc = c_abstract_http_ws_send(
        &rbox->req, (enum c_abstract_http_ws_opcode)opcode, bytes, len);
  }
  return fah_error_from_c(crc);
}

fah_error_code_t fah_ws_close(fah_handle_t req_h, int status_code) {
  void *raw = NULL;
  c_abstract_http_error_t crc;
  fah_error_code_t rc = fah_handle_resolve(req_h, FAH_TAG_REQUEST, &raw);
  if (rc != FAH_OK) {
    return rc;
  }
  crc = c_abstract_http_ws_close(&((struct FahRequestBox *)raw)->req,
                                 status_code);
  return fah_error_from_c(crc);
}

fah_error_code_t fah_ws_free(fah_handle_t req_h) {
  void *raw = NULL;
  c_abstract_http_error_t crc;
  fah_error_code_t rc = fah_handle_resolve(req_h, FAH_TAG_REQUEST, &raw);
  if (rc != FAH_OK) {
    return rc;
  }
  crc = c_abstract_http_ws_free(&((struct FahRequestBox *)raw)->req);
  return fah_error_from_c(crc);
}

fah_error_code_t fah_sse_init(fah_handle_t req_h, const char *last_event_id,
                              int retry_ms) {
  void *raw = NULL;
  struct FahRequestBox *rbox;
  struct c_abstract_http_sse_config cfg;
  c_abstract_http_error_t crc;
  fah_error_code_t rc;

  if (retry_ms < 0) {
    return FAH_ERR_INVAL;
  }
  rc = fah_handle_resolve(req_h, FAH_TAG_REQUEST, &raw);
  if (rc != FAH_OK) {
    return rc;
  }
  rbox = (struct FahRequestBox *)raw;
  memset(&cfg, 0, sizeof(cfg));
  cfg.last_event_id = fah_opt_str(last_event_id);
  cfg.retry_timeout_ms = retry_ms;
  crc = c_abstract_http_sse_init(&rbox->req, &cfg);
  return fah_error_from_c(crc);
}

fah_error_code_t fah_sse_sync_loop(fah_handle_t cli_h, fah_handle_t req_h,
                                   fah_f77_sse_on_evt_cb on_evt,
                                   fah_f77_sse_on_err_cb on_err,
                                   fah_f77_sse_on_close_cb on_close,
                                   int user_id, fah_handle_t poll_q_h) {
  void *raw_c = NULL;
  void *raw_r = NULL;
  struct FahClientBox *cbox;
  struct FahRequestBox *rbox;
  c_abstract_http_error_t crc;
  fah_error_code_t rc = fah_handle_resolve(cli_h, FAH_TAG_CLIENT, &raw_c);
  if (rc != FAH_OK) {
    return rc;
  }
  rc = fah_handle_resolve(req_h, FAH_TAG_REQUEST, &raw_r);
  if (rc != FAH_OK) {
    return rc;
  }
  if (poll_q_h != 0) {
    void *rq = NULL;
    rc = fah_handle_resolve(poll_q_h, FAH_TAG_POLL_QUEUE, &rq);
    if (rc != FAH_OK) {
      return rc;
    }
  }
  cbox = (struct FahClientBox *)raw_c;
  rbox = (struct FahRequestBox *)raw_r;
  rbox->sse_cb.on_evt = on_evt;
  rbox->sse_cb.on_err = on_err;
  rbox->sse_cb.on_close = on_close;
  rbox->sse_cb.user_id = user_id;
  rbox->sse_cb.poll_queue_h = poll_q_h;

  crc = c_abstract_http_sse_sync_read_loop(
      &cbox->client, &rbox->req, fah_trampoline_sse_on_evt,
      fah_trampoline_sse_on_err, fah_trampoline_sse_on_close, &rbox->sse_cb,
      &rbox->cancel_flag);
  return fah_error_from_c(crc);
}

fah_error_code_t fah_sse_async_register(fah_handle_t cli_h, fah_handle_t loop_h,
                                        fah_handle_t req_h,
                                        fah_f77_sse_on_evt_cb on_evt,
                                        fah_f77_sse_on_err_cb on_err,
                                        fah_f77_sse_on_close_cb on_close,
                                        int user_id, fah_handle_t poll_q_h) {
  void *raw_c = NULL;
  void *raw_l = NULL;
  void *raw_r = NULL;
  struct FahClientBox *cbox;
  struct FahLoopBox *lbox;
  struct FahRequestBox *rbox;
  c_abstract_http_error_t crc;
  fah_error_code_t rc = fah_handle_resolve(cli_h, FAH_TAG_CLIENT, &raw_c);
  if (rc != FAH_OK) {
    return rc;
  }
  rc = fah_handle_resolve(loop_h, FAH_TAG_EVENT_LOOP, &raw_l);
  if (rc != FAH_OK) {
    return rc;
  }
  rc = fah_handle_resolve(req_h, FAH_TAG_REQUEST, &raw_r);
  if (rc != FAH_OK) {
    return rc;
  }
  if (poll_q_h != 0) {
    void *rq = NULL;
    rc = fah_handle_resolve(poll_q_h, FAH_TAG_POLL_QUEUE, &rq);
    if (rc != FAH_OK) {
      return rc;
    }
  }
  cbox = (struct FahClientBox *)raw_c;
  lbox = (struct FahLoopBox *)raw_l;
  rbox = (struct FahRequestBox *)raw_r;
  cbox->client.loop = lbox->loop;
  rbox->sse_cb.on_evt = on_evt;
  rbox->sse_cb.on_err = on_err;
  rbox->sse_cb.on_close = on_close;
  rbox->sse_cb.user_id = user_id;
  rbox->sse_cb.poll_queue_h = poll_q_h;

  crc = c_abstract_http_sse_async_register(
      &cbox->client, &rbox->req, fah_trampoline_sse_on_evt,
      fah_trampoline_sse_on_err, fah_trampoline_sse_on_close, &rbox->sse_cb);
  return fah_error_from_c(crc);
}

fah_error_code_t fah_sse_free(fah_handle_t req_h) {
  void *raw = NULL;
  c_abstract_http_error_t crc;
  fah_error_code_t rc = fah_handle_resolve(req_h, FAH_TAG_REQUEST, &raw);
  if (rc != FAH_OK) {
    return rc;
  }
  crc = c_abstract_http_sse_free(&((struct FahRequestBox *)raw)->req);
  return fah_error_from_c(crc);
}

/* --- Execution & Concurrency Modalities --- */

fah_error_code_t fah_loop_create(int backend, fah_handle_t *out_loop_h) {
  void *raw = NULL;
  struct FahLoopBox *lbox;
  c_abstract_http_error_t crc;
  fah_error_code_t rc;

  if (out_loop_h == NULL || backend < (int)HTTP_LOOP_BACKEND_AUTO ||
      backend > (int)HTTP_LOOP_BACKEND_IOCP) {
    return FAH_ERR_INVAL;
  }
  rc = fah_calloc(1u, sizeof(struct FahLoopBox), &raw);
  if (rc != FAH_OK) {
    return rc;
  }
  lbox = (struct FahLoopBox *)raw;
  crc = http_loop_init_with_backend(&lbox->loop, (enum HttpLoopBackend)backend);
  if (crc != C_ABSTRACT_HTTP_SUCCESS) {
    fah_free(lbox);
    return fah_error_from_c(crc);
  }
  rc = fah_handle_register(FAH_TAG_EVENT_LOOP, lbox, out_loop_h);
  if (rc != FAH_OK) {
    FAH_IGNORE_RC(http_loop_free(lbox->loop));
    fah_free(lbox);
    return rc;
  }
  return FAH_OK;
}

fah_error_code_t fah_loop_get_backend(fah_handle_t loop_h, int *out_backend) {
  void *raw = NULL;
  enum HttpLoopBackend b = HTTP_LOOP_BACKEND_AUTO;
  c_abstract_http_error_t crc;
  fah_error_code_t rc;
  if (out_backend == NULL) {
    return FAH_ERR_INVAL;
  }
  rc = fah_handle_resolve(loop_h, FAH_TAG_EVENT_LOOP, &raw);
  if (rc != FAH_OK) {
    return rc;
  }
  crc = http_loop_get_backend(((const struct FahLoopBox *)raw)->loop, &b);
  *out_backend = (int)b;
  return fah_error_from_c(crc);
}

fah_error_code_t fah_loop_drive(fah_handle_t loop_h, int mode) {
  void *raw = NULL;
  struct ModalityEventLoop *lp;
  c_abstract_http_error_t crc;
  fah_error_code_t rc = fah_handle_resolve(loop_h, FAH_TAG_EVENT_LOOP, &raw);
  if (rc != FAH_OK) {
    return rc;
  }
  lp = ((struct FahLoopBox *)raw)->loop;
  switch (mode) {
  case 0:
    crc = http_loop_tick(lp);
    break;
  case 1:
    crc = http_loop_run(lp);
    break;
  case 2:
    crc = http_loop_stop(lp);
    break;
  case 3:
    crc = http_loop_wakeup(lp);
    break;
  default:
    return FAH_ERR_INVAL;
  }
  return fah_error_from_c(crc);
}

/**
 * @brief Internal `http_timer_cb` trampoline invoking a Fortran timer callback.
 * @param[in] loop Underlying C event loop (unused; `loop_h` stored in closure).
 * @param[in] timer_id Fired timer identifier.
 * @param[in] user_data `FahTimerClosure *`.
 * @return `C_ABSTRACT_HTTP_SUCCESS` or `C_ABSTRACT_HTTP_ERR_CANCELED`.
 */
static c_abstract_http_error_t
fah_timer_trampoline(struct ModalityEventLoop *loop, int timer_id,
                     void *user_data) {
  const struct FahTimerClosure *tc = (const struct FahTimerClosure *)user_data;
  int tid = timer_id;
  int cb_rc;
  (void)loop;
  cb_rc = tc->cb(&tc->loop_h, &tid, &tc->user_id);
  return (cb_rc == 0) ? C_ABSTRACT_HTTP_SUCCESS : C_ABSTRACT_HTTP_ERR_CANCELED;
}

fah_error_code_t fah_loop_add_timer(fah_handle_t loop_h, long timeout_ms,
                                    fah_f77_loop_timer_cb cb, int user_id,
                                    int *out_timer_id) {
  void *raw = NULL;
  void *raw_tc = NULL;
  struct FahLoopBox *lbox;
  struct FahTimerClosure *tc;
  fah_error_code_t rc;

  if (cb == NULL || out_timer_id == NULL || timeout_ms < 0L) {
    return FAH_ERR_INVAL;
  }
  rc = fah_handle_resolve(loop_h, FAH_TAG_EVENT_LOOP, &raw);
  if (rc != FAH_OK) {
    return rc;
  }
  lbox = (struct FahLoopBox *)raw;
  rc = fah_calloc(1u, sizeof(struct FahTimerClosure), &raw_tc);
  if (rc != FAH_OK) {
    return rc;
  }
  tc = (struct FahTimerClosure *)raw_tc;
  tc->loop_h = loop_h;
  tc->cb = cb;
  tc->user_id = user_id;

  FAH_IGNORE_RC(http_loop_add_timer(lbox->loop, timeout_ms,
                                    fah_timer_trampoline, tc, out_timer_id));
  tc->next = lbox->timers;
  lbox->timers = tc;
  return FAH_OK;
}

fah_error_code_t fah_loop_cancel_timer(fah_handle_t loop_h, int timer_id) {
  void *raw = NULL;
  c_abstract_http_error_t crc;
  fah_error_code_t rc = fah_handle_resolve(loop_h, FAH_TAG_EVENT_LOOP, &raw);
  if (rc != FAH_OK) {
    return rc;
  }
  crc = http_loop_cancel_timer(((struct FahLoopBox *)raw)->loop, timer_id);
  return fah_error_from_c(crc);
}

fah_error_code_t fah_loop_destroy(fah_handle_t loop_h) {
  void *raw = NULL;
  struct FahLoopBox *lbox;
  struct FahTimerClosure *tc;
  c_abstract_http_error_t crc;
  fah_error_code_t rc = fah_handle_unregister(loop_h, FAH_TAG_EVENT_LOOP, &raw);
  if (rc != FAH_OK) {
    return rc;
  }
  lbox = (struct FahLoopBox *)raw;
  crc = http_loop_free(lbox->loop);
  tc = lbox->timers;
  while (tc != NULL) {
    struct FahTimerClosure *nxt = tc->next;
    fah_free(tc);
    tc = nxt;
  }
  fah_free(lbox);
  return fah_error_from_c(crc);
}

fah_error_code_t fah_time_ms(abstract_http_int64_t *out_ms) {
  if (out_ms == NULL) {
    return FAH_ERR_INVAL;
  }
  *out_ms = real_math_get_current_time_ms();
  return FAH_OK;
}

fah_error_code_t fah_mutex_op(fah_handle_t *io_mtx_h, int op) {
  void *raw = NULL;
  c_abstract_http_error_t crc;
  fah_error_code_t rc;

  if (io_mtx_h == NULL) {
    return FAH_ERR_INVAL;
  }
  if (op == 0) {
    struct AbstractHttpMutex *mtx = NULL;
    FAH_IGNORE_RC(abstract_http_mutex_init(&mtx));
    rc = fah_handle_register(FAH_TAG_MUTEX, mtx, io_mtx_h);
    if (rc != FAH_OK) {
      FAH_IGNORE_RC(abstract_http_mutex_free(mtx));
      return rc;
    }
    return FAH_OK;
  }
  if (op == 3) {
    rc = fah_handle_unregister(*io_mtx_h, FAH_TAG_MUTEX, &raw);
    if (rc != FAH_OK) {
      return rc;
    }
    *io_mtx_h = 0;
    crc = abstract_http_mutex_free((struct AbstractHttpMutex *)raw);
    return fah_error_from_c(crc);
  }
  rc = fah_handle_resolve(*io_mtx_h, FAH_TAG_MUTEX, &raw);
  if (rc != FAH_OK) {
    return rc;
  }
  if (op == 1) {
    crc = abstract_http_mutex_lock((struct AbstractHttpMutex *)raw);
    return fah_error_from_c(crc);
  }
  if (op == 2) {
    crc = abstract_http_mutex_unlock((struct AbstractHttpMutex *)raw);
    return fah_error_from_c(crc);
  }
  return FAH_ERR_INVAL;
}

fah_error_code_t fah_cond_op(fah_handle_t *io_cond_h, int op) {
  void *raw = NULL;
  c_abstract_http_error_t crc;
  fah_error_code_t rc;

  if (io_cond_h == NULL) {
    return FAH_ERR_INVAL;
  }
  if (op == 0) {
    struct AbstractHttpCond *cond = NULL;
    FAH_IGNORE_RC(abstract_http_cond_init(&cond));
    rc = fah_handle_register(FAH_TAG_COND, cond, io_cond_h);
    if (rc != FAH_OK) {
      FAH_IGNORE_RC(abstract_http_cond_free(cond));
      return rc;
    }
    return FAH_OK;
  }
  if (op == 3) {
    rc = fah_handle_unregister(*io_cond_h, FAH_TAG_COND, &raw);
    if (rc != FAH_OK) {
      return rc;
    }
    *io_cond_h = 0;
    crc = abstract_http_cond_free((struct AbstractHttpCond *)raw);
    return fah_error_from_c(crc);
  }
  rc = fah_handle_resolve(*io_cond_h, FAH_TAG_COND, &raw);
  if (rc != FAH_OK) {
    return rc;
  }
  if (op == 1) {
    crc = abstract_http_cond_signal((struct AbstractHttpCond *)raw);
    return fah_error_from_c(crc);
  }
  if (op == 2) {
    crc = abstract_http_cond_broadcast((struct AbstractHttpCond *)raw);
    return fah_error_from_c(crc);
  }
  return FAH_ERR_INVAL;
}

/**
 * @brief Internal worker task trampoline freeing its heap `FahTaskClosure` on
 * completion.
 * @param[in] arg `FahTaskClosure *`.
 * @return `C_ABSTRACT_HTTP_SUCCESS` or `C_ABSTRACT_HTTP_ERR_CANCELED`.
 */
static c_abstract_http_error_t fah_pool_task_trampoline(void *arg) {
  struct FahTaskClosure *tc = (struct FahTaskClosure *)arg;
  int cb_rc = tc->cb(&tc->user_id);
  fah_free(tc);
  return (cb_rc == 0) ? C_ABSTRACT_HTTP_SUCCESS : C_ABSTRACT_HTTP_ERR_CANCELED;
}

fah_error_code_t fah_pool_create(size_t num_threads, fah_handle_t *out_pool_h) {
  struct AbstractHttpThreadPool *pool = NULL;
  fah_error_code_t rc;

  if (out_pool_h == NULL || num_threads == 0u) {
    return FAH_ERR_INVAL;
  }
  FAH_IGNORE_RC(abstract_http_thread_pool_init(&pool, num_threads));
  rc = fah_handle_register(FAH_TAG_THREAD_POOL, pool, out_pool_h);
  if (rc != FAH_OK) {
    FAH_IGNORE_RC(abstract_http_thread_pool_free(pool));
    return rc;
  }
  return FAH_OK;
}

fah_error_code_t fah_pool_submit(fah_handle_t pool_h, fah_f77_task_cb cb,
                                 int user_id) {
  void *raw = NULL;
  void *raw_tc = NULL;
  struct FahTaskClosure *tc;
  fah_error_code_t rc;

  if (cb == NULL) {
    return FAH_ERR_INVAL;
  }
  rc = fah_handle_resolve(pool_h, FAH_TAG_THREAD_POOL, &raw);
  if (rc != FAH_OK) {
    return rc;
  }
  rc = fah_calloc(1u, sizeof(struct FahTaskClosure), &raw_tc);
  if (rc != FAH_OK) {
    return rc;
  }
  tc = (struct FahTaskClosure *)raw_tc;
  tc->cb = cb;
  tc->user_id = user_id;
  FAH_IGNORE_RC(abstract_http_thread_pool_push(
      (struct AbstractHttpThreadPool *)raw, fah_pool_task_trampoline, tc));
  return FAH_OK;
}

fah_error_code_t fah_pool_destroy(fah_handle_t pool_h) {
  void *raw = NULL;
  c_abstract_http_error_t crc;
  fah_error_code_t rc =
      fah_handle_unregister(pool_h, FAH_TAG_THREAD_POOL, &raw);
  if (rc != FAH_OK) {
    return rc;
  }
  crc = abstract_http_thread_pool_free((struct AbstractHttpThreadPool *)raw);
  return fah_error_from_c(crc);
}

fah_error_code_t fah_tls_op(fah_handle_t *io_tls_h, int op, int *io_val) {
  void *raw = NULL;
  c_abstract_http_error_t crc;
  fah_error_code_t rc;

  if (io_tls_h == NULL) {
    return FAH_ERR_INVAL;
  }
  if (op == 0) {
    struct AbstractHttpTlsKey *key = NULL;
    FAH_IGNORE_RC(abstract_http_tls_key_create(&key, fah_free));
    rc = fah_handle_register(FAH_TAG_TLS_KEY, key, io_tls_h);
    if (rc != FAH_OK) {
      FAH_IGNORE_RC(abstract_http_tls_key_delete(key));
      return rc;
    }
    return FAH_OK;
  }
  if (op == 3) {
    void *cur = NULL;
    rc = fah_handle_unregister(*io_tls_h, FAH_TAG_TLS_KEY, &raw);
    if (rc != FAH_OK) {
      return rc;
    }
    *io_tls_h = 0;
    if (abstract_http_tls_get((struct AbstractHttpTlsKey *)raw, &cur) ==
            C_ABSTRACT_HTTP_SUCCESS &&
        cur != NULL) {
      fah_free(cur);
      FAH_IGNORE_RC(
          abstract_http_tls_set((struct AbstractHttpTlsKey *)raw, NULL));
    }
    crc = abstract_http_tls_key_delete((struct AbstractHttpTlsKey *)raw);
    return fah_error_from_c(crc);
  }
  if (io_val == NULL) {
    return FAH_ERR_INVAL;
  }
  rc = fah_handle_resolve(*io_tls_h, FAH_TAG_TLS_KEY, &raw);
  if (rc != FAH_OK) {
    return rc;
  }
  if (op == 1) {
    void *existing = NULL;
    void *boxed = NULL;
    if (abstract_http_tls_get((struct AbstractHttpTlsKey *)raw, &existing) ==
            C_ABSTRACT_HTTP_SUCCESS &&
        existing != NULL) {
      *(int *)existing = *io_val;
      return FAH_OK;
    }
    rc = fah_malloc(sizeof(int), &boxed);
    if (rc != FAH_OK) {
      return rc;
    }
    *(int *)boxed = *io_val;
    FAH_IGNORE_RC(
        abstract_http_tls_set((struct AbstractHttpTlsKey *)raw, boxed));
    return FAH_OK;
  }
  if (op == 2) {
    void *stored = NULL;
    FAH_IGNORE_RC(
        abstract_http_tls_get((struct AbstractHttpTlsKey *)raw, &stored));
    *io_val = (stored != NULL) ? *(const int *)stored : 0;
    return FAH_OK;
  }
  return FAH_ERR_INVAL;
}

/**
 * @brief Internal `abstract_http_coroutine_cb` trampoline.
 * @param[in] arg `FahCoroBox *`.
 * @return `C_ABSTRACT_HTTP_SUCCESS` or `C_ABSTRACT_HTTP_ERR_CANCELED`.
 */
static c_abstract_http_error_t fah_coro_trampoline(void *arg) {
  struct FahCoroBox *cbox = (struct FahCoroBox *)arg;
  int cb_rc = cbox->cb(&cbox->user_id);
  return (cb_rc == 0) ? C_ABSTRACT_HTTP_SUCCESS : C_ABSTRACT_HTTP_ERR_CANCELED;
}

fah_error_code_t fah_coro_create(size_t stack_size, fah_f77_task_cb cb,
                                 int user_id, fah_handle_t *out_coro_h) {
  void *raw = NULL;
  struct FahCoroBox *cbox;
  fah_error_code_t rc;

  if (cb == NULL || out_coro_h == NULL) {
    return FAH_ERR_INVAL;
  }
  rc = fah_calloc(1u, sizeof(struct FahCoroBox), &raw);
  if (rc != FAH_OK) {
    return rc;
  }
  cbox = (struct FahCoroBox *)raw;
  cbox->cb = cb;
  cbox->user_id = user_id;
  FAH_IGNORE_RC(abstract_http_coroutine_init(&cbox->co, stack_size,
                                             fah_coro_trampoline, cbox));
  rc = fah_handle_register(FAH_TAG_COROUTINE, cbox, out_coro_h);
  if (rc != FAH_OK) {
    FAH_IGNORE_RC(abstract_http_coroutine_free(cbox->co));
    fah_free(cbox);
    return rc;
  }
  return FAH_OK;
}

fah_error_code_t fah_coro_op(fah_handle_t coro_h, int op, int *out_done) {
  void *raw = NULL;
  struct FahCoroBox *cbox;
  c_abstract_http_error_t crc;
  fah_error_code_t rc;

  if (op == 1) {
    crc = abstract_http_coroutine_yield();
    return fah_error_from_c(crc);
  }
  if (op == 3) {
    rc = fah_handle_unregister(coro_h, FAH_TAG_COROUTINE, &raw);
    if (rc != FAH_OK) {
      return rc;
    }
    cbox = (struct FahCoroBox *)raw;
    crc = abstract_http_coroutine_free(cbox->co);
    fah_free(cbox);
    return fah_error_from_c(crc);
  }
  rc = fah_handle_resolve(coro_h, FAH_TAG_COROUTINE, &raw);
  if (rc != FAH_OK) {
    return rc;
  }
  cbox = (struct FahCoroBox *)raw;
  if (op == 0) {
    crc = abstract_http_coroutine_resume(cbox->co);
    return fah_error_from_c(crc);
  }
  if (op == 2) {
    if (out_done == NULL) {
      return FAH_ERR_INVAL;
    }
    crc = abstract_http_coroutine_is_done(cbox->co, out_done);
    return fah_error_from_c(crc);
  }
  return FAH_ERR_INVAL;
}

/**
 * @brief Internal `abstract_http_actor_handler_cb` trampoline unpacking
 * `FahActorMsgPayload` and freeing it after dispatch.
 * @param[in] actor Underlying C actor.
 * @param[in] msg Delivered actor message.
 * @return `C_ABSTRACT_HTTP_SUCCESS` or error code.
 */
static c_abstract_http_error_t
fah_actor_trampoline(struct AbstractHttpActor *actor,
                     struct AbstractHttpMessage *msg) {
  void *state = NULL;
  struct FahActorBox *abox;
  struct FahActorMsgPayload *pl;
  int mtype;
  int mval = 0;
  int sh = 0;
  int cb_rc;

  FAH_IGNORE_RC(abstract_http_actor_get_state(actor, &state));
  abox = (struct FahActorBox *)state;
  pl = (struct FahActorMsgPayload *)msg->payload;
  if (pl != NULL) {
    mval = pl->value;
    sh = pl->sender_h;
    fah_free(pl);
    msg->payload = NULL;
  }
  mtype = msg->type;
  cb_rc = abox->cb(&abox->self_h, &mtype, &mval, &sh, &abox->user_id);
  return (cb_rc == 0) ? C_ABSTRACT_HTTP_SUCCESS : C_ABSTRACT_HTTP_ERR_CANCELED;
}

fah_error_code_t fah_bus_create(fah_handle_t *out_bus_h) {
  void *raw = NULL;
  struct FahBusBox *bbox;
  fah_error_code_t rc;

  if (out_bus_h == NULL) {
    return FAH_ERR_INVAL;
  }
  rc = fah_calloc(1u, sizeof(struct FahBusBox), &raw);
  if (rc != FAH_OK) {
    return rc;
  }
  bbox = (struct FahBusBox *)raw;
  FAH_IGNORE_RC(abstract_http_message_bus_init(&bbox->bus));
  rc = fah_handle_register(FAH_TAG_MSG_BUS, bbox, out_bus_h);
  if (rc != FAH_OK) {
    FAH_IGNORE_RC(abstract_http_message_bus_free(bbox->bus));
    fah_free(bbox);
    return rc;
  }
  return FAH_OK;
}

fah_error_code_t fah_actor_spawn(fah_handle_t bus_h, const char *name,
                                 fah_f77_actor_cb cb, int user_id,
                                 fah_handle_t *out_actor_h) {
  void *raw_b = NULL;
  void *raw_a = NULL;
  struct FahBusBox *bbox;
  struct FahActorBox *abox;
  fah_error_code_t rc;

  if (name == NULL || cb == NULL || out_actor_h == NULL) {
    return FAH_ERR_INVAL;
  }
  rc = fah_handle_resolve(bus_h, FAH_TAG_MSG_BUS, &raw_b);
  if (rc != FAH_OK) {
    return rc;
  }
  bbox = (struct FahBusBox *)raw_b;
  rc = fah_calloc(1u, sizeof(struct FahActorBox), &raw_a);
  if (rc != FAH_OK) {
    return rc;
  }
  abox = (struct FahActorBox *)raw_a;
  abox->cb = cb;
  abox->user_id = user_id;

  FAH_IGNORE_RC(abstract_http_actor_spawn(bbox->bus, name, fah_actor_trampoline,
                                          abox, &abox->actor));
  abox->next_in_bus = bbox->actors;
  bbox->actors = abox;

  rc = fah_handle_register(FAH_TAG_ACTOR, abox, out_actor_h);
  if (rc != FAH_OK) {
    return rc;
  }
  abox->self_h = *out_actor_h;
  return FAH_OK;
}

fah_error_code_t fah_actor_send_int(fah_handle_t bus_h, fah_handle_t sender_h,
                                    fah_handle_t receiver_h, int msg_type,
                                    int msg_val) {
  void *raw_b = NULL;
  void *raw_r = NULL;
  void *raw_s = NULL;
  void *raw_pl = NULL;
  struct FahBusBox *bbox;
  struct FahActorBox *rbox;
  struct FahActorMsgPayload *pl;
  struct AbstractHttpMessage msg;
  fah_error_code_t rc = fah_handle_resolve(bus_h, FAH_TAG_MSG_BUS, &raw_b);
  if (rc != FAH_OK) {
    return rc;
  }
  rc = fah_handle_resolve(receiver_h, FAH_TAG_ACTOR, &raw_r);
  if (rc != FAH_OK) {
    return rc;
  }
  if (sender_h != 0) {
    rc = fah_handle_resolve(sender_h, FAH_TAG_ACTOR, &raw_s);
    if (rc != FAH_OK) {
      return rc;
    }
  }
  rc = fah_calloc(1u, sizeof(struct FahActorMsgPayload), &raw_pl);
  if (rc != FAH_OK) {
    return rc;
  }
  pl = (struct FahActorMsgPayload *)raw_pl;
  pl->sender_h = sender_h;
  pl->value = msg_val;

  bbox = (struct FahBusBox *)raw_b;
  rbox = (struct FahActorBox *)raw_r;
  memset(&msg, 0, sizeof(msg));
  msg.type = msg_type;
  msg.payload = pl;
  msg.sender = (raw_s != NULL) ? ((struct FahActorBox *)raw_s)->actor : NULL;
  msg.receiver = rbox->actor;

  FAH_IGNORE_RC(abstract_http_actor_send(bbox->bus, &msg));
  return FAH_OK;
}

fah_error_code_t fah_bus_process(fah_handle_t bus_h, int *out_processed) {
  void *raw = NULL;
  c_abstract_http_error_t crc;
  fah_error_code_t rc;
  if (out_processed == NULL) {
    return FAH_ERR_INVAL;
  }
  rc = fah_handle_resolve(bus_h, FAH_TAG_MSG_BUS, &raw);
  if (rc != FAH_OK) {
    return rc;
  }
  crc = abstract_http_message_bus_process(((struct FahBusBox *)raw)->bus,
                                          out_processed);
  return fah_error_from_c(crc);
}

fah_error_code_t fah_actor_get_name(fah_handle_t actor_h, char *out_buf,
                                    size_t cap, size_t *out_len,
                                    int space_pad) {
  void *raw = NULL;
  const char *nm = NULL;
  fah_error_code_t rc = fah_handle_resolve(actor_h, FAH_TAG_ACTOR, &raw);
  if (rc != FAH_OK) {
    return rc;
  }
  FAH_IGNORE_RC(abstract_http_actor_get_name(
      ((const struct FahActorBox *)raw)->actor, &nm));
  return fah_cbytes_to_fstr(nm, strlen(nm), out_buf, cap, out_len, space_pad);
}

fah_error_code_t fah_actor_destroy(fah_handle_t actor_h) {
  void *raw = NULL;
  fah_error_code_t rc = fah_handle_unregister(actor_h, FAH_TAG_ACTOR, &raw);
  if (rc != FAH_OK) {
    return rc;
  }
  ((struct FahActorBox *)raw)->self_h = 0;
  return FAH_OK;
}

fah_error_code_t fah_bus_destroy(fah_handle_t bus_h) {
  void *raw = NULL;
  struct FahBusBox *bbox;
  struct FahActorBox *cur;
  c_abstract_http_error_t crc;
  fah_error_code_t rc = fah_handle_unregister(bus_h, FAH_TAG_MSG_BUS, &raw);
  if (rc != FAH_OK) {
    return rc;
  }
  bbox = (struct FahBusBox *)raw;
  crc = abstract_http_message_bus_free(bbox->bus);
  cur = bbox->actors;
  while (cur != NULL) {
    struct FahActorBox *nxt = cur->next_in_bus;
    if (cur->self_h != 0) {
      void *u = NULL;
      FAH_IGNORE_RC(fah_handle_unregister(cur->self_h, FAH_TAG_ACTOR, &u));
    }
    fah_free(cur);
    cur = nxt;
  }
  fah_free(bbox);
  return fah_error_from_c(crc);
}

fah_error_code_t fah_ipc_op(int op, fah_handle_t in_h, const void *in_buf,
                            size_t in_len, void *out_buf, size_t out_cap,
                            fah_handle_t *out_h) {
  c_abstract_http_error_t crc;
  fah_error_code_t rc;

  if (op == 0) {
    struct AbstractHttpIpcPipe pipe_pair;
    if (in_len == 0u || in_buf == NULL || out_buf == NULL || out_cap < in_len) {
      return FAH_ERR_INVAL;
    }
    memset(&pipe_pair, 0, sizeof(pipe_pair));
    FAH_IGNORE_RC(abstract_http_ipc_pipe_init(&pipe_pair));
    crc = abstract_http_ipc_write(pipe_pair.write_handle, in_buf, in_len);
    if (crc == C_ABSTRACT_HTTP_SUCCESS) {
      crc = abstract_http_ipc_read(pipe_pair.read_handle, out_buf, in_len);
    }
    FAH_IGNORE_RC(abstract_http_ipc_pipe_free(&pipe_pair));
    return fah_error_from_c(crc);
  }
  if (op == 1) {
    void *raw_r = NULL;
    char *ser = NULL;
    size_t ser_len = 0u;
    fah_handle_t new_h = 0;
    void *raw_nr = NULL;
    if (out_h == NULL) {
      return FAH_ERR_INVAL;
    }
    rc = fah_handle_resolve(in_h, FAH_TAG_REQUEST, &raw_r);
    if (rc != FAH_OK) {
      return rc;
    }
    FAH_IGNORE_RC(abstract_http_ipc_serialize_request(
        &((const struct FahRequestBox *)raw_r)->req, &ser, &ser_len));
    rc = fah_request_create(&new_h);
    if (rc != FAH_OK) {
      fah_free(ser);
      return rc;
    }
    FAH_IGNORE_RC(fah_handle_resolve(new_h, FAH_TAG_REQUEST, &raw_nr));
    FAH_IGNORE_RC(http_request_free(&((struct FahRequestBox *)raw_nr)->req));
    FAH_IGNORE_RC(abstract_http_ipc_deserialize_request(
        ser, ser_len, &((struct FahRequestBox *)raw_nr)->req));
    fah_free(ser);
    *out_h = new_h;
    return FAH_OK;
  }
  if (op == 2) {
    void *raw_rs = NULL;
    char *ser = NULL;
    size_t ser_len = 0u;
    void *raw_box = NULL;
    struct FahResponseBox *nbox;
    if (out_h == NULL) {
      return FAH_ERR_INVAL;
    }
    rc = fah_handle_resolve(in_h, FAH_TAG_RESPONSE, &raw_rs);
    if (rc != FAH_OK) {
      return rc;
    }
    FAH_IGNORE_RC(abstract_http_ipc_serialize_response(
        ((const struct FahResponseBox *)raw_rs)->res, &ser, &ser_len));
    rc = fah_calloc(1u, sizeof(struct FahResponseBox), &raw_box);
    if (rc != FAH_OK) {
      fah_free(ser);
      return rc;
    }
    nbox = (struct FahResponseBox *)raw_box;
    nbox->res = &nbox->inline_res;
    nbox->owns_outer_struct = 0;
    FAH_IGNORE_RC(
        abstract_http_ipc_deserialize_response(ser, ser_len, nbox->res));
    fah_free(ser);
    rc = fah_handle_register(FAH_TAG_RESPONSE, nbox, out_h);
    if (rc != FAH_OK) {
      FAH_IGNORE_RC(http_response_free(nbox->res));
      fah_free(nbox);
      return rc;
    }
    return FAH_OK;
  }
  return FAH_ERR_INVAL;
}

fah_error_code_t fah_cmp_op(int op, int cmp_mod, size_t min_t, size_t max_t,
                            fah_handle_t target_h, int *out_mod,
                            fah_handle_t *out_cmp_h) {
  c_abstract_http_error_t crc;
  fah_error_code_t rc;

  if (op == 0) {
    enum ExecutionModality em = MODALITY_SYNC;
    if (out_mod == NULL) {
      return FAH_ERR_INVAL;
    }
    crc = cmp_http_modality_adapter(cmp_mod, &em);
    *out_mod = (int)em;
    return fah_error_from_c(crc);
  }
  if (op == 1) {
    void *raw_cfg = NULL;
    struct CmpAppConfig acfg;
    rc = fah_handle_resolve(target_h, FAH_TAG_CONFIG, &raw_cfg);
    if (rc != FAH_OK) {
      return rc;
    }
    acfg.modality = cmp_mod;
    acfg.min_threads = min_t;
    acfg.max_threads = max_t;
    crc = cmp_http_inject_config(&acfg, (struct HttpConfig *)raw_cfg);
    return fah_error_from_c(crc);
  }
  if (op == 2) {
    void *raw_req = NULL;
    void *raw_b = NULL;
    struct CmpProgressBinding *bind_p;
    if (out_cmp_h == NULL) {
      return FAH_ERR_INVAL;
    }
    rc = fah_handle_resolve(target_h, FAH_TAG_REQUEST, &raw_req);
    if (rc != FAH_OK) {
      return rc;
    }
    rc = fah_calloc(1u, sizeof(struct CmpProgressBinding), &raw_b);
    if (rc != FAH_OK) {
      return rc;
    }
    bind_p = (struct CmpProgressBinding *)raw_b;
    FAH_IGNORE_RC(
        cmp_http_bind_request(&((struct FahRequestBox *)raw_req)->req, bind_p));
    rc = fah_handle_register(FAH_TAG_CMP_BINDING, bind_p, out_cmp_h);
    if (rc != FAH_OK) {
      FAH_IGNORE_RC(
          cmp_http_bind_request(&((struct FahRequestBox *)raw_req)->req, NULL));
      fah_free(bind_p);
      return rc;
    }
    return FAH_OK;
  }
  if (op == 3) {
    void *raw_b = NULL;
    rc = fah_handle_resolve(target_h, FAH_TAG_CMP_BINDING, &raw_b);
    if (rc != FAH_OK) {
      return rc;
    }
    ((struct CmpProgressBinding *)raw_b)->cancel_requested = 1;
    return FAH_OK;
  }
  if (op == 4) {
    void *raw_b = NULL;
    rc = fah_handle_unregister(target_h, FAH_TAG_CMP_BINDING, &raw_b);
    if (rc != FAH_OK) {
      return rc;
    }
    fah_free(raw_b);
    return FAH_OK;
  }
  return FAH_ERR_INVAL;
}
