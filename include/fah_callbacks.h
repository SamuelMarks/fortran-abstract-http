/**
 * @file fah_callbacks.h
 * @brief Re-entrant C-to-Fortran callback trampolines for FORTRAN IV through
 * Fortran 95 (`EXTERNAL` pass-by-reference ABI) and Fortran 2003-2023
 * (`ISO_C_BINDING` C ABI).
 */

#ifndef FAH_CALLBACKS_H
#define FAH_CALLBACKS_H

/* clang-format off */
#include <stddef.h>
#include "c_abstract_http/actor.h"
#include "c_abstract_http/coroutine.h"
#include "c_abstract_http/event_loop.h"
#include "c_abstract_http/http_sse.h"
#include "c_abstract_http/http_types.h"
#include "c_abstract_http/http_ws.h"
#include "c_abstract_http/thread_pool.h"
#include "fah_error.h"
#include "fah_export.h"
#include "fah_handle_registry.h"
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @brief FORTRAN IV-95 `EXTERNAL` response chunk callback (`0` = continue,
 * non-zero = abort).
 */
typedef int (*fah_f77_on_chunk_cb)(const int *user_id, const char *chunk,
                                   const int *chunk_len);

/**
 * @brief FORTRAN IV-95 `EXTERNAL` request upload chunk callback (`0` = ok,
 * sets `*out_len = 0` on EOF).
 */
typedef int (*fah_f77_read_chunk_cb)(const int *user_id, char *buf,
                                     const int *buf_cap, int *out_len);

/**
 * @brief FORTRAN IV-95 `EXTERNAL` transfer progress callback (`0` = continue).
 */
typedef int (*fah_f77_on_progress_cb)(const int *cur_bytes,
                                      const int *tot_bytes, const int *user_id);

/**
 * @brief FORTRAN IV-95 `EXTERNAL` WebSocket incoming message callback.
 */
typedef int (*fah_f77_ws_on_msg_cb)(const int *opcode, const char *payload,
                                    const int *payload_len, const int *is_fin,
                                    const int *user_id);

/**
 * @brief FORTRAN IV-95 `EXTERNAL` WebSocket error callback.
 */
typedef int (*fah_f77_ws_on_err_cb)(const int *err_code, const int *user_id);

/**
 * @brief FORTRAN IV-95 `EXTERNAL` WebSocket close callback.
 */
typedef int (*fah_f77_ws_on_close_cb)(const int *status_code,
                                      const int *user_id);

/**
 * @brief FORTRAN IV-95 `EXTERNAL` SSE event callback.
 */
typedef int (*fah_f77_sse_on_evt_cb)(const char *id_str, const int *id_len,
                                     const char *evt_str, const int *evt_len,
                                     const char *data_str, const int *data_len,
                                     const int *user_id);

/**
 * @brief FORTRAN IV-95 `EXTERNAL` SSE error callback.
 */
typedef int (*fah_f77_sse_on_err_cb)(const int *err_code, const int *user_id);

/**
 * @brief FORTRAN IV-95 `EXTERNAL` SSE close callback.
 */
typedef int (*fah_f77_sse_on_close_cb)(const int *user_id);

/**
 * @brief FORTRAN IV-95 `EXTERNAL` event-loop file descriptor readiness
 * callback.
 */
typedef int (*fah_f77_loop_fd_cb)(const int *loop_h, const int *fd,
                                  const int *events, const int *user_id);

/**
 * @brief FORTRAN IV-95 `EXTERNAL` event-loop timer callback.
 */
typedef int (*fah_f77_loop_timer_cb)(const int *loop_h, const int *timer_id,
                                     const int *user_id);

/**
 * @brief FORTRAN IV-95 `EXTERNAL` worker task / coroutine callback.
 */
typedef int (*fah_f77_task_cb)(const int *user_id);

/**
 * @brief FORTRAN IV-95 `EXTERNAL` actor mailbox handler callback.
 */
typedef int (*fah_f77_actor_cb)(const int *actor_h, const int *msg_type,
                                const int *msg_val, const int *sender_h,
                                const int *user_id);

/**
 * @brief Per-request Fortran callback state bound to a `FahRequestBox`.
 */
typedef struct FahReqCallbackCtx {
  fah_f77_on_chunk_cb on_chunk;       /**< Optional chunk receiver */
  int on_chunk_uid;                   /**< User integer context for on_chunk */
  fah_f77_read_chunk_cb read_chunk;   /**< Optional chunk uploader */
  int read_chunk_uid;                 /**< User integer context for upload */
  fah_f77_on_progress_cb on_progress; /**< Optional progress receiver */
  int on_progress_uid;                /**< User integer context for progress */
  fah_handle_t poll_queue_h;          /**< Optional FORTRAN I-III poll queue */
} FahReqCallbackCtx;

/**
 * @brief Active WebSocket callback bundle for FORTRAN IV-95 `EXTERNAL` loops.
 */
typedef struct FahWsCallbackCtx {
  fah_f77_ws_on_msg_cb on_msg;     /**< Message callback */
  fah_f77_ws_on_err_cb on_err;     /**< Error callback */
  fah_f77_ws_on_close_cb on_close; /**< Close callback */
  int user_id;                     /**< Integer user context */
  fah_handle_t poll_queue_h;       /**< Optional FORTRAN I-III poll queue */
} FahWsCallbackCtx;

/**
 * @brief Active SSE callback bundle for FORTRAN IV-95 `EXTERNAL` loops.
 */
typedef struct FahSseCallbackCtx {
  fah_f77_sse_on_evt_cb on_evt;     /**< Event callback */
  fah_f77_sse_on_err_cb on_err;     /**< Error callback */
  fah_f77_sse_on_close_cb on_close; /**< Close callback */
  int user_id;                      /**< Integer user context */
  fah_handle_t poll_queue_h;        /**< Optional FORTRAN I-III poll queue */
} FahSseCallbackCtx;

/**
 * @brief C-to-Fortran `http_on_chunk_fn` trampoline.
 * @param[in] user_data Pointer to `FahReqCallbackCtx`.
 * @param[in] chunk Received byte buffer.
 * @param[in] len Byte count of `chunk`.
 * @return `C_ABSTRACT_HTTP_SUCCESS` or `C_ABSTRACT_HTTP_ERR_CANCELED`.
 */
FAH_NO_DISCARD FAH_API c_abstract_http_error_t
fah_trampoline_on_chunk(void *user_data, const void *chunk, size_t len);

/**
 * @brief C-to-Fortran `http_read_chunk_fn` trampoline.
 * @param[in] user_data Pointer to `FahReqCallbackCtx`.
 * @param[out] buf Destination upload buffer.
 * @param[in] buf_len Maximum byte capacity of `buf`.
 * @param[out] out_len Receives bytes written (`0` = EOF).
 * @return `C_ABSTRACT_HTTP_SUCCESS` or error code on callback abort.
 */
FAH_NO_DISCARD FAH_API c_abstract_http_error_t fah_trampoline_read_chunk(
    void *user_data, void *buf, size_t buf_len, size_t *out_len);

/**
 * @brief C-to-Fortran `http_on_progress_fn` trampoline.
 * @param[in] current_bytes Transferred byte count.
 * @param[in] total_bytes Expected total byte count (`0` if unknown).
 * @param[in] user_data Pointer to `FahReqCallbackCtx`.
 * @return `C_ABSTRACT_HTTP_SUCCESS` or `C_ABSTRACT_HTTP_ERR_CANCELED`.
 */
FAH_NO_DISCARD FAH_API c_abstract_http_error_t fah_trampoline_on_progress(
    size_t current_bytes, size_t total_bytes, void *user_data);

/**
 * @brief C-to-Fortran `c_abstract_http_ws_on_message` trampoline.
 * @param[in] ev Parsed WebSocket frame event.
 * @param[in] user_data Pointer to `FahWsCallbackCtx`.
 * @return `C_ABSTRACT_HTTP_SUCCESS` or error code.
 */
FAH_NO_DISCARD FAH_API c_abstract_http_error_t fah_trampoline_ws_on_msg(
    const struct c_abstract_http_ws_event *ev, void *user_data);

/**
 * @brief C-to-Fortran `c_abstract_http_ws_on_error` trampoline.
 * @param[in] error_code WebSocket error code.
 * @param[in] user_data Pointer to `FahWsCallbackCtx`.
 * @return `C_ABSTRACT_HTTP_SUCCESS` or error code.
 */
FAH_NO_DISCARD FAH_API c_abstract_http_error_t
fah_trampoline_ws_on_err(int error_code, void *user_data);

/**
 * @brief C-to-Fortran `c_abstract_http_ws_on_close` trampoline.
 * @param[in] status_code RFC 6455 close status code.
 * @param[in] user_data Pointer to `FahWsCallbackCtx`.
 * @return `C_ABSTRACT_HTTP_SUCCESS` or error code.
 */
FAH_NO_DISCARD FAH_API c_abstract_http_error_t
fah_trampoline_ws_on_close(int status_code, void *user_data);

/**
 * @brief C-to-Fortran `c_abstract_http_sse_on_event` trampoline.
 * @param[in] ev Parsed Server-Sent Event.
 * @param[in] user_data Pointer to `FahSseCallbackCtx`.
 * @return `C_ABSTRACT_HTTP_SUCCESS` or error code.
 */
FAH_NO_DISCARD FAH_API c_abstract_http_error_t fah_trampoline_sse_on_evt(
    const struct c_abstract_http_sse_event *ev, void *user_data);

/**
 * @brief C-to-Fortran `c_abstract_http_sse_on_error` trampoline.
 * @param[in] error_code SSE error code.
 * @param[in] user_data Pointer to `FahSseCallbackCtx`.
 * @return `C_ABSTRACT_HTTP_SUCCESS` or error code.
 */
FAH_NO_DISCARD FAH_API c_abstract_http_error_t
fah_trampoline_sse_on_err(int error_code, void *user_data);

/**
 * @brief C-to-Fortran `c_abstract_http_sse_on_close` trampoline.
 * @param[in] user_data Pointer to `FahSseCallbackCtx`.
 * @return `C_ABSTRACT_HTTP_SUCCESS` or error code.
 */
FAH_NO_DISCARD FAH_API c_abstract_http_error_t
fah_trampoline_sse_on_close(void *user_data);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* FAH_CALLBACKS_H */
