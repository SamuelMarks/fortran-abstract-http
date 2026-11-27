/**
 * @file mock_loopback_server.h
 * @brief Local ephemeral `127.0.0.1:0` HTTP/1.1, OAuth2, RFC 6455 WebSocket,
 * and W3C SSE loopback test server for C89 and all 12 Fortran test suites.
 */

#ifndef FAH_MOCK_LOOPBACK_SERVER_H
#define FAH_MOCK_LOOPBACK_SERVER_H

/* clang-format off */
#include "fah_export.h"
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @brief Start a background loopback server on `127.0.0.1:0`.
 * @param[out] out_port Receives bound ephemeral TCP port (`> 0`).
 * @return `0` on success, negative code on socket/thread error.
 */
int fah_mock_server_start(int *out_port);

/**
 * @brief Stop a running loopback server started on `port`.
 * @param[in] port Bound TCP port returned by `fah_mock_server_start`.
 * @return `0` on success.
 */
int fah_mock_server_stop(int port);

/**
 * @brief Reserve an ephemeral localhost port and spawn a short-lived client
 * thread that delivers an OAuth 2.0 `GET /cb?code=auth77&state=st99` redirect
 * once `http_oauth2_localhost_intercept` begins listening.
 * @param[out] out_port Receives reserved TCP port.
 * @return `0` on success, negative on failure.
 */
int fah_mock_spawn_oauth_redirect_client(int *out_port);

/**
 * @brief FORTRAN I-66 server start (`IRC = IHMKST(IPORT)`).
 * @param[out] out_port Bound TCP port.
 * @return `0` on success.
 */
FAH_API int FAH_FC_GLOBAL(ihmkst, IHMKST)(int *out_port);

/**
 * @brief FORTRAN I-66 server stop (`IRC = IHMKSP(IPORT)`).
 * @param[in] port Bound TCP port.
 * @return `0` on success.
 */
FAH_API int FAH_FC_GLOBAL(ihmksp, IHMKSP)(const int *port);

/**
 * @brief FORTRAN I-66 URL builder into unpacked `INTEGER` (`mode == 0`) or
 * 4-char Hollerith (`mode == 1`) arrays.
 * @param[in] port Bound TCP port.
 * @param[in] route_id Route selector (`0`=/hello, `1`=/echo, `2`=/chunked,
 * `3`=ws, `4`=/sse, `5`=/empty, `6`=/redirect, `7`=/flaky, `8`=/oauth/token).
 * @param[in] mode `0` = unpacked ints, `1` = 4-char Hollerith words.
 * @param[out] out_arr Destination integer array.
 * @param[out] out_nchars Exact character count written.
 * @return `0` on success.
 */
FAH_API int FAH_FC_GLOBAL(ihmkul, IHMKUL)(const int *port, const int *route_id,
                                          const int *mode, int *out_arr,
                                          int *out_nchars);

/**
 * @brief FORTRAN 77-2023 URL builder into space-padded `CHARACTER*(*)`.
 * @param[in] port Bound TCP port.
 * @param[in] path Route path (e.g. `'/hello'` or `'ws:/ws'`).
 * @param[out] out_url Destination `CHARACTER*(*)` buffer.
 * @param[out] out_len Exact URL length before padding.
 * @param[in] plen Hidden length of `path`.
 * @param[in] ulen Hidden length of `out_url`.
 */
FAH_API void FAH_FC_GLOBAL(fhmks7, FHMKS7)(const int *port, const char *path,
                                           char *out_url, int *out_len,
                                           fah_charlen_t plen,
                                           fah_charlen_t ulen);

/**
 * @brief FORTRAN 77 helper spawning an OAuth2 localhost redirect client.
 * @param[out] out_port Reserved TCP port.
 * @param[out] out_err `0` on success.
 */
FAH_API void FAH_FC_GLOBAL(fhmkoa, FHMKOA)(int *out_port, int *out_err);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* FAH_MOCK_LOOPBACK_SERVER_H */
