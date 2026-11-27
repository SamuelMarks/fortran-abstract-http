/**
 * @file fah_fortran_entries.c
 * @brief Fortran-mangled (`FAH_FC_GLOBAL`) external functions and subroutines
 * bridging FORTRAN (I), II, III, IV, 66, 77, Fortran 90, and Fortran 95 to
 * `fah_c_bridge`.
 */

/* clang-format off */
#include <limits.h>
#include <string.h>
#include "fah_alloc.h"
#include "fah_c_bridge.h"
#include "fah_f77_strings.h"
#include "fah_hollerith.h"
/* clang-format on */

/* Forward declarations satisfying -Wmissing-prototypes */

/**
 * @brief FORTRAN I-66 global init (`IERR = IHGINI(IDUMMY)`).
 * @param[in] dummy Unused dummy argument required by 1957 FORTRAN I syntax.
 * @return Unified error code (`0` on success).
 */
FAH_API int FAH_FC_GLOBAL(ihgini, IHGINI)(const int *dummy);
/**
 * @brief FORTRAN I-66 global cleanup (`IERR = IHGCLN(IDUMMY)`).
 * @param[in] dummy Unused dummy argument.
 * @return Unified error code.
 */
FAH_API int FAH_FC_GLOBAL(ihgcln, IHGCLN)(const int *dummy);
/**
 * @brief FORTRAN I-66 poll queue create (`IERR = IHPQCR(IOUTQ)`).
 * @param[out] out_q Receives positive poll queue handle.
 * @return Unified error code.
 */
FAH_API int FAH_FC_GLOBAL(ihpqcr, IHPQCR)(int *out_q);
/**
 * @brief FORTRAN I-66 poll queue destroy (`IERR = IHPQFR(IQH)`).
 * @param[in] q_h Poll queue handle.
 * @return Unified error code.
 */
FAH_API int FAH_FC_GLOBAL(ihpqfr, IHPQFR)(const int *q_h);
/**
 * @brief FORTRAN I-66 poll queue pop into unpacked integer character array.
 * @param[in] q_h Poll queue handle.
 * @param[out] out_kind Event kind (`0` = empty).
 * @param[out] out_code Event opcode/status/error code.
 * @param[out] ibuf Unpacked integer payload buffer.
 * @param[in] icap Capacity of `ibuf`.
 * @param[out] out_len Payload byte length.
 * @return Unified error code.
 */
FAH_API int FAH_FC_GLOBAL(ihpqpp, IHPQPP)(const int *q_h, int *out_kind,
                                          int *out_code, int *ibuf,
                                          const int *icap, int *out_len);
/**
 * @brief FORTRAN I-66 config create (`IERR = IHCFCR(IOUTCF)`).
 * @param[out] out_cfg Receives config handle.
 * @return Unified error code.
 */
FAH_API int FAH_FC_GLOBAL(ihcfcr, IHCFCR)(int *out_cfg);
/**
 * @brief FORTRAN I-66 config destroy (`IERR = IHCFFR(ICFH)`).
 * @param[in] cfg_h Config handle.
 * @return Unified error code.
 */
FAH_API int FAH_FC_GLOBAL(ihcffr, IHCFFR)(const int *cfg_h);
/**
 * @brief FORTRAN I-66 config timeouts & flags (`IERR = IHCFST(...)`).
 * @param[in] cfg_h Config handle.
 * @param[in] timeout_ms General timeout in ms.
 * @param[in] verify_peer TLS peer verification flag.
 * @param[in] follow_redirs Follow 3xx redirects flag.
 * @param[in] retry_cnt Max retry count.
 * @return Unified error code.
 */
FAH_API int FAH_FC_GLOBAL(ihcfst, IHCFST)(const int *cfg_h,
                                          const int *timeout_ms,
                                          const int *verify_peer,
                                          const int *follow_redirs,
                                          const int *retry_cnt);
/**
 * @brief FORTRAN I-66 client create (`IERR = IHCLCR(ICFH, IRAW, IOUTCL)`).
 * @param[in] cfg_h Optional config handle (`0` for defaults).
 * @param[in] use_raw `1` for raw socket transport, `0` for OS default.
 * @param[out] out_cli Receives client handle.
 * @return Unified error code.
 */
FAH_API int FAH_FC_GLOBAL(ihclcr, IHCLCR)(const int *cfg_h, const int *use_raw,
                                          int *out_cli);
/**
 * @brief FORTRAN I-66 client destroy (`IERR = IHCLFR(ICLH)`).
 * @param[in] cli_h Client handle.
 * @return Unified error code.
 */
FAH_API int FAH_FC_GLOBAL(ihclfr, IHCLFR)(const int *cli_h);
/**
 * @brief FORTRAN I-66 request create (`IERR = IHRQCR(IOUTRQ)`).
 * @param[out] out_req Receives request handle.
 * @return Unified error code.
 */
FAH_API int FAH_FC_GLOBAL(ihrqcr, IHRQCR)(int *out_req);
/**
 * @brief FORTRAN I-66 request destroy (`IERR = IHRQFR(IRQH)`).
 * @param[in] req_h Request handle.
 * @return Unified error code.
 */
FAH_API int FAH_FC_GLOBAL(ihrqfr, IHRQFR)(const int *req_h);
/**
 * @brief FORTRAN I-66 set request target URL (`IMODE=0` unpacked `INTEGER`
 * codes, `IMODE=1` 4-char Hollerith words).
 * @param[in] req_h Request handle.
 * @param[in] method HTTP method (`0..9`).
 * @param[in] mode `0` = unpacked integer codes, `1` = 4-char Hollerith.
 * @param[in] iurl Integer/Hollerith URL buffer.
 * @param[in] nurl Character count of URL.
 * @return Unified error code.
 */
FAH_API int FAH_FC_GLOBAL(ihrqtg, IHRQTG)(const int *req_h, const int *method,
                                          const int *mode, const int *iurl,
                                          const int *nurl);
/**
 * @brief FORTRAN I-66 add request header from unpacked/Hollerith integer
 * arrays.
 * @param[in] req_h Request handle.
 * @param[in] mode `0` = unpacked integer codes, `1` = 4-char Hollerith.
 * @param[in] ikey Header name array.
 * @param[in] nkey Header name char count.
 * @param[in] ival Header value array.
 * @param[in] nval Header value char count.
 * @return Unified error code.
 */
FAH_API int FAH_FC_GLOBAL(ihrqhd, IHRQHD)(const int *req_h, const int *mode,
                                          const int *ikey, const int *nkey,
                                          const int *ival, const int *nval);
/**
 * @brief FORTRAN I-66 set request body from unpacked/Hollerith integer array.
 * @param[in] req_h Request handle.
 * @param[in] mode `0` = unpacked integer codes, `1` = 4-char Hollerith.
 * @param[in] ibdy Body array.
 * @param[in] nbdy Character/byte count.
 * @return Unified error code.
 */
FAH_API int FAH_FC_GLOBAL(ihrqbd, IHRQBD)(const int *req_h, const int *mode,
                                          const int *ibdy, const int *nbdy);
/**
 * @brief FORTRAN I-III attach poll queue onto request for streaming chunks.
 * @param[in] req_h Request handle.
 * @param[in] q_h Poll queue handle.
 * @return Unified error code.
 */
FAH_API int FAH_FC_GLOBAL(ihrqcb, IHRQCB)(const int *req_h, const int *q_h);
/**
 * @brief FORTRAN I-66 synchronous HTTP send (`IERR = IHSEND(ICLH, IRQH,
 * IRES)`).
 * @param[in] cli_h Client handle.
 * @param[in] req_h Request handle.
 * @param[out] out_res Receives response handle.
 * @return Unified error code.
 */
FAH_API int FAH_FC_GLOBAL(ihsend, IHSEND)(const int *cli_h, const int *req_h,
                                          int *out_res);
/**
 * @brief FORTRAN I-66 query HTTP status and body length (`IHRSIN`).
 * @param[in] res_h Response handle.
 * @param[out] out_status Receives HTTP status code.
 * @param[out] out_len Receives response body byte length.
 * @return Unified error code.
 */
FAH_API int FAH_FC_GLOBAL(ihrsin, IHRSIN)(const int *res_h, int *out_status,
                                          int *out_len);
/**
 * @brief FORTRAN I-66 copy response body into unpacked/Hollerith integer array.
 * @param[in] res_h Response handle.
 * @param[in] mode `0` = unpacked integer codes, `1` = 4-char Hollerith.
 * @param[out] ibuf Destination integer array.
 * @param[in] icap Capacity of `ibuf` (elements for mode 0, words for mode 1).
 * @param[out] out_len Actual byte length.
 * @return Unified error code.
 */
FAH_API int FAH_FC_GLOBAL(ihrsbd, IHRSBD)(const int *res_h, const int *mode,
                                          int *ibuf, const int *icap,
                                          int *out_len);
/**
 * @brief FORTRAN I-66 get response header in unpacked/Hollerith mode.
 * @param[in] res_h Response handle.
 * @param[in] mode `0` = unpacked integer codes, `1` = 4-char Hollerith.
 * @param[in] ikey Header key array.
 * @param[in] nkey Key char count.
 * @param[out] ival Destination value array.
 * @param[in] icap Capacity of `ival`.
 * @param[out] out_len Actual value char count.
 * @return Unified error code.
 */
FAH_API int FAH_FC_GLOBAL(ihrshd, IHRSHD)(const int *res_h, const int *mode,
                                          const int *ikey, const int *nkey,
                                          int *ival, const int *icap,
                                          int *out_len);
/**
 * @brief FORTRAN I-66 response destroy (`IERR = IHRSFR(IRSH)`).
 * @param[in] res_h Response handle.
 * @return Unified error code.
 */
FAH_API int FAH_FC_GLOBAL(ihrsfr, IHRSFR)(const int *res_h);
/**
 * @brief FORTRAN I-66 poll-queue WebSocket dispatcher (`IOP`: `0`=init,
 * `1`=sync_loop, `2`=send, `3`=close, `4`=free).
 * @param[in] cli_h Client handle.
 * @param[in] req_h Request handle.
 * @param[in] op Operation code (`0..4`).
 * @param[in] arg Opcode/masking/close status.
 * @param[in] mode `0` = unpacked ints, `1` = Hollerith.
 * @param[in] ibuf Payload array (for `op == 2`).
 * @param[in] nlen Payload char count.
 * @param[in] q_h Poll queue handle (for `op == 1`).
 * @return Unified error code.
 */
FAH_API int FAH_FC_GLOBAL(ihwsop, IHWSOP)(const int *cli_h, const int *req_h,
                                          const int *op, const int *arg,
                                          const int *mode, const int *ibuf,
                                          const int *nlen, const int *q_h);
/**
 * @brief FORTRAN I-66 poll-queue SSE dispatcher (`IOP`: `0`=init,
 * `1`=sync_loop, `2`=free).
 * @param[in] cli_h Client handle.
 * @param[in] req_h Request handle.
 * @param[in] op Operation code (`0..2`).
 * @param[in] retry_ms Retry backoff ms.
 * @param[in] q_h Poll queue handle.
 * @return Unified error code.
 */
FAH_API int FAH_FC_GLOBAL(ihsseo, IHSSEO)(const int *cli_h, const int *req_h,
                                          const int *op, const int *retry_ms,
                                          const int *q_h);
/**
 * @brief FORTRAN I-66 error metadata & unpacked integer formatting (`IHERRC`).
 * @param[in] code Error code to inspect.
 * @param[out] out_cat Receives `fah_error_category_t`.
 * @param[out] out_retry Receives `1` if retryable, `0` otherwise.
 * @param[out] ibuf Unpacked integer diagnostic buffer.
 * @param[in] icap Capacity of `ibuf`.
 * @param[out] out_len Formatted string length.
 * @return Unified error code.
 */
FAH_API int FAH_FC_GLOBAL(iherrc, IHERRC)(const int *code, int *out_cat,
                                          int *out_retry, int *ibuf,
                                          const int *icap, int *out_len);

/* FORTRAN IV & 66 EXTERNAL callback registration entry points */

/**
 * @brief FORTRAN IV-95 bind `EXTERNAL` streaming callbacks onto `req_h`.
 * @param[in] req_h Request handle.
 * @param[in] on_chunk `EXTERNAL` chunk callback.
 * @param[in] chunk_uid Integer user context.
 * @param[in] on_prog `EXTERNAL` progress callback.
 * @param[in] prog_uid Integer user context.
 * @param[out] out_err Unified error code.
 */
FAH_API void FAH_FC_GLOBAL(fhc4st, FHC4ST)(const int *req_h,
                                           fah_f77_on_chunk_cb on_chunk,
                                           const int *chunk_uid,
                                           fah_f77_on_progress_cb on_prog,
                                           const int *prog_uid, int *out_err);

/**
 * @brief FORTRAN IV-95 bind `EXTERNAL` upload `read_chunk` callback onto
 * `req_h`.
 * @param[in] req_h Request handle.
 * @param[in] read_chunk `EXTERNAL` upload chunk callback.
 * @param[in] read_uid Integer user context.
 * @param[in] expected_len Expected upload byte count.
 * @param[out] out_err Unified error code.
 */
FAH_API void FAH_FC_GLOBAL(fhc4up, FHC4UP)(const int *req_h,
                                           fah_f77_read_chunk_cb read_chunk,
                                           const int *read_uid,
                                           const int *expected_len,
                                           int *out_err);

/**
 * @brief FORTRAN IV-95 WebSocket sync loop with `EXTERNAL` callbacks.
 * @param[in] cli_h Client handle.
 * @param[in] req_h Request handle.
 * @param[in] on_msg `EXTERNAL` message callback.
 * @param[in] on_err `EXTERNAL` error callback.
 * @param[in] on_close `EXTERNAL` close callback.
 * @param[in] uid Integer user context.
 * @param[out] out_err Unified error code.
 */
FAH_API void FAH_FC_GLOBAL(fhc4ws, FHC4WS)(const int *cli_h, const int *req_h,
                                           fah_f77_ws_on_msg_cb on_msg,
                                           fah_f77_ws_on_err_cb on_err,
                                           fah_f77_ws_on_close_cb on_close,
                                           const int *uid, int *out_err);

/**
 * @brief FORTRAN IV-95 SSE sync loop with `EXTERNAL` callbacks.
 * @param[in] cli_h Client handle.
 * @param[in] req_h Request handle.
 * @param[in] on_evt `EXTERNAL` SSE event callback.
 * @param[in] on_err `EXTERNAL` error callback.
 * @param[in] on_close `EXTERNAL` close callback.
 * @param[in] uid Integer user context.
 * @param[out] out_err Unified error code.
 */
FAH_API void FAH_FC_GLOBAL(fhc4ss, FHC4SS)(const int *cli_h, const int *req_h,
                                           fah_f77_sse_on_evt_cb on_evt,
                                           fah_f77_sse_on_err_cb on_err,
                                           fah_f77_sse_on_close_cb on_close,
                                           const int *uid, int *out_err);

/* FORTRAN 77, Fortran 90 & Fortran 95 CHARACTER*(*) entry points */

/**
 * @brief F77-F95 numeric/handle `HttpConfig` mutator (`IOP`: `1`=timeouts,
 * `2`=tls_flags, `3`=versions_retry, `4`=modality_pool, `5`=cookie_jar).
 * @param[in] cfg_h Config handle.
 * @param[in] op Operation selector (`1..5`).
 * @param[in] a1 Parameter 1.
 * @param[in] a2 Parameter 2.
 * @param[in] a3 Parameter 3.
 * @param[in] a4 Parameter 4.
 * @param[out] out_err Unified error code.
 */
FAH_API void FAH_FC_GLOBAL(fh7cfg, FH7CFG)(const int *cfg_h, const int *op,
                                           const int *a1, const int *a2,
                                           const int *a3, const int *a4,
                                           int *out_err);

/**
 * @brief F77-F95 string `HttpConfig` & client `base_url` mutator (`ISLOT`:
 * `1..8` = config string fields, `9` = `fah_client_set_base_url`).
 * @param[in] hnd Config or client handle.
 * @param[in] slot String field selector (`1..9`).
 * @param[in] str Fortran `CHARACTER*(*)` buffer.
 * @param[out] out_err Unified error code.
 * @param[in] str_len Hidden compiler character length.
 */
FAH_API void FAH_FC_GLOBAL(fh7cfs, FH7CFS)(const int *hnd, const int *slot,
                                           const char *str, int *out_err,
                                           fah_charlen_t str_len);

/**
 * @brief F77-F95 standalone `HttpHeaders` & `HttpCookieJar` operations (`IOP`:
 * `1`=hdr_create, `2`=hdr_destroy, `3`=hdr_add, `4`=hdr_get, `5`=hdr_remove,
 * `6`=hdr_count, `11`=jar_create, `12`=jar_destroy, `13`=jar_set,
 * `14`=jar_get, `15`=jar_parse_set, `16`=jar_build_hdr).
 * @param[in,out] io_h Container handle.
 * @param[in] op Operation selector.
 * @param[in] s1 Input string 1.
 * @param[in,out] s2 Input/output string 2.
 * @param[out] out_n Output count or string length.
 * @param[out] out_err Unified error code.
 * @param[in] s1_len Hidden length of `s1`.
 * @param[in] s2_len Hidden length of `s2`.
 */
FAH_API void FAH_FC_GLOBAL(fh7hcj, FH7HCJ)(int *io_h, const int *op,
                                           const char *s1, char *s2, int *out_n,
                                           int *out_err, fah_charlen_t s1_len,
                                           fah_charlen_t s2_len);

/**
 * @brief F77-F95 `HttpRequest` & Basic/Bearer Auth `CHARACTER*(*)` operations
 * (`IOP`: `1`=set_target, `2`=add_header, `3`=set_body_trimmed,
 * `4`=set_body_exact_len, `5`=add_part, `6`=add_part_hdr, `7`=flatten_parts,
 * `8`=set_cancel, `9`=auth_bearer, `10`=auth_basic, `11`=auth_basic_userpwd).
 * @param[in] req_h Request handle.
 * @param[in] op Operation selector (`1..11`).
 * @param[in] iarg Method / explicit byte length / cancel flag.
 * @param[in] s1 String argument 1.
 * @param[in] s2 String argument 2.
 * @param[in] s3 String argument 3.
 * @param[in] s4 String/payload argument 4.
 * @param[out] out_err Unified error code.
 * @param[in] l1 Hidden length of `s1`.
 * @param[in] l2 Hidden length of `s2`.
 * @param[in] l3 Hidden length of `s3`.
 * @param[in] l4 Hidden length of `s4`.
 */
FAH_API void FAH_FC_GLOBAL(fh7req, FH7REQ)(const int *req_h, const int *op,
                                           const int *iarg, const char *s1,
                                           const char *s2, const char *s3,
                                           const char *s4, int *out_err,
                                           fah_charlen_t l1, fah_charlen_t l2,
                                           fah_charlen_t l3, fah_charlen_t l4);

/**
 * @brief F77-F95 OAuth 2.0 grants (`IGRANT = 1..9`), `build_auth_url`
 * (`IGRANT = 10`), and `localhost_intercept` (`IGRANT = 11`).
 * @param[in] req_h Request handle (or port for `IGRANT == 11`).
 * @param[in] grant_kind Selector (`1..11`).
 * @param[in] ep Endpoint URL / HTML response.
 * @param[in] p1 Parameter 1.
 * @param[in] p2 Parameter 2.
 * @param[in] p3 Parameter 3.
 * @param[in] p4 Parameter 4.
 * @param[in] p5 Parameter 5.
 * @param[out] out_s1 Output URL or captured `code`.
 * @param[out] out_l1 Length of `out_s1`.
 * @param[out] out_s2 Captured `state` (`IGRANT == 11`).
 * @param[out] out_l2 Length of `out_s2`.
 * @param[out] out_err Unified error code.
 * @param[in] eplen Hidden length of `ep`.
 * @param[in] l1 Hidden length of `p1`.
 * @param[in] l2 Hidden length of `p2`.
 * @param[in] l3 Hidden length of `p3`.
 * @param[in] l4 Hidden length of `p4`.
 * @param[in] l5 Hidden length of `p5`.
 * @param[in] ol1 Hidden length of `out_s1`.
 * @param[in] ol2 Hidden length of `out_s2`.
 */
FAH_API void FAH_FC_GLOBAL(fh7oa2, FH7OA2)(
    const int *req_h, const int *grant_kind, const char *ep, const char *p1,
    const char *p2, const char *p3, const char *p4, const char *p5,
    char *out_s1, int *out_l1, char *out_s2, int *out_l2, int *out_err,
    fah_charlen_t eplen, fah_charlen_t l1, fah_charlen_t l2, fah_charlen_t l3,
    fah_charlen_t l4, fah_charlen_t l5, fah_charlen_t ol1, fah_charlen_t ol2);

/**
 * @brief F77-F95 `HttpResponse` `CHARACTER*(*)` inspection & file download
 * (`IOP`: `1`=copy_body_padded, `2`=get_header, `3`=header_count,
 * `4`=header_at, `5`=save_file).
 * @param[in] res_h Response handle.
 * @param[in] op Operation selector (`1..5`).
 * @param[in] idx Zero-based header index (`IOP == 4`).
 * @param[in,out] s1 Key input (`2`), key output (`4`), or file path (`5`).
 * @param[out] s2 Body/value output buffer.
 * @param[out] out_n1 Output length 1 / header count.
 * @param[out] out_n2 Output length 2.
 * @param[out] out_err Unified error code.
 * @param[in] l1 Hidden length of `s1`.
 * @param[in] l2 Hidden length of `s2`.
 */
FAH_API void FAH_FC_GLOBAL(fh7res, FH7RES)(const int *res_h, const int *op,
                                           const int *idx, char *s1, char *s2,
                                           int *out_n1, int *out_n2,
                                           int *out_err, fah_charlen_t l1,
                                           fah_charlen_t l2);

/**
 * @brief F77-F95 `HttpMultiRequest` & `HttpFuture` batch operations (`IOP`:
 * `1`=multi_create, `2`=multi_add, `3`=multi_send, `4`=multi_destroy,
 * `5`=fut_await, `6`=fut_cancel, `7`=fut_poll, `8`=fut_take_res,
 * `9`=fut_destroy).
 * @param[in] cli_h Client handle.
 * @param[in,out] io_h Multi or future handle.
 * @param[in] op Operation selector (`1..9`).
 * @param[in] arg Request handle / `fail_fast`.
 * @param[out] fut_arr Array of future handles (`IOP == 3`).
 * @param[in] cap Capacity of `fut_arr`.
 * @param[out] out_n1 Output count / `xfer_err` / `is_ready` / `res_h`.
 * @param[out] out_n2 Output `err_code` (`IOP == 7`).
 * @param[out] out_err Unified error code.
 */
FAH_API void FAH_FC_GLOBAL(fh7mul,
                           FH7MUL)(const int *cli_h, int *io_h, const int *op,
                                   const int *arg, int *fut_arr, const int *cap,
                                   int *out_n1, int *out_n2, int *out_err);

/**
 * @brief F77-F95 WebSocket & SSE `CHARACTER*(*)` + `EXTERNAL` operations
 * (`IOP`: `1`=ws_init, `2`=ws_async_connect, `3`=ws_send_str,
 * `4`=ws_send_async_str, `5`=ws_close, `6`=ws_free, `11`=sse_init,
 * `12`=sse_async_register, `13`=sse_free).
 * @param[in] cli_h Client handle.
 * @param[in] loop_h Event loop handle.
 * @param[in] req_h Request handle.
 * @param[in] op Operation selector.
 * @param[in] iarg Opcode / masking / close code / retry_ms.
 * @param[in] str Subprotocols / payload / Last-Event-ID.
 * @param[in] cb1 `EXTERNAL` ws `on_msg` or sse `on_evt`.
 * @param[in] cb2 `EXTERNAL` `on_err`.
 * @param[in] cb3 `EXTERNAL` `on_close`.
 * @param[in] uid Integer user context.
 * @param[out] out_err Unified error code.
 * @param[in] slen Hidden length of `str`.
 */
FAH_API void FAH_FC_GLOBAL(fh7stm, FH7STM)(const int *cli_h, const int *loop_h,
                                           const int *req_h, const int *op,
                                           const int *iarg, const char *str,
                                           void (*cb1)(void), void (*cb2)(void),
                                           void (*cb3)(void), const int *uid,
                                           int *out_err, fah_charlen_t slen);

/**
 * @brief F77-F95 Execution & Concurrency Modalities (`EventLoop`, `Mutex`,
 * `Cond`, `ThreadPool`, `TLS`, `Coroutine`, `MessageBus`/`Actor`, `IPC`, and
 * `CMP`).
 * @param[in] op Modality operation selector.
 * @param[in,out] io_h1 Primary handle.
 * @param[in,out] io_h2 Secondary handle.
 * @param[in] a1 Integer argument 1.
 * @param[in] a2 Integer argument 2.
 * @param[in] a3 Integer argument 3.
 * @param[in,out] str String argument / output buffer.
 * @param[in] cb `EXTERNAL` timer / task / coroutine / actor callback.
 * @param[out] out_n1 Integer result 1.
 * @param[out] out_err Unified error code.
 * @param[in] slen Hidden length of `str`.
 */
FAH_API void FAH_FC_GLOBAL(fh7mod,
                           FH7MOD)(const int *op, int *io_h1, int *io_h2,
                                   const int *a1, const int *a2, const int *a3,
                                   char *str, void (*cb)(void), int *out_n1,
                                   int *out_err, fah_charlen_t slen);

/**
 * @brief F77-F95 `CHARACTER*(*)` structured error formatter (`derive_more`
 * Display equivalent).
 * @param[in] code Error code.
 * @param[in] op_name Originating operation name.
 * @param[out] out_msg Space-padded diagnostic string buffer.
 * @param[out] out_cat Receives `fah_error_category_t`.
 * @param[out] out_retry Receives `1` if retryable.
 * @param[out] out_len Untruncated diagnostic length.
 * @param[out] out_err Formatting return status (`FAH_OK` or
 * `FAH_ERR_BUF_TOO_SMALL`).
 * @param[in] oplen Hidden length of `op_name`.
 * @param[in] msglen Hidden length of `out_msg`.
 */
FAH_API void FAH_FC_GLOBAL(fh7err, FH7ERR)(const int *code, const char *op_name,
                                           char *out_msg, int *out_cat,
                                           int *out_retry, int *out_len,
                                           int *out_err, fah_charlen_t oplen,
                                           fah_charlen_t msglen);

/* --- Internal Helpers --- */

/**
 * @brief Decode either unpacked `INTEGER` codes (`mode == 0`) or 4-char
 * Hollerith words (`mode != 0`) into a heap C string.
 * @param[in] mode `0` = unpacked codes, non-zero = 4-char Hollerith.
 * @param[in] arr Source `INTEGER` array.
 * @param[in] nlen Character count.
 * @param[out] out_str Receives heap C string.
 * @return `FAH_OK` or error code.
 */
static fah_error_code_t fah_decode_legacy_text(int mode, const int *arr,
                                               int nlen, char **out_str) {
  if (mode == 0) {
    return fah_icodes_to_cstr(arr, nlen, out_str);
  }
  return fah_hollerith4_to_cstr(arr, nlen, out_str);
}

/**
 * @brief Encode raw C bytes into either unpacked `INTEGER` codes (`mode == 0`)
 * or 4-char Hollerith words (`mode != 0`).
 * @param[in] mode `0` = unpacked codes, non-zero = 4-char Hollerith.
 * @param[in] src Source byte buffer.
 * @param[in] src_len Source byte length.
 * @param[out] out_arr Destination `INTEGER` array.
 * @param[in] cap Element/word capacity of `out_arr`.
 * @param[out] out_len Actual byte count.
 * @return `FAH_OK` or error code.
 */
static fah_error_code_t fah_encode_legacy_bytes(int mode, const void *src,
                                                size_t src_len, int *out_arr,
                                                int cap, int *out_len) {
  if (mode == 0) {
    return fah_cbytes_to_icodes(src, src_len, out_arr, cap, out_len);
  }
  return fah_cbytes_to_hollerith4(src, src_len, out_arr, cap, out_len);
}

/* --- Group 1: IH* Implementations --- */

int FAH_FC_GLOBAL(ihgini, IHGINI)(const int *dummy) {
  (void)dummy;
  return (int)fah_global_init();
}

int FAH_FC_GLOBAL(ihgcln, IHGCLN)(const int *dummy) {
  (void)dummy;
  return (int)fah_global_cleanup();
}

int FAH_FC_GLOBAL(ihpqcr, IHPQCR)(int *out_q) {
  return (int)fah_poll_queue_create(out_q);
}

int FAH_FC_GLOBAL(ihpqfr, IHPQFR)(const int *q_h) {
  return (q_h != NULL) ? (int)fah_poll_queue_destroy(*q_h) : (int)FAH_ERR_INVAL;
}

int FAH_FC_GLOBAL(ihpqpp, IHPQPP)(const int *q_h, int *out_kind, int *out_code,
                                  int *ibuf, const int *icap, int *out_len) {
  char evt_dummy[64];
  size_t evt_l = 0u;
  size_t dat_l = 0u;
  void *tmp = NULL;
  size_t bcap;
  fah_error_code_t rc;
  fah_error_code_t rc2;

  if (q_h == NULL || icap == NULL || *icap < 0 || out_len == NULL) {
    return (int)FAH_ERR_INVAL;
  }
  bcap = (*icap > 0) ? (size_t)(*icap) : 1u;
  rc = fah_malloc(bcap, &tmp);
  if (rc != FAH_OK) {
    return (int)rc;
  }
  rc =
      fah_poll_queue_pop(*q_h, out_kind, out_code, evt_dummy, sizeof(evt_dummy),
                         &evt_l, tmp, (size_t)(*icap), &dat_l);
  *out_len = (int)dat_l;
  if (rc == FAH_OK && *out_kind != (int)FAH_POLL_NONE) {
    rc2 = fah_cbytes_to_icodes(tmp, dat_l, ibuf, *icap, out_len);
    fah_free(tmp);
    return (int)rc2;
  }
  fah_free(tmp);
  return (int)rc;
}

int FAH_FC_GLOBAL(ihcfcr, IHCFCR)(int *out_cfg) {
  return (int)fah_config_create(out_cfg);
}

int FAH_FC_GLOBAL(ihcffr, IHCFFR)(const int *cfg_h) {
  return (cfg_h != NULL) ? (int)fah_config_destroy(*cfg_h) : (int)FAH_ERR_INVAL;
}

int FAH_FC_GLOBAL(ihcfst, IHCFST)(const int *cfg_h, const int *timeout_ms,
                                  const int *verify_peer,
                                  const int *follow_redirs,
                                  const int *retry_cnt) {
  fah_error_code_t rc;
  if (cfg_h == NULL || timeout_ms == NULL || verify_peer == NULL ||
      follow_redirs == NULL || retry_cnt == NULL) {
    return (int)FAH_ERR_INVAL;
  }
  rc = fah_config_set_timeouts(*cfg_h, (long)(*timeout_ms), 0L, 0L, 0L);
  if (rc != FAH_OK) {
    return (int)rc;
  }
  FAH_IGNORE_RC(fah_config_set_tls_flags(*cfg_h, *verify_peer, *verify_peer,
                                         *follow_redirs, 1));
  return (int)fah_config_set_versions_and_retry(*cfg_h, 0u, 0u, *retry_cnt,
                                                (*retry_cnt > 0) ? 1 : 0);
}

int FAH_FC_GLOBAL(ihclcr, IHCLCR)(const int *cfg_h, const int *use_raw,
                                  int *out_cli) {
  if (cfg_h == NULL || use_raw == NULL) {
    return (int)FAH_ERR_INVAL;
  }
  return (int)fah_client_create(*cfg_h, *use_raw, out_cli);
}

int FAH_FC_GLOBAL(ihclfr, IHCLFR)(const int *cli_h) {
  return (cli_h != NULL) ? (int)fah_client_destroy(*cli_h) : (int)FAH_ERR_INVAL;
}

int FAH_FC_GLOBAL(ihrqcr, IHRQCR)(int *out_req) {
  return (int)fah_request_create(out_req);
}

int FAH_FC_GLOBAL(ihrqfr, IHRQFR)(const int *req_h) {
  return (req_h != NULL) ? (int)fah_request_destroy(*req_h)
                         : (int)FAH_ERR_INVAL;
}

int FAH_FC_GLOBAL(ihrqtg, IHRQTG)(const int *req_h, const int *method,
                                  const int *mode, const int *iurl,
                                  const int *nurl) {
  char *c_url = NULL;
  fah_error_code_t rc;
  if (req_h == NULL || method == NULL || mode == NULL || nurl == NULL) {
    return (int)FAH_ERR_INVAL;
  }
  rc = fah_decode_legacy_text(*mode, iurl, *nurl, &c_url);
  if (rc != FAH_OK) {
    return (int)rc;
  }
  rc = fah_request_set_target(*req_h, *method, c_url);
  fah_free(c_url);
  return (int)rc;
}

int FAH_FC_GLOBAL(ihrqhd, IHRQHD)(const int *req_h, const int *mode,
                                  const int *ikey, const int *nkey,
                                  const int *ival, const int *nval) {
  char *c_k = NULL;
  char *c_v = NULL;
  fah_error_code_t rc;
  if (req_h == NULL || mode == NULL || nkey == NULL || nval == NULL) {
    return (int)FAH_ERR_INVAL;
  }
  rc = fah_decode_legacy_text(*mode, ikey, *nkey, &c_k);
  if (rc != FAH_OK) {
    return (int)rc;
  }
  rc = fah_decode_legacy_text(*mode, ival, *nval, &c_v);
  if (rc != FAH_OK) {
    fah_free(c_k);
    return (int)rc;
  }
  rc = fah_request_add_header(*req_h, c_k, c_v);
  fah_free(c_k);
  fah_free(c_v);
  return (int)rc;
}

int FAH_FC_GLOBAL(ihrqbd, IHRQBD)(const int *req_h, const int *mode,
                                  const int *ibdy, const int *nbdy) {
  char *c_b = NULL;
  fah_error_code_t rc;
  if (req_h == NULL || mode == NULL || nbdy == NULL || *nbdy < 0) {
    return (int)FAH_ERR_INVAL;
  }
  rc = fah_decode_legacy_text(*mode, ibdy, *nbdy, &c_b);
  if (rc != FAH_OK) {
    return (int)rc;
  }
  rc = fah_request_set_body(*req_h, c_b, (size_t)(*nbdy));
  fah_free(c_b);
  return (int)rc;
}

int FAH_FC_GLOBAL(ihrqcb, IHRQCB)(const int *req_h, const int *q_h) {
  if (req_h == NULL || q_h == NULL) {
    return (int)FAH_ERR_INVAL;
  }
  return (int)fah_request_set_callbacks(*req_h, NULL, 0, NULL, 0, 0u, NULL, 0,
                                        *q_h);
}

int FAH_FC_GLOBAL(ihsend, IHSEND)(const int *cli_h, const int *req_h,
                                  int *out_res) {
  if (cli_h == NULL || req_h == NULL) {
    return (int)FAH_ERR_INVAL;
  }
  return (int)fah_send(*cli_h, *req_h, out_res);
}

int FAH_FC_GLOBAL(ihrsin, IHRSIN)(const int *res_h, int *out_status,
                                  int *out_len) {
  size_t blen = 0u;
  fah_error_code_t rc;
  if (res_h == NULL || out_status == NULL || out_len == NULL) {
    return (int)FAH_ERR_INVAL;
  }
  rc = fah_response_status(*res_h, out_status);
  if (rc != FAH_OK) {
    return (int)rc;
  }
  rc = fah_response_body_len(*res_h, &blen);
  *out_len = (int)blen;
  return (int)rc;
}

int FAH_FC_GLOBAL(ihrsbd, IHRSBD)(const int *res_h, const int *mode, int *ibuf,
                                  const int *icap, int *out_len) {
  const void *bptr = NULL;
  size_t blen = 0u;
  fah_error_code_t rc;
  if (res_h == NULL || mode == NULL || icap == NULL || out_len == NULL) {
    return (int)FAH_ERR_INVAL;
  }
  rc = fah_response_body_ptr(*res_h, &bptr, &blen);
  if (rc != FAH_OK) {
    return (int)rc;
  }
  return (int)fah_encode_legacy_bytes(*mode, bptr, blen, ibuf, *icap, out_len);
}

int FAH_FC_GLOBAL(ihrshd, IHRSHD)(const int *res_h, const int *mode,
                                  const int *ikey, const int *nkey, int *ival,
                                  const int *icap, int *out_len) {
  char *c_k = NULL;
  char vbuf[512];
  size_t vlen = 0u;
  fah_error_code_t rc;
  if (res_h == NULL || mode == NULL || nkey == NULL || icap == NULL ||
      out_len == NULL) {
    return (int)FAH_ERR_INVAL;
  }
  rc = fah_decode_legacy_text(*mode, ikey, *nkey, &c_k);
  if (rc != FAH_OK) {
    return (int)rc;
  }
  rc = fah_response_get_header(*res_h, c_k, vbuf, sizeof(vbuf), &vlen, 0);
  fah_free(c_k);
  if (rc != FAH_OK) {
    return (int)rc;
  }
  return (int)fah_encode_legacy_bytes(*mode, vbuf, vlen, ival, *icap, out_len);
}

int FAH_FC_GLOBAL(ihrsfr, IHRSFR)(const int *res_h) {
  return (res_h != NULL) ? (int)fah_response_destroy(*res_h)
                         : (int)FAH_ERR_INVAL;
}

int FAH_FC_GLOBAL(ihwsop, IHWSOP)(const int *cli_h, const int *req_h,
                                  const int *op, const int *arg,
                                  const int *mode, const int *ibuf,
                                  const int *nlen, const int *q_h) {
  if (req_h == NULL || op == NULL || arg == NULL) {
    return (int)FAH_ERR_INVAL;
  }
  switch (*op) {
  case 0:
    return (int)fah_ws_init(*req_h, NULL, *arg);
  case 1:
    if (cli_h == NULL || q_h == NULL)
      return (int)FAH_ERR_INVAL;
    return (int)fah_ws_sync_loop(*cli_h, *req_h, NULL, NULL, NULL, 0, *q_h);
  case 2: {
    char *p = NULL;
    fah_error_code_t rc;
    if (mode == NULL || nlen == NULL || *nlen < 0)
      return (int)FAH_ERR_INVAL;
    rc = fah_decode_legacy_text(*mode, ibuf, *nlen, &p);
    if (rc != FAH_OK)
      return (int)rc;
    rc = fah_ws_send(*req_h, *arg, p, (size_t)(*nlen), 0);
    fah_free(p);
    return (int)rc;
  }
  case 3:
    return (int)fah_ws_close(*req_h, *arg);
  case 4:
    return (int)fah_ws_free(*req_h);
  default:
    return (int)FAH_ERR_INVAL;
  }
}

int FAH_FC_GLOBAL(ihsseo, IHSSEO)(const int *cli_h, const int *req_h,
                                  const int *op, const int *retry_ms,
                                  const int *q_h) {
  if (req_h == NULL || op == NULL) {
    return (int)FAH_ERR_INVAL;
  }
  switch (*op) {
  case 0:
    return (int)fah_sse_init(*req_h, NULL, (retry_ms != NULL) ? *retry_ms : 0);
  case 1:
    if (cli_h == NULL || q_h == NULL)
      return (int)FAH_ERR_INVAL;
    return (int)fah_sse_sync_loop(*cli_h, *req_h, NULL, NULL, NULL, 0, *q_h);
  case 2:
    return (int)fah_sse_free(*req_h);
  default:
    return (int)FAH_ERR_INVAL;
  }
}

int FAH_FC_GLOBAL(iherrc, IHERRC)(const int *code, int *out_cat, int *out_retry,
                                  int *ibuf, const int *icap, int *out_len) {
  char fbuf[256];
  size_t flen = 0u;
  fah_error_code_t ec;

  if (code == NULL || out_cat == NULL || out_retry == NULL || icap == NULL ||
      out_len == NULL) {
    return (int)FAH_ERR_INVAL;
  }
  ec = (fah_error_code_t)(*code);
  *out_cat = (int)fah_error_category_of(ec);
  *out_retry = fah_error_is_retryable(ec);
  FAH_IGNORE_RC(fah_error_format(ec, NULL, fbuf, sizeof(fbuf), &flen));
  return (int)fah_cbytes_to_icodes(fbuf, strlen(fbuf), ibuf, *icap, out_len);
}

/* --- Group 2: FORTRAN IV & 66 EXTERNAL Callback Entry Points --- */

void FAH_FC_GLOBAL(fhc4st,
                   FHC4ST)(const int *req_h, fah_f77_on_chunk_cb on_chunk,
                           const int *chunk_uid, fah_f77_on_progress_cb on_prog,
                           const int *prog_uid, int *out_err) {
  *out_err = (int)fah_request_set_callbacks(*req_h, on_chunk, *chunk_uid, NULL,
                                            0, 0u, on_prog, *prog_uid, 0);
}

void FAH_FC_GLOBAL(fhc4up, FHC4UP)(const int *req_h,
                                   fah_f77_read_chunk_cb read_chunk,
                                   const int *read_uid, const int *expected_len,
                                   int *out_err) {
  size_t exp_sz = (*expected_len > 0) ? (size_t)(*expected_len) : 0u;
  *out_err = (int)fah_request_set_callbacks(*req_h, NULL, 0, read_chunk,
                                            *read_uid, exp_sz, NULL, 0, 0);
}

void FAH_FC_GLOBAL(fhc4ws, FHC4WS)(const int *cli_h, const int *req_h,
                                   fah_f77_ws_on_msg_cb on_msg,
                                   fah_f77_ws_on_err_cb on_err,
                                   fah_f77_ws_on_close_cb on_close,
                                   const int *uid, int *out_err) {
  *out_err =
      (int)fah_ws_sync_loop(*cli_h, *req_h, on_msg, on_err, on_close, *uid, 0);
}

void FAH_FC_GLOBAL(fhc4ss, FHC4SS)(const int *cli_h, const int *req_h,
                                   fah_f77_sse_on_evt_cb on_evt,
                                   fah_f77_sse_on_err_cb on_err,
                                   fah_f77_sse_on_close_cb on_close,
                                   const int *uid, int *out_err) {
  *out_err =
      (int)fah_sse_sync_loop(*cli_h, *req_h, on_evt, on_err, on_close, *uid, 0);
}

/* --- Group 3: FORTRAN 77, Fortran 90 & Fortran 95 CHARACTER*(*) Entries --- */

void FAH_FC_GLOBAL(fh7cfg, FH7CFG)(const int *cfg_h, const int *op,
                                   const int *a1, const int *a2, const int *a3,
                                   const int *a4, int *out_err) {
  switch (*op) {
  case 1:
    *out_err = (int)fah_config_set_timeouts(*cfg_h, (long)(*a1), (long)(*a2),
                                            (long)(*a3), (long)(*a4));
    break;
  case 2:
    *out_err = (int)fah_config_set_tls_flags(*cfg_h, *a1, *a2, *a3, *a4);
    break;
  case 3:
    *out_err = (int)fah_config_set_versions_and_retry(
        *cfg_h, (unsigned int)(*a1), (unsigned int)(*a2), *a3, *a4);
    break;
  case 4:
    *out_err = (int)fah_config_set_modality_and_pool(
        *cfg_h, *a1, (*a2 > 0) ? (size_t)(*a2) : 0u,
        (*a3 > 0) ? (size_t)(*a3) : 0u, *a4, 30000L, 4u);
    break;
  case 5:
    *out_err = (int)fah_config_set_cookie_jar(*cfg_h, *a1);
    break;
  default:
    *out_err = (int)FAH_ERR_INVAL;
    break;
  }
}

void FAH_FC_GLOBAL(fh7cfs, FH7CFS)(const int *hnd, const int *slot,
                                   const char *str, int *out_err,
                                   fah_charlen_t str_len) {
  char *cs = NULL;
  const char *slots[8];
  int i;
  fah_error_code_t rc = fah_fstr_to_cstr(str, (size_t)str_len, 1, &cs);
  if (rc != FAH_OK) {
    *out_err = (int)rc;
    return;
  }
  if (*slot == 9) {
    *out_err = (int)fah_client_set_base_url(*hnd, cs);
    fah_free(cs);
    return;
  }
  if (*slot < 1 || *slot > 8) {
    fah_free(cs);
    *out_err = (int)FAH_ERR_INVAL;
    return;
  }
  for (i = 0; i < 8; i++) {
    slots[i] = NULL;
  }
  slots[*slot - 1] = cs;
  *out_err =
      (int)fah_config_set_strings(*hnd, slots[0], slots[1], slots[2], slots[3],
                                  slots[4], slots[5], slots[6], slots[7]);
  fah_free(cs);
}

void FAH_FC_GLOBAL(fh7hcj, FH7HCJ)(int *io_h, const int *op, const char *s1,
                                   char *s2, int *out_n, int *out_err,
                                   fah_charlen_t s1_len, fah_charlen_t s2_len) {
  char *c1 = NULL;
  char *c2 = NULL;
  size_t sz_out = 0u;
  fah_error_code_t rc;

  switch (*op) {
  case 1:
    *out_err = (int)fah_headers_create(io_h);
    return;
  case 2:
    *out_err = (int)fah_headers_destroy(*io_h);
    if (*out_err == 0)
      *io_h = 0;
    return;
  case 6:
    rc = fah_headers_count(*io_h, &sz_out);
    *out_n = (int)sz_out;
    *out_err = (int)rc;
    return;
  case 11:
    *out_err = (int)fah_cookie_jar_create(io_h);
    return;
  case 12:
    *out_err = (int)fah_cookie_jar_destroy(*io_h);
    if (*out_err == 0)
      *io_h = 0;
    return;
  default:
    break;
  }

  rc = fah_fstr_to_cstr(s1, (size_t)s1_len, 1, &c1);
  if (rc != FAH_OK) {
    *out_err = (int)rc;
    return;
  }

  switch (*op) {
  case 3:
    rc = fah_fstr_to_cstr(s2, (size_t)s2_len, 1, &c2);
    if (rc == FAH_OK) {
      rc = fah_headers_add(*io_h, c1, c2);
      fah_free(c2);
    }
    break;
  case 4:
    rc = fah_headers_get(*io_h, c1, s2, (size_t)s2_len, &sz_out, 1);
    *out_n = (int)sz_out;
    break;
  case 5:
    rc = fah_headers_remove(*io_h, c1);
    break;
  case 13:
    rc = fah_fstr_to_cstr(s2, (size_t)s2_len, 1, &c2);
    if (rc == FAH_OK) {
      rc = fah_cookie_jar_set(*io_h, c1, c2);
      fah_free(c2);
    }
    break;
  case 14:
    rc = fah_cookie_jar_get(*io_h, c1, s2, (size_t)s2_len, &sz_out, 1);
    *out_n = (int)sz_out;
    break;
  case 15:
    rc = fah_cookie_jar_parse_set(*io_h, c1);
    break;
  case 16:
    rc = fah_cookie_jar_build_hdr(*io_h, c1, s2, (size_t)s2_len, &sz_out, 1);
    *out_n = (int)sz_out;
    break;
  default:
    rc = FAH_ERR_INVAL;
    break;
  }
  fah_free(c1);
  *out_err = (int)rc;
}

void FAH_FC_GLOBAL(fh7req, FH7REQ)(const int *req_h, const int *op,
                                   const int *iarg, const char *s1,
                                   const char *s2, const char *s3,
                                   const char *s4, int *out_err,
                                   fah_charlen_t l1, fah_charlen_t l2,
                                   fah_charlen_t l3, fah_charlen_t l4) {
  char *c1 = NULL;
  char *c2 = NULL;
  char *c3 = NULL;
  fah_error_code_t rc;

  if (*op == 7) {
    *out_err = (int)fah_request_flatten_parts(*req_h);
    return;
  }
  if (*op == 8) {
    *out_err = (int)fah_request_set_cancel(*req_h, *iarg);
    return;
  }
  if (*op == 4) {
    size_t blen = (*iarg >= 0 && (size_t)(*iarg) <= (size_t)l1)
                      ? (size_t)(*iarg)
                      : (size_t)l1;
    *out_err = (*iarg < 0) ? (int)FAH_ERR_INVALID_SLICE
                           : (int)fah_request_set_body(*req_h, s1, blen);
    return;
  }

  rc = fah_fstr_to_cstr(s1, (size_t)l1, 1, &c1);
  if (rc != FAH_OK) {
    *out_err = (int)rc;
    return;
  }

  switch (*op) {
  case 1:
    rc = fah_request_set_target(*req_h, *iarg, c1);
    break;
  case 2:
    rc = fah_fstr_to_cstr(s2, (size_t)l2, 1, &c2);
    if (rc == FAH_OK) {
      rc = fah_request_add_header(*req_h, c1, c2);
    }
    break;
  case 3:
    rc = fah_request_set_body(*req_h, c1, strlen(c1));
    break;
  case 5:
    rc = fah_fstr_to_cstr(s2, (size_t)l2, 1, &c2);
    if (rc == FAH_OK) {
      rc = fah_fstr_to_cstr(s3, (size_t)l3, 1, &c3);
    }
    if (rc == FAH_OK) {
      size_t dlen = (*iarg >= 0 && (size_t)(*iarg) <= (size_t)l4)
                        ? (size_t)(*iarg)
                        : (size_t)l4;
      rc = fah_request_add_part(*req_h, c1, c2, c3, s4, dlen);
    }
    break;
  case 6:
    rc = fah_fstr_to_cstr(s2, (size_t)l2, 1, &c2);
    if (rc == FAH_OK) {
      rc = fah_request_add_part_header(*req_h, c1, c2);
    }
    break;
  case 9:
    rc = fah_auth_bearer(*req_h, c1);
    break;
  case 10:
    rc = fah_auth_basic(*req_h, c1);
    break;
  case 11:
    rc = fah_fstr_to_cstr(s2, (size_t)l2, 1, &c2);
    if (rc == FAH_OK) {
      rc = fah_auth_basic_userpwd(*req_h, c1, c2);
    }
    break;
  default:
    rc = FAH_ERR_INVAL;
    break;
  }

  fah_free(c1);
  fah_free(c2);
  fah_free(c3);
  *out_err = (int)rc;
}

void FAH_FC_GLOBAL(fh7oa2, FH7OA2)(
    const int *req_h, const int *grant_kind, const char *ep, const char *p1,
    const char *p2, const char *p3, const char *p4, const char *p5,
    char *out_s1, int *out_l1, char *out_s2, int *out_l2, int *out_err,
    fah_charlen_t eplen, fah_charlen_t l1, fah_charlen_t l2, fah_charlen_t l3,
    fah_charlen_t l4, fah_charlen_t l5, fah_charlen_t ol1, fah_charlen_t ol2) {
  char *cep = NULL;
  char *cp1 = NULL;
  char *cp2 = NULL;
  char *cp3 = NULL;
  char *cp4 = NULL;
  char *cp5 = NULL;
  size_t sz1 = 0u;
  size_t sz2 = 0u;
  fah_error_code_t rc;

  rc = fah_fstr_to_cstr(ep, (size_t)eplen, 1, &cep);
  if (rc == FAH_OK)
    rc = fah_fstr_to_cstr(p1, (size_t)l1, 1, &cp1);
  if (rc == FAH_OK)
    rc = fah_fstr_to_cstr(p2, (size_t)l2, 1, &cp2);
  if (rc == FAH_OK)
    rc = fah_fstr_to_cstr(p3, (size_t)l3, 1, &cp3);
  if (rc == FAH_OK)
    rc = fah_fstr_to_cstr(p4, (size_t)l4, 1, &cp4);
  if (rc == FAH_OK)
    rc = fah_fstr_to_cstr(p5, (size_t)l5, 1, &cp5);

  if (rc == FAH_OK) {
    if (*grant_kind >= 1 && *grant_kind <= 9) {
      rc = fah_oauth2_grant(*req_h, (fah_oauth2_grant_kind_t)(*grant_kind), cep,
                            cp1, cp2, cp3, cp4, cp5);
    } else if (*grant_kind == 10) {
      rc = fah_oauth2_build_auth_url(cep, cp1, cp2, cp3, cp4, cp5, NULL, NULL,
                                     out_s1, (size_t)ol1, &sz1, 1);
      *out_l1 = (int)sz1;
    } else if (*grant_kind == 11) {
      rc = fah_oauth2_localhost_intercept((unsigned short)(*req_h), cep, out_s1,
                                          (size_t)ol1, &sz1, out_s2,
                                          (size_t)ol2, &sz2, 1);
      *out_l1 = (int)sz1;
      *out_l2 = (int)sz2;
    } else {
      rc = FAH_ERR_INVAL;
    }
  }

  fah_free(cep);
  fah_free(cp1);
  fah_free(cp2);
  fah_free(cp3);
  fah_free(cp4);
  fah_free(cp5);
  *out_err = (int)rc;
}

void FAH_FC_GLOBAL(fh7res, FH7RES)(const int *res_h, const int *op,
                                   const int *idx, char *s1, char *s2,
                                   int *out_n1, int *out_n2, int *out_err,
                                   fah_charlen_t l1, fah_charlen_t l2) {
  char *cs1 = NULL;
  size_t sz1 = 0u;
  size_t sz2 = 0u;
  fah_error_code_t rc;

  switch (*op) {
  case 1:
    rc = fah_response_copy_body(*res_h, s2, (size_t)l2, &sz1, 1);
    *out_n1 = (int)sz1;
    break;
  case 2:
    rc = fah_fstr_to_cstr(s1, (size_t)l1, 1, &cs1);
    if (rc == FAH_OK) {
      rc = fah_response_get_header(*res_h, cs1, s2, (size_t)l2, &sz1, 1);
      *out_n1 = (int)sz1;
      fah_free(cs1);
    }
    break;
  case 3:
    rc = fah_response_header_count(*res_h, &sz1);
    *out_n1 = (int)sz1;
    break;
  case 4:
    if (*idx < 0) {
      rc = FAH_ERR_INVAL;
    } else {
      rc = fah_response_header_at(*res_h, (size_t)(*idx), s1, (size_t)l1, &sz1,
                                  s2, (size_t)l2, &sz2, 1);
      *out_n1 = (int)sz1;
      *out_n2 = (int)sz2;
    }
    break;
  case 5:
    rc = fah_fstr_to_cstr(s1, (size_t)l1, 1, &cs1);
    if (rc == FAH_OK) {
      rc = fah_response_save_file(*res_h, cs1);
      fah_free(cs1);
    }
    break;
  default:
    rc = FAH_ERR_INVAL;
    break;
  }
  *out_err = (int)rc;
}

void FAH_FC_GLOBAL(fh7mul, FH7MUL)(const int *cli_h, int *io_h, const int *op,
                                   const int *arg, int *fut_arr, const int *cap,
                                   int *out_n1, int *out_n2, int *out_err) {
  size_t cnt = 0u;
  fah_error_code_t xfer = FAH_OK;
  fah_error_code_t rc;

  switch (*op) {
  case 1:
    rc = fah_multi_create(io_h);
    break;
  case 2:
    rc = fah_multi_add(*io_h, *arg);
    break;
  case 3:
    rc = (*cap < 0) ? FAH_ERR_INVAL
                    : fah_multi_send(*cli_h, *io_h, *arg, fut_arr,
                                     (size_t)(*cap), &cnt);
    *out_n1 = (int)cnt;
    break;
  case 4:
    rc = fah_multi_destroy(*io_h);
    if (rc == FAH_OK)
      *io_h = 0;
    break;
  case 5:
    rc = fah_future_await(*io_h, &xfer);
    *out_n1 = (int)xfer;
    break;
  case 6:
    rc = fah_future_cancel(*io_h);
    break;
  case 7:
    rc = fah_future_poll(*io_h, out_n1, out_n2);
    break;
  case 8:
    rc = fah_future_take_response(*io_h, out_n1);
    break;
  case 9:
    rc = fah_future_destroy(*io_h);
    if (rc == FAH_OK)
      *io_h = 0;
    break;
  default:
    rc = FAH_ERR_INVAL;
    break;
  }
  *out_err = (int)rc;
}

void FAH_FC_GLOBAL(fh7stm, FH7STM)(const int *cli_h, const int *loop_h,
                                   const int *req_h, const int *op,
                                   const int *iarg, const char *str,
                                   void (*cb1)(void), void (*cb2)(void),
                                   void (*cb3)(void), const int *uid,
                                   int *out_err, fah_charlen_t slen) {
  char *cs = NULL;
  fah_error_code_t rc = FAH_OK;

  if (*op == 5) {
    *out_err = (int)fah_ws_close(*req_h, *iarg);
    return;
  }
  if (*op == 6) {
    *out_err = (int)fah_ws_free(*req_h);
    return;
  }
  if (*op == 13) {
    *out_err = (int)fah_sse_free(*req_h);
    return;
  }

  rc = fah_fstr_to_cstr(str, (size_t)slen, 1, &cs);
  if (rc != FAH_OK) {
    *out_err = (int)rc;
    return;
  }

  switch (*op) {
  case 1:
    rc = fah_ws_init(*req_h, cs, *iarg);
    break;
  case 2:
    rc = fah_ws_async_connect(
        *cli_h, *loop_h, *req_h, (fah_f77_ws_on_msg_cb)cb1,
        (fah_f77_ws_on_err_cb)cb2, (fah_f77_ws_on_close_cb)cb3, *uid, 0);
    break;
  case 3:
    rc = fah_ws_send(*req_h, *iarg, cs, strlen(cs), 0);
    break;
  case 4:
    rc = fah_ws_send(*req_h, *iarg, cs, strlen(cs), 1);
    break;
  case 11:
    rc = fah_sse_init(*req_h, cs, *iarg);
    break;
  case 12:
    rc = fah_sse_async_register(
        *cli_h, *loop_h, *req_h, (fah_f77_sse_on_evt_cb)cb1,
        (fah_f77_sse_on_err_cb)cb2, (fah_f77_sse_on_close_cb)cb3, *uid, 0);
    break;
  default:
    rc = FAH_ERR_INVAL;
    break;
  }
  fah_free(cs);
  *out_err = (int)rc;
}

void FAH_FC_GLOBAL(fh7mod, FH7MOD)(const int *op, int *io_h1, int *io_h2,
                                   const int *a1, const int *a2, const int *a3,
                                   char *str, void (*cb)(void), int *out_n1,
                                   int *out_err, fah_charlen_t slen) {
  fah_error_code_t rc = FAH_OK;
  size_t sz_out = 0u;
  char *cs = NULL;

  switch (*op) {
  case 1: /* loop_create */
    rc = fah_loop_create(*a1, io_h1);
    break;
  case 2: /* loop_get_backend */
    rc = fah_loop_get_backend(*io_h1, out_n1);
    break;
  case 3: /* loop_drive (tick/run/stop/wakeup) */
    rc = fah_loop_drive(*io_h1, *a1);
    break;
  case 4: /* loop_add_timer */
    rc = fah_loop_add_timer(*io_h1, (long)(*a1), (fah_f77_loop_timer_cb)cb, *a2,
                            out_n1);
    break;
  case 5: /* loop_cancel_timer */
    rc = fah_loop_cancel_timer(*io_h1, *a1);
    break;
  case 6: /* loop_destroy */
    rc = fah_loop_destroy(*io_h1);
    if (rc == FAH_OK)
      *io_h1 = 0;
    break;
  case 7: { /* time_ms */
    abstract_http_int64_t ms = 0;
    rc = fah_time_ms(&ms);
    *out_n1 = (int)(ms & 0x7FFFFFFF);
    break;
  }
  case 10: /* mutex_op */
    rc = fah_mutex_op(io_h1, *a1);
    break;
  case 11: /* cond_op */
    rc = fah_cond_op(io_h1, *a1);
    break;
  case 12: /* pool_create */
    rc = (*a1 <= 0) ? FAH_ERR_INVAL : fah_pool_create((size_t)(*a1), io_h1);
    break;
  case 13: /* pool_submit */
    rc = fah_pool_submit(*io_h1, (fah_f77_task_cb)cb, *a1);
    break;
  case 14: /* pool_destroy */
    rc = fah_pool_destroy(*io_h1);
    if (rc == FAH_OK)
      *io_h1 = 0;
    break;
  case 15: { /* tls_op */
    int v = *a2;
    rc = fah_tls_op(io_h1, *a1, &v);
    *out_n1 = v;
    break;
  }
  case 16: /* coro_create */
    rc = fah_coro_create((*a1 > 0) ? (size_t)(*a1) : 0u, (fah_f77_task_cb)cb,
                         *a2, io_h1);
    break;
  case 17: /* coro_op */
    rc = fah_coro_op(*io_h1, *a1, out_n1);
    if (rc == FAH_OK && *a1 == 3)
      *io_h1 = 0;
    break;
  case 20: /* bus_create */
    rc = fah_bus_create(io_h1);
    break;
  case 21: /* actor_spawn */
    rc = fah_fstr_to_cstr(str, (size_t)slen, 1, &cs);
    if (rc == FAH_OK) {
      rc = fah_actor_spawn(*io_h1, cs, (fah_f77_actor_cb)cb, *a1, io_h2);
      fah_free(cs);
    }
    break;
  case 22: /* actor_send_int */
    rc = fah_actor_send_int(*io_h1, *a1, *io_h2, *a2, *a3);
    break;
  case 23: /* bus_process */
    rc = fah_bus_process(*io_h1, out_n1);
    break;
  case 24: /* actor_get_name */
    rc = fah_actor_get_name(*io_h2, str, (size_t)slen, &sz_out, 1);
    *out_n1 = (int)sz_out;
    break;
  case 25: /* actor_destroy */
    rc = fah_actor_destroy(*io_h2);
    if (rc == FAH_OK)
      *io_h2 = 0;
    break;
  case 26: /* bus_destroy */
    rc = fah_bus_destroy(*io_h1);
    if (rc == FAH_OK)
      *io_h1 = 0;
    break;
  case 30: { /* ipc_op */
    char ibuf[128];
    if (*a1 == 0) {
      rc = fah_fstr_to_cstr(str, (size_t)slen, 1, &cs);
      if (rc == FAH_OK) {
        size_t cl = strlen(cs);
        if (cl > 0u && cl <= sizeof(ibuf)) {
          rc = fah_ipc_op(0, 0, cs, cl, ibuf, sizeof(ibuf), NULL);
          if (rc == FAH_OK) {
            rc = fah_cbytes_to_fstr(ibuf, cl, str, (size_t)slen, &sz_out, 1);
            *out_n1 = (int)sz_out;
          }
        } else {
          rc = FAH_ERR_INVAL;
        }
        fah_free(cs);
      }
    } else {
      rc = fah_ipc_op(*a1, *io_h1, NULL, 0u, NULL, 0u, io_h2);
    }
    break;
  }
  case 31: /* cmp_op */
    rc = fah_cmp_op(*a1, *a2, (*a3 > 0) ? (size_t)(*a3) : 1u,
                    (*a3 > 0) ? (size_t)(*a3) : 2u, *io_h1, out_n1, io_h2);
    break;
  default:
    rc = FAH_ERR_INVAL;
    break;
  }
  *out_err = (int)rc;
}

void FAH_FC_GLOBAL(fh7err, FH7ERR)(const int *code, const char *op_name,
                                   char *out_msg, int *out_cat, int *out_retry,
                                   int *out_len, int *out_err,
                                   fah_charlen_t oplen, fah_charlen_t msglen) {
  char *cop = NULL;
  char tmp[512];
  size_t flen = 0u;
  size_t clen = 0u;
  fah_error_code_t ec = (fah_error_code_t)(*code);
  fah_error_code_t rc = fah_fstr_to_cstr(op_name, (size_t)oplen, 1, &cop);
  if (rc != FAH_OK) {
    *out_err = (int)rc;
    return;
  }
  *out_cat = (int)fah_error_category_of(ec);
  *out_retry = fah_error_is_retryable(ec);
  FAH_IGNORE_RC(fah_error_format(ec, cop, tmp, sizeof(tmp), &flen));
  fah_free(cop);
  rc = fah_cbytes_to_fstr(tmp, strlen(tmp), out_msg, (size_t)msglen, &clen, 1);
  *out_len = (int)flen;
  *out_err = (int)rc;
}
