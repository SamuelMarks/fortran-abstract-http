/**
 * @file c_abstract_http_fortran_shim.c
 * @brief Auto-generated strict ISO C89 implementation of ABI struct size
 * queries for `c-abstract-http`.
 */

/* clang-format off */
#include "c_abstract_http_fortran_shim.h"
#include <c_abstract_http/http_types.h>
/* clang-format on */

size_t fah_abi_sizeof_http_config(void) { return sizeof(struct HttpConfig); }

size_t fah_abi_sizeof_http_client(void) { return sizeof(struct HttpClient); }

size_t fah_abi_sizeof_http_headers(void) { return sizeof(struct HttpHeaders); }

size_t fah_abi_sizeof_http_cookie_jar(void) {
  return sizeof(struct HttpCookieJar);
}

size_t fah_abi_sizeof_http_request(void) { return sizeof(struct HttpRequest); }

size_t fah_abi_sizeof_http_response(void) {
  return sizeof(struct HttpResponse);
}

size_t fah_abi_sizeof_http_multi_request(void) {
  return sizeof(struct HttpMultiRequest);
}

size_t fah_abi_sizeof_http_future(void) { return sizeof(struct HttpFuture); }
