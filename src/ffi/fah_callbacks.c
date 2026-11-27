/**
 * @file fah_callbacks.c
 * @brief Implementation of C-to-Fortran callback trampolines and automatic
 * `FahPollQueue` event forwarding.
 */

/* clang-format off */
#include <limits.h>
#include <stddef.h>
#include "fah_c_bridge.h"
#include "fah_callbacks.h"
/* clang-format on */

/**
 * @brief Clamp a `size_t` value into signed 32-bit `int` range for classical
 * FORTRAN IV-95 `INTEGER` callback arguments.
 * @param[in] v Unsigned size value.
 * @return Saturated non-negative `int`.
 */
static int fah_clamp_size_to_int(size_t v) {
  return (v > (size_t)INT_MAX) ? INT_MAX : (int)v;
}

c_abstract_http_error_t fah_trampoline_on_chunk(void *user_data,
                                                const void *chunk, size_t len) {
  const FahReqCallbackCtx *ctx = (const FahReqCallbackCtx *)user_data;
  const char *c_ptr = (const char *)chunk;
  int ilen;

  if (ctx == NULL) {
    return C_ABSTRACT_HTTP_ERR_INVAL;
  }

  if (ctx->poll_queue_h > 0) {
    fah_error_code_t qrc = fah_poll_queue_push(
        ctx->poll_queue_h, FAH_POLL_CHUNK, 0, NULL, chunk, len);
    if (qrc != FAH_OK) {
      return C_ABSTRACT_HTTP_ERR_NOMEM;
    }
  }

  if (ctx->on_chunk != NULL) {
    int cb_rc;
    ilen = fah_clamp_size_to_int(len);
    cb_rc =
        ctx->on_chunk(&ctx->on_chunk_uid, (c_ptr != NULL) ? c_ptr : "", &ilen);
    if (cb_rc != 0) {
      return C_ABSTRACT_HTTP_ERR_CANCELED;
    }
  }
  return C_ABSTRACT_HTTP_SUCCESS;
}

c_abstract_http_error_t fah_trampoline_read_chunk(void *user_data, void *buf,
                                                  size_t buf_len,
                                                  size_t *out_len) {
  const FahReqCallbackCtx *ctx = (const FahReqCallbackCtx *)user_data;
  int icap;
  int iwritten = 0;
  int cb_rc;

  if (ctx == NULL || buf == NULL || out_len == NULL ||
      ctx->read_chunk == NULL) {
    return C_ABSTRACT_HTTP_ERR_INVAL;
  }

  icap = fah_clamp_size_to_int(buf_len);
  cb_rc = ctx->read_chunk(&ctx->read_chunk_uid, (char *)buf, &icap, &iwritten);
  if (cb_rc != 0 || iwritten < 0 || (size_t)iwritten > buf_len) {
    *out_len = 0u;
    return C_ABSTRACT_HTTP_ERR_CANCELED;
  }
  *out_len = (size_t)iwritten;
  return C_ABSTRACT_HTTP_SUCCESS;
}

c_abstract_http_error_t fah_trampoline_on_progress(size_t current_bytes,
                                                   size_t total_bytes,
                                                   void *user_data) {
  const FahReqCallbackCtx *ctx = (const FahReqCallbackCtx *)user_data;
  int icur;
  int itot;
  int cb_rc;

  if (ctx == NULL) {
    return C_ABSTRACT_HTTP_ERR_INVAL;
  }
  if (ctx->on_progress == NULL) {
    return C_ABSTRACT_HTTP_SUCCESS;
  }

  icur = fah_clamp_size_to_int(current_bytes);
  itot = fah_clamp_size_to_int(total_bytes);
  cb_rc = ctx->on_progress(&icur, &itot, &ctx->on_progress_uid);
  return (cb_rc == 0) ? C_ABSTRACT_HTTP_SUCCESS : C_ABSTRACT_HTTP_ERR_CANCELED;
}

c_abstract_http_error_t
fah_trampoline_ws_on_msg(const struct c_abstract_http_ws_event *ev,
                         void *user_data) {
  const FahWsCallbackCtx *ctx = (const FahWsCallbackCtx *)user_data;
  int iop;
  int ilen;
  int ifin;

  if (ctx == NULL || ev == NULL) {
    return C_ABSTRACT_HTTP_ERR_INVAL;
  }

  iop = (int)ev->opcode;
  if (ctx->poll_queue_h > 0) {
    fah_error_code_t qrc =
        fah_poll_queue_push(ctx->poll_queue_h, FAH_POLL_WS_MSG, iop, NULL,
                            ev->payload, ev->payload_len);
    if (qrc != FAH_OK) {
      return C_ABSTRACT_HTTP_ERR_NOMEM;
    }
  }

  if (ctx->on_msg != NULL) {
    int cb_rc;
    const char *p = (ev->payload != NULL) ? (const char *)ev->payload : "";
    ilen = fah_clamp_size_to_int(ev->payload_len);
    ifin = (ev->is_fin != 0) ? 1 : 0;
    cb_rc = ctx->on_msg(&iop, p, &ilen, &ifin, &ctx->user_id);
    if (cb_rc != 0) {
      return C_ABSTRACT_HTTP_ERR_CANCELED;
    }
  }
  return C_ABSTRACT_HTTP_SUCCESS;
}

c_abstract_http_error_t fah_trampoline_ws_on_err(int error_code,
                                                 void *user_data) {
  const FahWsCallbackCtx *ctx = (const FahWsCallbackCtx *)user_data;
  int ierr = error_code;

  if (ctx == NULL) {
    return C_ABSTRACT_HTTP_ERR_INVAL;
  }
  if (ctx->poll_queue_h > 0) {
    fah_error_code_t qrc = fah_poll_queue_push(
        ctx->poll_queue_h, FAH_POLL_WS_ERR, ierr, NULL, NULL, 0u);
    if (qrc != FAH_OK) {
      return C_ABSTRACT_HTTP_ERR_NOMEM;
    }
  }
  if (ctx->on_err != NULL) {
    int cb_rc = ctx->on_err(&ierr, &ctx->user_id);
    if (cb_rc != 0) {
      return C_ABSTRACT_HTTP_ERR_CANCELED;
    }
  }
  return C_ABSTRACT_HTTP_SUCCESS;
}

c_abstract_http_error_t fah_trampoline_ws_on_close(int status_code,
                                                   void *user_data) {
  const FahWsCallbackCtx *ctx = (const FahWsCallbackCtx *)user_data;
  int ists = status_code;

  if (ctx == NULL) {
    return C_ABSTRACT_HTTP_ERR_INVAL;
  }
  if (ctx->poll_queue_h > 0) {
    fah_error_code_t qrc = fah_poll_queue_push(
        ctx->poll_queue_h, FAH_POLL_WS_CLOSE, ists, NULL, NULL, 0u);
    if (qrc != FAH_OK) {
      return C_ABSTRACT_HTTP_ERR_NOMEM;
    }
  }
  if (ctx->on_close != NULL) {
    int cb_rc = ctx->on_close(&ists, &ctx->user_id);
    if (cb_rc != 0) {
      return C_ABSTRACT_HTTP_ERR_CANCELED;
    }
  }
  return C_ABSTRACT_HTTP_SUCCESS;
}

c_abstract_http_error_t
fah_trampoline_sse_on_evt(const struct c_abstract_http_sse_event *ev,
                          void *user_data) {
  const FahSseCallbackCtx *ctx = (const FahSseCallbackCtx *)user_data;

  if (ctx == NULL || ev == NULL) {
    return C_ABSTRACT_HTTP_ERR_INVAL;
  }

  if (ctx->poll_queue_h > 0) {
    fah_error_code_t qrc = fah_poll_queue_push(
        ctx->poll_queue_h, FAH_POLL_SSE_EVT, 0,
        ev->event != NULL ? ev->event : "message", ev->data, ev->data_len);
    if (qrc != FAH_OK) {
      return C_ABSTRACT_HTTP_ERR_NOMEM;
    }
  }

  if (ctx->on_evt != NULL) {
    const char *id_s = (ev->id != NULL) ? ev->id : "";
    const char *ev_s = (ev->event != NULL) ? ev->event : "message";
    const char *dt_s = (ev->data != NULL) ? ev->data : "";
    int id_l = 0;
    int ev_l = 0;
    int dt_l = fah_clamp_size_to_int(ev->data_len);
    int cb_rc;

    while (id_s[id_l] != '\0' && id_l < INT_MAX) {
      id_l++;
    }
    while (ev_s[ev_l] != '\0' && ev_l < INT_MAX) {
      ev_l++;
    }
    cb_rc = ctx->on_evt(id_s, &id_l, ev_s, &ev_l, dt_s, &dt_l, &ctx->user_id);
    if (cb_rc != 0) {
      return C_ABSTRACT_HTTP_ERR_CANCELED;
    }
  }
  return C_ABSTRACT_HTTP_SUCCESS;
}

c_abstract_http_error_t fah_trampoline_sse_on_err(int error_code,
                                                  void *user_data) {
  const FahSseCallbackCtx *ctx = (const FahSseCallbackCtx *)user_data;
  int ierr = error_code;

  if (ctx == NULL) {
    return C_ABSTRACT_HTTP_ERR_INVAL;
  }
  if (ctx->poll_queue_h > 0) {
    fah_error_code_t qrc = fah_poll_queue_push(
        ctx->poll_queue_h, FAH_POLL_SSE_ERR, ierr, NULL, NULL, 0u);
    if (qrc != FAH_OK) {
      return C_ABSTRACT_HTTP_ERR_NOMEM;
    }
  }
  if (ctx->on_err != NULL) {
    int cb_rc = ctx->on_err(&ierr, &ctx->user_id);
    if (cb_rc != 0) {
      return C_ABSTRACT_HTTP_ERR_CANCELED;
    }
  }
  return C_ABSTRACT_HTTP_SUCCESS;
}

c_abstract_http_error_t fah_trampoline_sse_on_close(void *user_data) {
  const FahSseCallbackCtx *ctx = (const FahSseCallbackCtx *)user_data;

  if (ctx == NULL) {
    return C_ABSTRACT_HTTP_ERR_INVAL;
  }
  if (ctx->poll_queue_h > 0) {
    fah_error_code_t qrc = fah_poll_queue_push(
        ctx->poll_queue_h, FAH_POLL_SSE_CLOSE, 0, NULL, NULL, 0u);
    if (qrc != FAH_OK) {
      return C_ABSTRACT_HTTP_ERR_NOMEM;
    }
  }
  if (ctx->on_close != NULL) {
    int cb_rc = ctx->on_close(&ctx->user_id);
    if (cb_rc != 0) {
      return C_ABSTRACT_HTTP_ERR_CANCELED;
    }
  }
  return C_ABSTRACT_HTTP_SUCCESS;
}
