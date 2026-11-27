/**
 * @file c_abstract_http_fortran_shim.h
 * @brief Auto-generated strict ISO C89 ABI struct size queries for verifying
 * `C_SIZEOF` parity between `c-abstract-http` C structs and
 * `c_abstract_http_bind_mod.f90`.
 */

#ifndef C_ABSTRACT_HTTP_FORTRAN_SHIM_H
#define C_ABSTRACT_HTTP_FORTRAN_SHIM_H

/* clang-format off */
#include <stddef.h>
#include "fah_export.h"
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @brief Return `sizeof(struct HttpConfig)` in bytes.
 * @return Byte size of `struct HttpConfig`.
 */
FAH_NO_DISCARD FAH_API size_t fah_abi_sizeof_http_config(void);

/**
 * @brief Return `sizeof(struct HttpClient)` in bytes.
 * @return Byte size of `struct HttpClient`.
 */
FAH_NO_DISCARD FAH_API size_t fah_abi_sizeof_http_client(void);

/**
 * @brief Return `sizeof(struct HttpHeaders)` in bytes.
 * @return Byte size of `struct HttpHeaders`.
 */
FAH_NO_DISCARD FAH_API size_t fah_abi_sizeof_http_headers(void);

/**
 * @brief Return `sizeof(struct HttpCookieJar)` in bytes.
 * @return Byte size of `struct HttpCookieJar`.
 */
FAH_NO_DISCARD FAH_API size_t fah_abi_sizeof_http_cookie_jar(void);

/**
 * @brief Return `sizeof(struct HttpRequest)` in bytes.
 * @return Byte size of `struct HttpRequest`.
 */
FAH_NO_DISCARD FAH_API size_t fah_abi_sizeof_http_request(void);

/**
 * @brief Return `sizeof(struct HttpResponse)` in bytes.
 * @return Byte size of `struct HttpResponse`.
 */
FAH_NO_DISCARD FAH_API size_t fah_abi_sizeof_http_response(void);

/**
 * @brief Return `sizeof(struct HttpMultiRequest)` in bytes.
 * @return Byte size of `struct HttpMultiRequest`.
 */
FAH_NO_DISCARD FAH_API size_t fah_abi_sizeof_http_multi_request(void);

/**
 * @brief Return `sizeof(struct HttpFuture)` in bytes.
 * @return Byte size of `struct HttpFuture`.
 */
FAH_NO_DISCARD FAH_API size_t fah_abi_sizeof_http_future(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* C_ABSTRACT_HTTP_FORTRAN_SHIM_H */
