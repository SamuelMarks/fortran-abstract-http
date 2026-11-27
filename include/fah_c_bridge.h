/**
 * @file fah_c_bridge.h
 * @brief Complete strict ISO C89 FFI bridge (`BIND(C)` & handle-based) wrapping
 * `c-abstract-http` for FORTRAN (I) through Fortran 2023.
 */

#ifndef FAH_C_BRIDGE_H
#define FAH_C_BRIDGE_H

/* clang-format off */
#include <stddef.h>
#include "fah_callbacks.h"
#include "fah_cfi_c89.h"
#include "fah_error.h"
#include "fah_export.h"
#include "fah_handle_registry.h"
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @brief OAuth 2.0 grant selector used by `fah_oauth2_grant`.
 */
typedef enum fah_oauth2_grant_kind {
  FAH_OAUTH2_PASSWORD = 1,           /**< Resource Owner Password Credentials */
  FAH_OAUTH2_REFRESH_TOKEN = 2,      /**< Refresh Token Grant */
  FAH_OAUTH2_AUTH_CODE = 3,          /**< Authorization Code + optional PKCE */
  FAH_OAUTH2_CLIENT_CREDENTIALS = 4, /**< Client Credentials Grant */
  FAH_OAUTH2_JWT_BEARER = 5,         /**< RFC 7523 JWT Bearer Grant */
  FAH_OAUTH2_DEVICE_AUTH = 6,        /**< RFC 8628 Device Authorization */
  FAH_OAUTH2_DEVICE_TOKEN = 7,       /**< RFC 8628 Device Access Token Poll */
  FAH_OAUTH2_REVOKE = 8,             /**< RFC 7009 Token Revocation */
  FAH_OAUTH2_INTROSPECT = 9          /**< RFC 7662 Token Introspection */
} fah_oauth2_grant_kind_t;

/**
 * @brief Poll event kinds stored in `FahPollQueue` for FORTRAN I-III pull
 * loops.
 */
typedef enum fah_poll_event_kind {
  FAH_POLL_NONE = 0,     /**< Queue was empty */
  FAH_POLL_CHUNK = 1,    /**< Streaming HTTP body chunk */
  FAH_POLL_WS_MSG = 2,   /**< WebSocket data/control frame (`code` = opcode) */
  FAH_POLL_WS_ERR = 3,   /**< WebSocket error (`code` = error_code) */
  FAH_POLL_WS_CLOSE = 4, /**< WebSocket close (`code` = status_code) */
  FAH_POLL_SSE_EVT = 5,  /**< SSE event (`evt_name` + `payload`) */
  FAH_POLL_SSE_ERR = 6,  /**< SSE error (`code` = error_code) */
  FAH_POLL_SSE_CLOSE = 7 /**< SSE stream closed */
} fah_poll_event_kind_t;

/* --- Global Lifecycle --- */

/**
 * @brief Initialize global transport state (`transport_global_init` +
 * `http_raw_global_init`).
 * @return `FAH_OK` on success, or unified error code.
 */
FAH_NO_DISCARD FAH_API fah_error_code_t fah_global_init(void);

/**
 * @brief Clean up global transport state (`http_raw_global_cleanup` +
 * `transport_global_cleanup`).
 * @return `FAH_OK` on success, or unified error code.
 */
FAH_NO_DISCARD FAH_API fah_error_code_t fah_global_cleanup(void);

/* --- FORTRAN I-III Pull-Polling Queue (`FahPollQueue`) --- */

/**
 * @brief Allocate a FIFO event queue handle for callback-less FORTRAN I-III
 * programs.
 * @param[out] out_q_h Receives positive `FAH_TAG_POLL_QUEUE` handle.
 * @return `FAH_OK` on success, or `FAH_ERR_INVAL` / `FAH_ERR_NOMEM`.
 */
FAH_NO_DISCARD FAH_API fah_error_code_t
fah_poll_queue_create(fah_handle_t *out_q_h);

/**
 * @brief Destroy a poll queue handle and release any queued payloads.
 * @param[in] q_h Poll queue handle.
 * @return `FAH_OK` on success, or handle validation error.
 */
FAH_NO_DISCARD FAH_API fah_error_code_t
fah_poll_queue_destroy(fah_handle_t q_h);

/**
 * @brief Enqueue an event into `q_h` (used internally by callback trampolines).
 * @param[in] q_h Poll queue handle.
 * @param[in] kind Event kind (`fah_poll_event_kind_t`).
 * @param[in] code Opcode, status code, or error code.
 * @param[in] evt_name Optional event name string (`NULL` allowed).
 * @param[in] payload Optional byte payload (`NULL` allowed when `len == 0`).
 * @param[in] len Byte length of `payload`.
 * @return `FAH_OK` on success, or handle/allocation error.
 */
FAH_NO_DISCARD FAH_API fah_error_code_t
fah_poll_queue_push(fah_handle_t q_h, fah_poll_event_kind_t kind, int code,
                    const char *evt_name, const void *payload, size_t len);

/**
 * @brief Pop the oldest queued event from `q_h` without blocking.
 * @param[in] q_h Poll queue handle.
 * @param[out] out_kind Receives `FAH_POLL_NONE` if empty, else event kind.
 * @param[out] out_code Receives event opcode/status/error code.
 * @param[out] evt_buf Optional destination buffer for SSE event type.
 * @param[in] evt_cap Byte capacity of `evt_buf`.
 * @param[out] out_evt_len Optional pointer receiving full event name length.
 * @param[out] data_buf Optional destination buffer for payload bytes.
 * @param[in] data_cap Byte capacity of `data_buf`.
 * @param[out] out_data_len Optional pointer receiving full payload length.
 * @return `FAH_OK` (including when empty), `FAH_ERR_BUF_TOO_SMALL` if
 * truncated, or handle error.
 */
FAH_NO_DISCARD FAH_API fah_error_code_t
fah_poll_queue_pop(fah_handle_t q_h, int *out_kind, int *out_code,
                   char *evt_buf, size_t evt_cap, size_t *out_evt_len,
                   void *data_buf, size_t data_cap, size_t *out_data_len);

/* --- HttpConfig & HttpClient --- */

/**
 * @brief Create a default-initialized `HttpConfig` handle.
 * @param[out] out_cfg_h Receives positive `FAH_TAG_CONFIG` handle.
 * @return `FAH_OK` on success, or error code.
 */
FAH_NO_DISCARD FAH_API fah_error_code_t
fah_config_create(fah_handle_t *out_cfg_h);

/**
 * @brief Destroy an `HttpConfig` handle and free owned strings.
 * @param[in] cfg_h Config handle.
 * @return `FAH_OK` on success, or handle validation error.
 */
FAH_NO_DISCARD FAH_API fah_error_code_t fah_config_destroy(fah_handle_t cfg_h);

/**
 * @brief Configure general, connect, read, and write timeouts in milliseconds.
 * @param[in] cfg_h Config handle.
 * @param[in] timeout_ms General timeout in ms.
 * @param[in] connect_ms Connect timeout in ms (`0` = use `timeout_ms`).
 * @param[in] read_ms Read timeout in ms (`0` = use `timeout_ms`).
 * @param[in] write_ms Write timeout in ms (`0` = use `timeout_ms`).
 * @return `FAH_OK` on success, or handle error.
 */
FAH_NO_DISCARD FAH_API fah_error_code_t
fah_config_set_timeouts(fah_handle_t cfg_h, long timeout_ms, long connect_ms,
                        long read_ms, long write_ms);

/**
 * @brief Configure peer/host TLS verification, redirect following, and HTTP/3
 * fallback flags.
 * @param[in] cfg_h Config handle.
 * @param[in] verify_peer `1` to verify TLS peer certificate, `0` to skip.
 * @param[in] verify_host `1` to verify TLS hostname, `0` to skip.
 * @param[in] follow_redirects `1` to follow 3xx redirects, `0` to disable.
 * @param[in] http3_fallback `1` to fall back to HTTP/1.1-2 on QUIC failure.
 * @return `FAH_OK` on success, or handle error.
 */
FAH_NO_DISCARD FAH_API fah_error_code_t
fah_config_set_tls_flags(fah_handle_t cfg_h, int verify_peer, int verify_host,
                         int follow_redirects, int http3_fallback);

/**
 * @brief Configure HTTP and TLS version bitmasks and retry policy.
 * @param[in] cfg_h Config handle.
 * @param[in] version_mask Bitmask of `enum HttpVersion`.
 * @param[in] tls_version_mask Bitmask of `enum HttpTlsVersion`.
 * @param[in] retry_count Maximum automatic retries (`>= 0`).
 * @param[in] retry_policy `enum HttpRetryPolicy` (`0..2`).
 * @return `FAH_OK` on success, or handle/argument error.
 */
FAH_NO_DISCARD FAH_API fah_error_code_t fah_config_set_versions_and_retry(
    fah_handle_t cfg_h, unsigned int version_mask,
    unsigned int tls_version_mask, int retry_count, int retry_policy);

/**
 * @brief Configure execution modality, thread pool bounds, and keep-alive
 * pooling.
 * @param[in] cfg_h Config handle.
 * @param[in] modality `enum ExecutionModality` (`0..5`).
 * @param[in] min_threads Minimum worker threads.
 * @param[in] max_threads Maximum worker threads.
 * @param[in] keep_alive_enabled `1` to pool HTTP/1.1 connections.
 * @param[in] keep_alive_idle_ms Max idle lifetime in ms.
 * @param[in] max_idle_conns Max pooled idle sockets.
 * @return `FAH_OK` on success, or handle/argument error.
 */
FAH_NO_DISCARD FAH_API fah_error_code_t fah_config_set_modality_and_pool(
    fah_handle_t cfg_h, int modality, size_t min_threads, size_t max_threads,
    int keep_alive_enabled, long keep_alive_idle_ms, size_t max_idle_conns);

/**
 * @brief Configure User-Agent, proxy URL/credentials, and TLS CA/mTLS/SPKI
 * paths on `cfg_h` (`NULL` leaves the corresponding field unchanged or clears
 * if empty).
 * @param[in] cfg_h Config handle.
 * @param[in] user_agent Optional custom `User-Agent` (`NULL` to skip).
 * @param[in] proxy_url Optional HTTP/SOCKS5 proxy URL (`NULL` to skip).
 * @param[in] proxy_user Optional proxy username (`NULL` to skip).
 * @param[in] proxy_pass Optional proxy password (`NULL` to skip).
 * @param[in] ca_path Optional PEM/DER CA bundle path (`NULL` to skip).
 * @param[in] client_cert Optional mTLS client cert path (`NULL` to skip).
 * @param[in] client_key Optional mTLS client key path (`NULL` to skip).
 * @param[in] cert_pin_sha256 Optional SPKI SHA-256 pin (`NULL` to skip).
 * @return `FAH_OK` on success, or `FAH_ERR_NOMEM` / handle error.
 */
FAH_NO_DISCARD FAH_API fah_error_code_t fah_config_set_strings(
    fah_handle_t cfg_h, const char *user_agent, const char *proxy_url,
    const char *proxy_user, const char *proxy_pass, const char *ca_path,
    const char *client_cert, const char *client_key,
    const char *cert_pin_sha256);

/**
 * @brief Attach a shared `FAH_TAG_COOKIE_JAR` handle (`0` to detach) onto
 * `cfg_h`.
 * @param[in] cfg_h Config handle.
 * @param[in] jar_h Cookie jar handle (`0` to clear).
 * @return `FAH_OK` on success, or handle error.
 */
FAH_NO_DISCARD FAH_API fah_error_code_t
fah_config_set_cookie_jar(fah_handle_t cfg_h, fah_handle_t jar_h);

/**
 * @brief Create an `HttpClient` handle using either the platform default
 * transport (`use_raw_transport == 0`) or the deterministic raw-socket HTTP/1.1
 * transport (`use_raw_transport != 0`).
 * @param[in] cfg_h Optional `FAH_TAG_CONFIG` handle (`0` for defaults).
 * @param[in] use_raw_transport Non-zero to bind `http_raw_context_init`.
 * @param[out] out_cli_h Receives positive `FAH_TAG_CLIENT` handle.
 * @return `FAH_OK` on success, or unified error code.
 */
FAH_NO_DISCARD FAH_API fah_error_code_t fah_client_create(
    fah_handle_t cfg_h, int use_raw_transport, fah_handle_t *out_cli_h);

/**
 * @brief Set `base_url` on an `HttpClient` for relative request URL resolution.
 * @param[in] cli_h Client handle.
 * @param[in] base_url NUL-terminated base URL string.
 * @return `FAH_OK` on success, or error code.
 */
FAH_NO_DISCARD FAH_API fah_error_code_t
fah_client_set_base_url(fah_handle_t cli_h, const char *base_url);

/**
 * @brief Clean up transport state and destroy an `HttpClient` handle.
 * @param[in] cli_h Client handle.
 * @return `FAH_OK` on success, or handle error.
 */
FAH_NO_DISCARD FAH_API fah_error_code_t fah_client_destroy(fah_handle_t cli_h);

/* --- Standalone Headers & Cookie Jar --- */

/**
 * @brief Create a standalone `HttpHeaders` container handle.
 * @param[out] out_hdr_h Receives positive `FAH_TAG_HEADERS` handle.
 * @return `FAH_OK` on success, or error code.
 */
FAH_NO_DISCARD FAH_API fah_error_code_t
fah_headers_create(fah_handle_t *out_hdr_h);

/**
 * @brief Destroy a standalone `HttpHeaders` handle.
 * @param[in] hdr_h Headers handle.
 * @return `FAH_OK` on success, or handle error.
 */
FAH_NO_DISCARD FAH_API fah_error_code_t fah_headers_destroy(fah_handle_t hdr_h);

/**
 * @brief Append a `(key, value)` header pair to `hdr_h`.
 * @param[in] hdr_h Headers handle.
 * @param[in] key NUL-terminated header name.
 * @param[in] val NUL-terminated header value.
 * @return `FAH_OK` on success, or error code.
 */
FAH_NO_DISCARD FAH_API fah_error_code_t fah_headers_add(fah_handle_t hdr_h,
                                                        const char *key,
                                                        const char *val);

/**
 * @brief Case-insensitively look up `key` in `hdr_h` and copy its value into
 * `out_buf`.
 * @param[in] hdr_h Headers handle.
 * @param[in] key NUL-terminated header key.
 * @param[out] out_buf Destination buffer (`NULL` allowed when `cap == 0`).
 * @param[in] cap Capacity of `out_buf`.
 * @param[out] out_len Optional pointer receiving full value length.
 * @param[in] space_pad Non-zero to pad trailing capacity with spaces.
 * @return `FAH_OK` on match, `FAH_ERR_INVAL` if missing, or
 * `FAH_ERR_BUF_TOO_SMALL`.
 */
FAH_NO_DISCARD FAH_API fah_error_code_t
fah_headers_get(fah_handle_t hdr_h, const char *key, char *out_buf, size_t cap,
                size_t *out_len, int space_pad);

/**
 * @brief Remove headers matching `key` from `hdr_h`.
 * @param[in] hdr_h Headers handle.
 * @param[in] key Header key to remove.
 * @return `FAH_OK` on success, or error code.
 */
FAH_NO_DISCARD FAH_API fah_error_code_t fah_headers_remove(fah_handle_t hdr_h,
                                                           const char *key);

/**
 * @brief Query the number of headers stored in `hdr_h`.
 * @param[in] hdr_h Headers handle.
 * @param[out] out_count Receives header count.
 * @return `FAH_OK` on success, or handle/argument error.
 */
FAH_NO_DISCARD FAH_API fah_error_code_t fah_headers_count(fah_handle_t hdr_h,
                                                          size_t *out_count);

/**
 * @brief Create an RFC 6265 `HttpCookieJar` handle.
 * @param[out] out_jar_h Receives positive `FAH_TAG_COOKIE_JAR` handle.
 * @return `FAH_OK` on success, or error code.
 */
FAH_NO_DISCARD FAH_API fah_error_code_t
fah_cookie_jar_create(fah_handle_t *out_jar_h);

/**
 * @brief Destroy an `HttpCookieJar` handle.
 * @param[in] jar_h Cookie jar handle.
 * @return `FAH_OK` on success, or handle error.
 */
FAH_NO_DISCARD FAH_API fah_error_code_t
fah_cookie_jar_destroy(fah_handle_t jar_h);

/**
 * @brief Insert or update a cookie `(name, val)` in `jar_h`.
 * @param[in] jar_h Cookie jar handle.
 * @param[in] name Cookie name.
 * @param[in] val Cookie value.
 * @return `FAH_OK` on success, or error code.
 */
FAH_NO_DISCARD FAH_API fah_error_code_t fah_cookie_jar_set(fah_handle_t jar_h,
                                                           const char *name,
                                                           const char *val);

/**
 * @brief Retrieve a cookie value by `name` from `jar_h`.
 * @param[in] jar_h Cookie jar handle.
 * @param[in] name Cookie name.
 * @param[out] out_buf Destination buffer.
 * @param[in] cap Byte capacity of `out_buf`.
 * @param[out] out_len Optional pointer receiving full value length.
 * @param[in] space_pad Non-zero to pad trailing capacity with spaces.
 * @return `FAH_OK` on match, `FAH_ERR_INVAL` if missing, or
 * `FAH_ERR_BUF_TOO_SMALL`.
 */
FAH_NO_DISCARD FAH_API fah_error_code_t
fah_cookie_jar_get(fah_handle_t jar_h, const char *name, char *out_buf,
                   size_t cap, size_t *out_len, int space_pad);

/**
 * @brief Parse an incoming `Set-Cookie` header string into `jar_h`.
 * @param[in] jar_h Cookie jar handle.
 * @param[in] set_cookie Raw `Set-Cookie` header value.
 * @return `FAH_OK` on success, or error code.
 */
FAH_NO_DISCARD FAH_API fah_error_code_t
fah_cookie_jar_parse_set(fah_handle_t jar_h, const char *set_cookie);

/**
 * @brief Build an outgoing `Cookie` header value for `req_url` from `jar_h`.
 * @param[in] jar_h Cookie jar handle.
 * @param[in] req_url Optional target request URL.
 * @param[out] out_buf Destination buffer.
 * @param[in] cap Byte capacity of `out_buf`.
 * @param[out] out_len Receives built header length (`0` if no cookies match).
 * @param[in] space_pad Non-zero to space-pad `out_buf`.
 * @return `FAH_OK` on success, or error code.
 */
FAH_NO_DISCARD FAH_API fah_error_code_t
fah_cookie_jar_build_hdr(fah_handle_t jar_h, const char *req_url, char *out_buf,
                         size_t cap, size_t *out_len, int space_pad);

/* --- HttpRequest, Multipart, Auth, OAuth2, Send & HttpResponse --- */

/**
 * @brief Create a default-initialized `HttpRequest` handle.
 * @param[out] out_req_h Receives positive `FAH_TAG_REQUEST` handle.
 * @return `FAH_OK` on success, or error code.
 */
FAH_NO_DISCARD FAH_API fah_error_code_t
fah_request_create(fah_handle_t *out_req_h);

/**
 * @brief Destroy an `HttpRequest` handle and all owned buffers/parts/headers.
 * @param[in] req_h Request handle.
 * @return `FAH_OK` on success, or handle error.
 */
FAH_NO_DISCARD FAH_API fah_error_code_t fah_request_destroy(fah_handle_t req_h);

/**
 * @brief Set the HTTP verb (`0..9`) and destination URL on `req_h`.
 * @param[in] req_h Request handle.
 * @param[in] method `enum HttpMethod` (`HTTP_GET = 0`, `HTTP_POST = 1`, ...).
 * @param[in] url NUL-terminated destination URL.
 * @return `FAH_OK` on success, or error code.
 */
FAH_NO_DISCARD FAH_API fah_error_code_t
fah_request_set_target(fah_handle_t req_h, int method, const char *url);

/**
 * @brief Add an HTTP header `(key, val)` onto `req_h`.
 * @param[in] req_h Request handle.
 * @param[in] key Header name.
 * @param[in] val Header value.
 * @return `FAH_OK` on success, or error code.
 */
FAH_NO_DISCARD FAH_API fah_error_code_t
fah_request_add_header(fah_handle_t req_h, const char *key, const char *val);

/**
 * @brief Deep-copy an arbitrary binary/text request body onto `req_h`.
 * @param[in] req_h Request handle.
 * @param[in] body Source byte buffer (`NULL` allowed when `body_len == 0`).
 * @param[in] body_len Byte length of `body`.
 * @return `FAH_OK` on success, or error code.
 */
FAH_NO_DISCARD FAH_API fah_error_code_t fah_request_set_body(fah_handle_t req_h,
                                                             const void *body,
                                                             size_t body_len);

/**
 * @brief Ingest any Fortran 2018 `TYPE(*), DIMENSION(..)` array or scalar
 * descriptor into `req_h`'s request body.
 * @param[in] req_h Request handle.
 * @param[in] desc Pointer to Fortran 2018 `CFI_cdesc_t` descriptor.
 * @return `FAH_OK` on success, or `FAH_ERR_CFI_DESCRIPTOR` / `FAH_ERR_NOMEM`.
 */
FAH_NO_DISCARD FAH_API fah_error_code_t
fah_request_set_body_cfi(fah_handle_t req_h, const fah_cfi_cdesc_t *desc);

/**
 * @brief Append an RFC 7578 `multipart/form-data` part to `req_h`.
 * @param[in] req_h Request handle.
 * @param[in] name Form field name.
 * @param[in] filename Optional filename (`NULL` for non-file field).
 * @param[in] content_type Optional MIME type (`NULL` for default).
 * @param[in] data Part byte payload.
 * @param[in] data_len Byte length of `data`.
 * @return `FAH_OK` on success, or error code.
 */
FAH_NO_DISCARD FAH_API fah_error_code_t fah_request_add_part(
    fah_handle_t req_h, const char *name, const char *filename,
    const char *content_type, const void *data, size_t data_len);

/**
 * @brief Attach a per-part header to the most recently added multipart part.
 * @param[in] req_h Request handle.
 * @param[in] key Header name.
 * @param[in] val Header value.
 * @return `FAH_OK` on success, or error code.
 */
FAH_NO_DISCARD FAH_API fah_error_code_t fah_request_add_part_header(
    fah_handle_t req_h, const char *key, const char *val);

/**
 * @brief Explicitly flatten multipart parts on `req_h` into `req->body`.
 * @param[in] req_h Request handle.
 * @return `FAH_OK` on success, or error code.
 */
FAH_NO_DISCARD FAH_API fah_error_code_t
fah_request_flatten_parts(fah_handle_t req_h);

/**
 * @brief Bind streaming callbacks and/or a FORTRAN I-III `FahPollQueue` onto
 * `req_h`.
 * @param[in] req_h Request handle.
 * @param[in] on_chunk Optional download chunk callback (`NULL` to omit).
 * @param[in] chunk_uid Integer user context passed to `on_chunk`.
 * @param[in] read_chunk Optional upload chunk callback (`NULL` to omit).
 * @param[in] read_uid Integer user context passed to `read_chunk`.
 * @param[in] expected_len Expected upload byte size (`0` if unknown).
 * @param[in] on_prog Optional progress callback (`NULL` to omit).
 * @param[in] prog_uid Integer user context passed to `on_prog`.
 * @param[in] poll_q_h Optional `FAH_TAG_POLL_QUEUE` handle (`0` if unused).
 * @return `FAH_OK` on success, or handle error.
 */
FAH_NO_DISCARD FAH_API fah_error_code_t fah_request_set_callbacks(
    fah_handle_t req_h, fah_f77_on_chunk_cb on_chunk, int chunk_uid,
    fah_f77_read_chunk_cb read_chunk, int read_uid, size_t expected_len,
    fah_f77_on_progress_cb on_prog, int prog_uid, fah_handle_t poll_q_h);

/**
 * @brief Set or clear the cooperative cancellation flag on `req_h`.
 * @param[in] req_h Request handle.
 * @param[in] cancel_val Non-zero to trigger `FAH_ERR_CANCELED`, `0` to clear.
 * @return `FAH_OK` on success, or handle error.
 */
FAH_NO_DISCARD FAH_API fah_error_code_t
fah_request_set_cancel(fah_handle_t req_h, int cancel_val);

/**
 * @brief Set Bearer token authorization on `req_h`.
 * @param[in] req_h Request handle.
 * @param[in] token Bearer token string.
 * @return `FAH_OK` on success, or error code.
 */
FAH_NO_DISCARD FAH_API fah_error_code_t fah_auth_bearer(fah_handle_t req_h,
                                                        const char *token);

/**
 * @brief Set pre-encoded Basic authorization token on `req_h`.
 * @param[in] req_h Request handle.
 * @param[in] b64_token Base64-encoded `user:pass` string.
 * @return `FAH_OK` on success, or error code.
 */
FAH_NO_DISCARD FAH_API fah_error_code_t fah_auth_basic(fah_handle_t req_h,
                                                       const char *b64_token);

/**
 * @brief Base64-encode `(username, password)` and set Basic auth on `req_h`.
 * @param[in] req_h Request handle.
 * @param[in] username Username string.
 * @param[in] password Password string.
 * @return `FAH_OK` on success, or error code.
 */
FAH_NO_DISCARD FAH_API fah_error_code_t fah_auth_basic_userpwd(
    fah_handle_t req_h, const char *username, const char *password);

/**
 * @brief Populate `req_h` using any of the 9 `c-abstract-http` OAuth 2.0 grant
 * / revocation / introspection request builders.
 * @param[in] req_h Request handle.
 * @param[in] grant_kind `fah_oauth2_grant_kind_t` (`1..9`).
 * @param[in] endpoint_url Target token/device/revocation/introspection URL.
 * @param[in] p1 Grant parameter 1 (e.g. `username`, `refresh_token`, `code`,
 * `client_id`, `assertion`, `token`).
 * @param[in] p2 Optional parameter 2 (`password`, `redirect_uri`,
 * `client_secret`, `scope`, `device_code`, `token_type_hint`).
 * @param[in] p3 Optional parameter 3 (`client_id`, `scope`).
 * @param[in] p4 Optional parameter 4 (`client_secret`).
 * @param[in] p5 Optional parameter 5 (`scope` or `code_verifier`).
 * @return `FAH_OK` on success, or unified error code.
 */
FAH_NO_DISCARD FAH_API fah_error_code_t
fah_oauth2_grant(fah_handle_t req_h, fah_oauth2_grant_kind_t grant_kind,
                 const char *endpoint_url, const char *p1, const char *p2,
                 const char *p3, const char *p4, const char *p5);

/**
 * @brief Build an OAuth 2.0 browser authorization redirect URL.
 * @param[in] auth_ep Authorization endpoint URL.
 * @param[in] client_id Client identifier.
 * @param[in] resp_type Response type (e.g. `"code"`).
 * @param[in] redirect_uri Optional redirect URI.
 * @param[in] scope Optional scope string.
 * @param[in] state Optional CSRF state string.
 * @param[in] challenge Optional PKCE code challenge.
 * @param[in] challenge_method Optional PKCE method (`"S256"`).
 * @param[out] out_buf Destination URL buffer.
 * @param[in] cap Capacity of `out_buf`.
 * @param[out] out_len Receives full URL byte length.
 * @param[in] space_pad Non-zero to space-pad `out_buf`.
 * @return `FAH_OK` on success, or error code.
 */
FAH_NO_DISCARD FAH_API fah_error_code_t fah_oauth2_build_auth_url(
    const char *auth_ep, const char *client_id, const char *resp_type,
    const char *redirect_uri, const char *scope, const char *state,
    const char *challenge, const char *challenge_method, char *out_buf,
    size_t cap, size_t *out_len, int space_pad);

/**
 * @brief Run synchronous localhost OAuth 2.0 redirect callback capture on
 * `127.0.0.1:port`.
 * @param[in] port Local TCP port to bind.
 * @param[in] html_resp Raw HTTP response sent to the browser.
 * @param[out] code_buf Destination buffer for `code` query parameter.
 * @param[in] code_cap Capacity of `code_buf`.
 * @param[out] out_code_len Receives length of `code`.
 * @param[out] state_buf Destination buffer for `state` query parameter.
 * @param[in] state_cap Capacity of `state_buf`.
 * @param[out] out_state_len Receives length of `state`.
 * @param[in] space_pad Non-zero to space-pad output buffers.
 * @return `FAH_OK` on success, or error code.
 */
FAH_NO_DISCARD FAH_API fah_error_code_t fah_oauth2_localhost_intercept(
    unsigned short port, const char *html_resp, char *code_buf, size_t code_cap,
    size_t *out_code_len, char *state_buf, size_t state_cap,
    size_t *out_state_len, int space_pad);

/**
 * @brief Execute `req_h` through `cli_h` (`http_client_send`) and mint a new
 * `FAH_TAG_RESPONSE` handle in `*out_res_h`.
 * @param[in] cli_h Client handle.
 * @param[in] req_h Request handle.
 * @param[out] out_res_h Receives positive `FAH_TAG_RESPONSE` handle.
 * @return `FAH_OK` on success, or unified error code.
 */
FAH_NO_DISCARD FAH_API fah_error_code_t fah_send(fah_handle_t cli_h,
                                                 fah_handle_t req_h,
                                                 fah_handle_t *out_res_h);

/**
 * @brief Query HTTP status code (e.g. `200`, `404`) from `res_h`.
 * @param[in] res_h Response handle.
 * @param[out] out_status Receives HTTP status code.
 * @return `FAH_OK` on success, or handle/argument error.
 */
FAH_NO_DISCARD FAH_API fah_error_code_t fah_response_status(fah_handle_t res_h,
                                                            int *out_status);

/**
 * @brief Query payload byte length of `res_h`.
 * @param[in] res_h Response handle.
 * @param[out] out_len Receives response body byte count.
 * @return `FAH_OK` on success, or handle/argument error.
 */
FAH_NO_DISCARD FAH_API fah_error_code_t
fah_response_body_len(fah_handle_t res_h, size_t *out_len);

/**
 * @brief Copy response body bytes into caller buffer `out_buf`.
 * @param[in] res_h Response handle.
 * @param[out] out_buf Destination buffer (`NULL` allowed when `cap == 0`).
 * @param[in] cap Byte capacity of `out_buf`.
 * @param[out] out_len Optional pointer receiving full `body_len`.
 * @param[in] space_pad Non-zero to space-pad trailing bytes.
 * @return `FAH_OK` if `body_len <= cap`, or `FAH_ERR_BUF_TOO_SMALL` / error.
 */
FAH_NO_DISCARD FAH_API fah_error_code_t
fah_response_copy_body(fah_handle_t res_h, void *out_buf, size_t cap,
                       size_t *out_len, int space_pad);

/**
 * @brief Obtain a zero-copy raw C pointer and byte length to `res_h`'s body
 * (valid until `fah_response_destroy(res_h)`).
 * @param[in] res_h Response handle.
 * @param[out] out_ptr Receives `const void *` body pointer (or `NULL` if
 * empty).
 * @param[out] out_len Receives `body_len`.
 * @return `FAH_OK` on success, or handle/argument error.
 */
FAH_NO_DISCARD FAH_API fah_error_code_t fah_response_body_ptr(
    fah_handle_t res_h, const void **out_ptr, size_t *out_len);

/**
 * @brief Populate a Fortran 2018 rank-1 `INTEGER(C_INT8_T), ALLOCATABLE ::
 * body(:)` descriptor directly from `res_h` via `CFI_allocate`.
 * @param[in] res_h Response handle.
 * @param[in,out] desc Fortran 2018 `CFI_cdesc_t` allocatable byte array.
 * @return `FAH_OK` on success, or CFI/allocation/handle error.
 */
FAH_NO_DISCARD FAH_API fah_error_code_t
fah_response_body_cfi(fah_handle_t res_h, fah_cfi_cdesc_t *desc);

/**
 * @brief Look up a response header by `key` (case-insensitive) on `res_h`.
 * @param[in] res_h Response handle.
 * @param[in] key Header name.
 * @param[out] out_buf Destination buffer.
 * @param[in] cap Capacity of `out_buf`.
 * @param[out] out_len Optional pointer receiving full header value length.
 * @param[in] space_pad Non-zero to space-pad `out_buf`.
 * @return `FAH_OK` on match, `FAH_ERR_INVAL` if missing, or
 * `FAH_ERR_BUF_TOO_SMALL`.
 */
FAH_NO_DISCARD FAH_API fah_error_code_t
fah_response_get_header(fah_handle_t res_h, const char *key, char *out_buf,
                        size_t cap, size_t *out_len, int space_pad);

/**
 * @brief Query the number of response headers on `res_h` and optionally read
 * index `idx_0based`.
 * @param[in] res_h Response handle.
 * @param[out] out_count Receives total header count.
 * @return `FAH_OK` on success, or handle/argument error.
 */
FAH_NO_DISCARD FAH_API fah_error_code_t
fah_response_header_count(fah_handle_t res_h, size_t *out_count);

/**
 * @brief Retrieve the `(key, value)` header at 0-based index `idx` on `res_h`.
 * @param[in] res_h Response handle.
 * @param[in] idx Zero-based header index.
 * @param[out] key_buf Destination key buffer.
 * @param[in] key_cap Capacity of `key_buf`.
 * @param[out] out_key_len Optional pointer receiving full key length.
 * @param[out] val_buf Destination value buffer.
 * @param[in] val_cap Capacity of `val_buf`.
 * @param[out] out_val_len Optional pointer receiving full value length.
 * @param[in] space_pad Non-zero to space-pad output buffers.
 * @return `FAH_OK` on success, `FAH_ERR_INVAL` if `idx >= count`, or
 * `FAH_ERR_BUF_TOO_SMALL`.
 */
FAH_NO_DISCARD FAH_API fah_error_code_t
fah_response_header_at(fah_handle_t res_h, size_t idx, char *key_buf,
                       size_t key_cap, size_t *out_key_len, char *val_buf,
                       size_t val_cap, size_t *out_val_len, int space_pad);

/**
 * @brief Write `res_h`'s body payload to `file_path`
 * (`http_response_save_to_file`).
 * @param[in] res_h Response handle.
 * @param[in] file_path Target filesystem path.
 * @return `FAH_OK` on success, or error code.
 */
FAH_NO_DISCARD FAH_API fah_error_code_t
fah_response_save_file(fah_handle_t res_h, const char *file_path);

/**
 * @brief Free an `HttpResponse` handle and all owned memory.
 * @param[in] res_h Response handle.
 * @return `FAH_OK` on success, or handle error.
 */
FAH_NO_DISCARD FAH_API fah_error_code_t
fah_response_destroy(fah_handle_t res_h);

/* --- Concurrent Multi-Requests & Futures --- */

/**
 * @brief Create an `HttpMultiRequest` batch handle.
 * @param[out] out_multi_h Receives positive `FAH_TAG_MULTI_REQ` handle.
 * @return `FAH_OK` on success, or error code.
 */
FAH_NO_DISCARD FAH_API fah_error_code_t
fah_multi_create(fah_handle_t *out_multi_h);

/**
 * @brief Add a request handle `req_h` into `multi_h`.
 * @param[in] multi_h Multi-request handle.
 * @param[in] req_h Request handle to enqueue.
 * @return `FAH_OK` on success, or error code.
 */
FAH_NO_DISCARD FAH_API fah_error_code_t fah_multi_add(fah_handle_t multi_h,
                                                      fah_handle_t req_h);

/**
 * @brief Dispatch all requests in `multi_h` concurrently via
 * `http_client_send_multi` and populate `out_future_handles` with minted
 * `FAH_TAG_FUTURE` handles.
 * @param[in] cli_h Client handle.
 * @param[in] multi_h Multi-request handle.
 * @param[in] fail_fast `1` to abort remaining transfers on first failure.
 * @param[out] out_future_handles Caller array receiving `FAH_TAG_FUTURE` IDs.
 * @param[in] cap_futures Element capacity of `out_future_handles`.
 * @param[out] out_count Receives number of futures created.
 * @return `FAH_OK` on success, or error code.
 */
FAH_NO_DISCARD FAH_API fah_error_code_t fah_multi_send(
    fah_handle_t cli_h, fah_handle_t multi_h, int fail_fast,
    fah_handle_t *out_future_handles, size_t cap_futures, size_t *out_count);

/**
 * @brief Destroy an `HttpMultiRequest` container handle.
 * @param[in] multi_h Multi-request handle.
 * @return `FAH_OK` on success, or handle error.
 */
FAH_NO_DISCARD FAH_API fah_error_code_t fah_multi_destroy(fah_handle_t multi_h);

/**
 * @brief Block until `fut_h` completes and report its transfer result in
 * `*out_xfer_err`.
 * @param[in] fut_h Future handle.
 * @param[out] out_xfer_err Receives completed transfer error (`FAH_OK` = ok).
 * @return `FAH_OK` when `fut_h` is valid, or handle error.
 */
FAH_NO_DISCARD FAH_API fah_error_code_t
fah_future_await(fah_handle_t fut_h, fah_error_code_t *out_xfer_err);

/**
 * @brief Request cancellation of `fut_h`.
 * @param[in] fut_h Future handle.
 * @return `FAH_OK` on success, or error code.
 */
FAH_NO_DISCARD FAH_API fah_error_code_t fah_future_cancel(fah_handle_t fut_h);

/**
 * @brief Poll readiness and transfer status of `fut_h` without blocking.
 * @param[in] fut_h Future handle.
 * @param[out] out_is_ready Receives `1` if completed, `0` otherwise.
 * @param[out] out_err_code Receives future `error_code`.
 * @return `FAH_OK` on success, or handle error.
 */
FAH_NO_DISCARD FAH_API fah_error_code_t fah_future_poll(fah_handle_t fut_h,
                                                        int *out_is_ready,
                                                        int *out_err_code);

/**
 * @brief Detach the completed `HttpResponse *` from `fut_h` and register it as
 * a standalone `FAH_TAG_RESPONSE` handle.
 * @param[in] fut_h Completed future handle.
 * @param[out] out_res_h Receives `FAH_TAG_RESPONSE` handle.
 * @return `FAH_OK` on success, `FAH_ERR_INVAL` if no response available, or
 * handle/allocation error.
 */
FAH_NO_DISCARD FAH_API fah_error_code_t
fah_future_take_response(fah_handle_t fut_h, fah_handle_t *out_res_h);

/**
 * @brief Destroy an `HttpFuture` handle (freeing any unclaimed response).
 * @param[in] fut_h Future handle.
 * @return `FAH_OK` on success, or handle error.
 */
FAH_NO_DISCARD FAH_API fah_error_code_t fah_future_destroy(fah_handle_t fut_h);

/* --- WebSockets & Server-Sent Events --- */

/**
 * @brief Initialize WebSocket upgrade state on `req_h`.
 * @param[in] req_h Request handle.
 * @param[in] subprotocols Optional comma-separated subprotocols (`NULL` ok).
 * @param[in] masking_pref `1` to mask outbound client frames.
 * @return `FAH_OK` on success, or error code.
 */
FAH_NO_DISCARD FAH_API fah_error_code_t fah_ws_init(fah_handle_t req_h,
                                                    const char *subprotocols,
                                                    int masking_pref);

/**
 * @brief Run synchronous WebSocket frame loop on `(cli_h, req_h)`.
 * @param[in] cli_h Client handle.
 * @param[in] req_h Initialized WebSocket request handle.
 * @param[in] on_msg Optional message callback (`NULL` when using `poll_q_h`).
 * @param[in] on_err Optional error callback (`NULL` ok).
 * @param[in] on_close Optional close callback (`NULL` ok).
 * @param[in] user_id Integer user context passed to callbacks.
 * @param[in] poll_q_h Optional `FAH_TAG_POLL_QUEUE` handle (`0` if unused).
 * @return `FAH_OK` on clean close, or error code.
 */
FAH_NO_DISCARD FAH_API fah_error_code_t fah_ws_sync_loop(
    fah_handle_t cli_h, fah_handle_t req_h, fah_f77_ws_on_msg_cb on_msg,
    fah_f77_ws_on_err_cb on_err, fah_f77_ws_on_close_cb on_close, int user_id,
    fah_handle_t poll_q_h);

/**
 * @brief Perform non-blocking WebSocket `101` upgrade and attach `req_h` onto
 * `loop_h` (sets `client->loop`).
 * @param[in] cli_h Client handle.
 * @param[in] loop_h Event loop handle (`FAH_TAG_EVENT_LOOP`).
 * @param[in] req_h WebSocket request handle.
 * @param[in] on_msg Optional message callback (`NULL` ok).
 * @param[in] on_err Optional error callback (`NULL` ok).
 * @param[in] on_close Optional close callback (`NULL` ok).
 * @param[in] user_id Integer user context.
 * @param[in] poll_q_h Optional `FAH_TAG_POLL_QUEUE` handle (`0` if unused).
 * @return `FAH_OK` on success, or error code.
 */
FAH_NO_DISCARD FAH_API fah_error_code_t fah_ws_async_connect(
    fah_handle_t cli_h, fah_handle_t loop_h, fah_handle_t req_h,
    fah_f77_ws_on_msg_cb on_msg, fah_f77_ws_on_err_cb on_err,
    fah_f77_ws_on_close_cb on_close, int user_id, fah_handle_t poll_q_h);

/**
 * @brief Send a synchronous (`is_async == 0`) or asynchronous (`is_async != 0`)
 * WebSocket frame on `req_h`.
 * @param[in] req_h Connected WebSocket request handle.
 * @param[in] opcode RFC 6455 opcode (`1` = text, `2` = binary, `9` = ping).
 * @param[in] payload Frame payload bytes (`NULL` allowed when `len == 0`).
 * @param[in] len Byte length of `payload`.
 * @param[in] is_async Non-zero for `c_abstract_http_ws_send_async`.
 * @return `FAH_OK` on success, or error code.
 */
FAH_NO_DISCARD FAH_API fah_error_code_t fah_ws_send(fah_handle_t req_h,
                                                    int opcode,
                                                    const void *payload,
                                                    size_t len, int is_async);

/**
 * @brief Send a WebSocket close frame (`status_code`, e.g. `1000`) on `req_h`.
 * @param[in] req_h WebSocket request handle.
 * @param[in] status_code RFC 6455 closure status code.
 * @return `FAH_OK` on success, or error code.
 */
FAH_NO_DISCARD FAH_API fah_error_code_t fah_ws_close(fah_handle_t req_h,
                                                     int status_code);

/**
 * @brief Release WebSocket state attached to `req_h`.
 * @param[in] req_h WebSocket request handle.
 * @return `FAH_OK` on success, or error code.
 */
FAH_NO_DISCARD FAH_API fah_error_code_t fah_ws_free(fah_handle_t req_h);

/**
 * @brief Initialize Server-Sent Events streaming state on `req_h`.
 * @param[in] req_h Request handle.
 * @param[in] last_event_id Optional `Last-Event-ID` string (`NULL` ok).
 * @param[in] retry_ms Initial reconnect backoff in ms (`0` for default).
 * @return `FAH_OK` on success, or error code.
 */
FAH_NO_DISCARD FAH_API fah_error_code_t fah_sse_init(fah_handle_t req_h,
                                                     const char *last_event_id,
                                                     int retry_ms);

/**
 * @brief Run synchronous blocking SSE stream loop on `(cli_h, req_h)`.
 * @param[in] cli_h Client handle.
 * @param[in] req_h SSE request handle.
 * @param[in] on_evt Optional event callback (`NULL` ok).
 * @param[in] on_err Optional error callback (`NULL` ok).
 * @param[in] on_close Optional close callback (`NULL` ok).
 * @param[in] user_id Integer user context.
 * @param[in] poll_q_h Optional `FAH_TAG_POLL_QUEUE` handle (`0` if unused).
 * @return `FAH_OK` on clean stream completion, or error code.
 */
FAH_NO_DISCARD FAH_API fah_error_code_t fah_sse_sync_loop(
    fah_handle_t cli_h, fah_handle_t req_h, fah_f77_sse_on_evt_cb on_evt,
    fah_f77_sse_on_err_cb on_err, fah_f77_sse_on_close_cb on_close, int user_id,
    fah_handle_t poll_q_h);

/**
 * @brief Register non-blocking SSE stream on `(cli_h, loop_h, req_h)`.
 * @param[in] cli_h Client handle.
 * @param[in] loop_h Event loop handle (`FAH_TAG_EVENT_LOOP`).
 * @param[in] req_h SSE request handle.
 * @param[in] on_evt Optional event callback (`NULL` ok).
 * @param[in] on_err Optional error callback (`NULL` ok).
 * @param[in] on_close Optional close callback (`NULL` ok).
 * @param[in] user_id Integer user context.
 * @param[in] poll_q_h Optional `FAH_TAG_POLL_QUEUE` handle (`0` if unused).
 * @return `FAH_OK` on success, or error code.
 */
FAH_NO_DISCARD FAH_API fah_error_code_t fah_sse_async_register(
    fah_handle_t cli_h, fah_handle_t loop_h, fah_handle_t req_h,
    fah_f77_sse_on_evt_cb on_evt, fah_f77_sse_on_err_cb on_err,
    fah_f77_sse_on_close_cb on_close, int user_id, fah_handle_t poll_q_h);

/**
 * @brief Release SSE context attached to `req_h`.
 * @param[in] req_h SSE request handle.
 * @return `FAH_OK` on success, or error code.
 */
FAH_NO_DISCARD FAH_API fah_error_code_t fah_sse_free(fah_handle_t req_h);

/* --- Execution & Concurrency Modalities --- */

/**
 * @brief Create a `ModalityEventLoop` handle (`backend = 0` for `AUTO`).
 * @param[in] backend `enum HttpLoopBackend` (`0..4`).
 * @param[out] out_loop_h Receives positive `FAH_TAG_EVENT_LOOP` handle.
 * @return `FAH_OK` on success, or error code.
 */
FAH_NO_DISCARD FAH_API fah_error_code_t
fah_loop_create(int backend, fah_handle_t *out_loop_h);

/**
 * @brief Query active I/O backend of `loop_h`.
 * @param[in] loop_h Event loop handle.
 * @param[out] out_backend Receives `enum HttpLoopBackend`.
 * @return `FAH_OK` on success, or error code.
 */
FAH_NO_DISCARD FAH_API fah_error_code_t
fah_loop_get_backend(fah_handle_t loop_h, int *out_backend);

/**
 * @brief Drive `loop_h` (`mode`: `0` = non-blocking `tick`, `1` = `run`, `2` =
 * `stop`, `3` = `wakeup`).
 * @param[in] loop_h Event loop handle.
 * @param[in] mode Operation selector (`0..3`).
 * @return `FAH_OK` on success, or error code.
 */
FAH_NO_DISCARD FAH_API fah_error_code_t fah_loop_drive(fah_handle_t loop_h,
                                                       int mode);

/**
 * @brief Schedule a one-shot timer on `loop_h`.
 * @param[in] loop_h Event loop handle.
 * @param[in] timeout_ms Delay in milliseconds.
 * @param[in] cb Fortran timer callback.
 * @param[in] user_id Integer user context.
 * @param[out] out_timer_id Receives timer identifier.
 * @return `FAH_OK` on success, or error code.
 */
FAH_NO_DISCARD FAH_API fah_error_code_t
fah_loop_add_timer(fah_handle_t loop_h, long timeout_ms,
                   fah_f77_loop_timer_cb cb, int user_id, int *out_timer_id);

/**
 * @brief Cancel a pending timer on `loop_h`.
 * @param[in] loop_h Event loop handle.
 * @param[in] timer_id Timer ID returned by `fah_loop_add_timer`.
 * @return `FAH_OK` on success, or error code.
 */
FAH_NO_DISCARD FAH_API fah_error_code_t
fah_loop_cancel_timer(fah_handle_t loop_h, int timer_id);

/**
 * @brief Destroy an event loop handle and free registered timer contexts.
 * @param[in] loop_h Event loop handle.
 * @return `FAH_OK` on success, or error code.
 */
FAH_NO_DISCARD FAH_API fah_error_code_t fah_loop_destroy(fah_handle_t loop_h);

/**
 * @brief Query monotonic/wall timestamp in milliseconds.
 * @param[out] out_ms Receives 64-bit timestamp in ms.
 * @return `FAH_OK` on success, or `FAH_ERR_INVAL` if `out_ms == NULL`.
 */
FAH_NO_DISCARD FAH_API fah_error_code_t
fah_time_ms(abstract_http_int64_t *out_ms);

/**
 * @brief Create, lock, unlock, or destroy an `AbstractHttpMutex` (`op`: `0` =
 * create, `1` = lock, `2` = unlock, `3` = destroy).
 * @param[in,out] io_mtx_h Mutex handle pointer.
 * @param[in] op Operation selector (`0..3`).
 * @return `FAH_OK` on success, or error code.
 */
FAH_NO_DISCARD FAH_API fah_error_code_t fah_mutex_op(fah_handle_t *io_mtx_h,
                                                     int op);

/**
 * @brief Create, signal, broadcast, or destroy an `AbstractHttpCond` (`op`:
 * `0` = create, `1` = signal, `2` = broadcast, `3` = destroy).
 * @param[in,out] io_cond_h Condition variable handle pointer.
 * @param[in] op Operation selector (`0..3`).
 * @return `FAH_OK` on success, or error code.
 */
FAH_NO_DISCARD FAH_API fah_error_code_t fah_cond_op(fah_handle_t *io_cond_h,
                                                    int op);

/**
 * @brief Create an `AbstractHttpThreadPool` handle.
 * @param[in] num_threads Worker thread count (`>= 1`).
 * @param[out] out_pool_h Receives positive `FAH_TAG_THREAD_POOL` handle.
 * @return `FAH_OK` on success, or error code.
 */
FAH_NO_DISCARD FAH_API fah_error_code_t
fah_pool_create(size_t num_threads, fah_handle_t *out_pool_h);

/**
 * @brief Enqueue a Fortran task callback onto `pool_h`.
 * @param[in] pool_h Thread pool handle.
 * @param[in] cb Fortran task callback.
 * @param[in] user_id Integer user context.
 * @return `FAH_OK` on success, or error code.
 */
FAH_NO_DISCARD FAH_API fah_error_code_t fah_pool_submit(fah_handle_t pool_h,
                                                        fah_f77_task_cb cb,
                                                        int user_id);

/**
 * @brief Drain and destroy a thread pool handle.
 * @param[in] pool_h Thread pool handle.
 * @return `FAH_OK` on success, or error code.
 */
FAH_NO_DISCARD FAH_API fah_error_code_t fah_pool_destroy(fah_handle_t pool_h);

/**
 * @brief Create, set integer, get integer, or delete a Thread-Local Storage key
 * (`op`: `0` = create, `1` = set `*io_val`, `2` = get `*io_val`, `3` = delete).
 * @param[in,out] io_tls_h TLS key handle pointer.
 * @param[in] op Operation selector (`0..3`).
 * @param[in,out] io_val Integer value pointer for set/get (`NULL` ok on 0/3).
 * @return `FAH_OK` on success, or error code.
 */
FAH_NO_DISCARD FAH_API fah_error_code_t fah_tls_op(fah_handle_t *io_tls_h,
                                                   int op, int *io_val);

/**
 * @brief Create a stackful `AbstractHttpCoroutine` executing `cb(user_id)`.
 * @param[in] stack_size Stack size in bytes (`0` for default).
 * @param[in] cb Fortran coroutine body callback.
 * @param[in] user_id Integer user context.
 * @param[out] out_coro_h Receives positive `FAH_TAG_COROUTINE` handle.
 * @return `FAH_OK` on success, or error code.
 */
FAH_NO_DISCARD FAH_API fah_error_code_t
fah_coro_create(size_t stack_size, fah_f77_task_cb cb, int user_id,
                fah_handle_t *out_coro_h);

/**
 * @brief Resume, yield, check completion, or destroy a coroutine (`op`: `0` =
 * resume, `1` = yield active coroutine, `2` = query `*out_done`, `3` =
 * destroy).
 * @param[in] coro_h Coroutine handle (ignored when `op == 1`).
 * @param[in] op Operation selector (`0..3`).
 * @param[out] out_done Receives `1` if finished when `op == 2`.
 * @return `FAH_OK` on success, or error code.
 */
FAH_NO_DISCARD FAH_API fah_error_code_t fah_coro_op(fah_handle_t coro_h, int op,
                                                    int *out_done);

/**
 * @brief Create an `AbstractHttpMessageBus` handle.
 * @param[out] out_bus_h Receives positive `FAH_TAG_MSG_BUS` handle.
 * @return `FAH_OK` on success, or error code.
 */
FAH_NO_DISCARD FAH_API fah_error_code_t fah_bus_create(fah_handle_t *out_bus_h);

/**
 * @brief Spawn an `AbstractHttpActor` on `bus_h`.
 * @param[in] bus_h Message bus handle.
 * @param[in] name Actor name string.
 * @param[in] cb Fortran actor mailbox handler.
 * @param[in] user_id Integer actor state ID.
 * @param[out] out_actor_h Receives positive `FAH_TAG_ACTOR` handle.
 * @return `FAH_OK` on success, or error code.
 */
FAH_NO_DISCARD FAH_API fah_error_code_t
fah_actor_spawn(fah_handle_t bus_h, const char *name, fah_f77_actor_cb cb,
                int user_id, fah_handle_t *out_actor_h);

/**
 * @brief Send an integer-valued message to `receiver_h` on `bus_h`.
 * @param[in] bus_h Message bus handle.
 * @param[in] sender_h Optional sender actor handle (`0` if external).
 * @param[in] receiver_h Target actor handle.
 * @param[in] msg_type Message type code.
 * @param[in] msg_val Integer payload value.
 * @return `FAH_OK` on success, or error code.
 */
FAH_NO_DISCARD FAH_API fah_error_code_t
fah_actor_send_int(fah_handle_t bus_h, fah_handle_t sender_h,
                   fah_handle_t receiver_h, int msg_type, int msg_val);

/**
 * @brief Process queued actor messages on `bus_h`.
 * @param[in] bus_h Message bus handle.
 * @param[out] out_processed Receives count of processed messages.
 * @return `FAH_OK` on success, or error code.
 */
FAH_NO_DISCARD FAH_API fah_error_code_t fah_bus_process(fah_handle_t bus_h,
                                                        int *out_processed);

/**
 * @brief Query the name of `actor_h`.
 * @param[in] actor_h Actor handle.
 * @param[out] out_buf Destination buffer.
 * @param[in] cap Capacity of `out_buf`.
 * @param[out] out_len Optional pointer receiving full name length.
 * @param[in] space_pad Non-zero to space-pad `out_buf`.
 * @return `FAH_OK` on success, or error code.
 */
FAH_NO_DISCARD FAH_API fah_error_code_t fah_actor_get_name(fah_handle_t actor_h,
                                                           char *out_buf,
                                                           size_t cap,
                                                           size_t *out_len,
                                                           int space_pad);

/**
 * @brief Unregister an actor handle (`bus_destroy` frees the underlying C
 * actor).
 * @param[in] actor_h Actor handle.
 * @return `FAH_OK` on success, or error code.
 */
FAH_NO_DISCARD FAH_API fah_error_code_t fah_actor_destroy(fah_handle_t actor_h);

/**
 * @brief Destroy a message bus handle and its actors.
 * @param[in] bus_h Message bus handle.
 * @return `FAH_OK` on success, or error code.
 */
FAH_NO_DISCARD FAH_API fah_error_code_t fah_bus_destroy(fah_handle_t bus_h);

/**
 * @brief Exercise OS IPC pipe write + read framing and request/response binary
 * IPC serialization (`op`: `0` = pipe roundtrip on `buf`, `1` = serialize &
 * deserialize `in_h` request -> `*out_h`, `2` = serialize & deserialize `in_h`
 * response -> `*out_h`).
 * @param[in] op IPC operation (`0..2`).
 * @param[in] in_h Source request/response handle (for `op` 1 or 2).
 * @param[in] in_buf Input payload for pipe roundtrip (`op == 0`).
 * @param[in] in_len Byte length of `in_buf`.
 * @param[out] out_buf Output buffer for pipe roundtrip (`op == 0`).
 * @param[in] out_cap Capacity of `out_buf`.
 * @param[out] out_h Receives deserialized request/response handle (`op` 1/2).
 * @return `FAH_OK` on success, or error code.
 */
FAH_NO_DISCARD FAH_API fah_error_code_t fah_ipc_op(int op, fah_handle_t in_h,
                                                   const void *in_buf,
                                                   size_t in_len, void *out_buf,
                                                   size_t out_cap,
                                                   fah_handle_t *out_h);

/**
 * @brief `c-multiplatform` (`cmp_integration.h`) modality adaptation, config
 * injection, and progress/cancellation binding (`op`: `0` = adapt `cmp_mod` ->
 * `*out_mod`, `1` = inject `(cmp_mod, min_t, max_t)` into `target_h` config,
 * `2` = bind new `CmpProgressBinding` onto `target_h` request -> `*out_cmp_h`,
 * `3` = trigger `cancel_requested = 1` on `target_h` CMP binding, `4` = detach
 * & destroy `target_h` CMP binding).
 * @param[in] op CMP operation selector (`0..4`).
 * @param[in] cmp_mod `enum CmpModality` (`0..5`).
 * @param[in] min_t Minimum threads (for `op == 1`).
 * @param[in] max_t Maximum threads (for `op == 1`).
 * @param[in] target_h Config/Request/CMP handle depending on `op`.
 * @param[out] out_mod Receives adapted `ExecutionModality` (`op == 0`).
 * @param[out] out_cmp_h Receives `FAH_TAG_CMP_BINDING` handle (`op == 2`).
 * @return `FAH_OK` on success, or error code.
 */
FAH_NO_DISCARD FAH_API fah_error_code_t fah_cmp_op(int op, int cmp_mod,
                                                   size_t min_t, size_t max_t,
                                                   fah_handle_t target_h,
                                                   int *out_mod,
                                                   fah_handle_t *out_cmp_h);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* FAH_C_BRIDGE_H */
