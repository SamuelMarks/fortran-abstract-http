/**
 * @file fah_error.h
 * @brief Canonical unified error enumeration, categories, and structured
 * formatting routines for fortran-abstract-http.
 */

#ifndef FAH_ERROR_H
#define FAH_ERROR_H

/* clang-format off */
#include <stddef.h>
#include "c_abstract_http/http_types.h"
#include "fah_export.h"
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @brief Single unified error code enumeration spanning all 19
 * `c_abstract_http_error_t` codes and all Fortran FFI validation codes.
 */
typedef enum fah_error_code {
  FAH_OK = 0,                   /**< Operation succeeded */
  FAH_ERR_INVAL = -1,           /**< Invalid argument */
  FAH_ERR_NOMEM = -2,           /**< Out of heap memory */
  FAH_ERR_IO = -3,              /**< Network or socket I/O error */
  FAH_ERR_NOTSUP = -4,          /**< Backend or operation not supported */
  FAH_ERR_TIMEOUT = -5,         /**< Connect, read, or write timed out */
  FAH_ERR_SYSCALL = -6,         /**< OS system call failed */
  FAH_ERR_WS_FRAMING = -1001,   /**< RFC 6455 WebSocket frame violation */
  FAH_ERR_WS_HANDSHAKE = -1002, /**< WebSocket 101 handshake rejected */
  FAH_ERR_SSE_PARSE = -1003,    /**< W3C SSE stream parse error */
  FAH_ERR_SSE_OOM = -1004,      /**< SSE line buffer limit exceeded */
  FAH_ERR_WS_CLOSED = -1005,    /**< WebSocket peer closed connection */
  FAH_ERR_ACCES = EACCES,       /**< Permission or TLS verification denied */
  FAH_ERR_EXIST = EEXIST,       /**< Resource or entry already exists */
  FAH_ERR_LOOP = ELOOP,         /**< Maximum HTTP redirect hops exceeded */
  FAH_ERR_MSGSIZE = 90,         /**< Payload or frame exceeds limit */
  FAH_ERR_CONNREFUSED = ECONNREFUSED, /**< Remote TCP/QUIC connection refused */
  FAH_ERR_HOSTUNREACH = EHOSTUNREACH, /**< Remote host or route unreachable */
  FAH_ERR_CANCELED = ECANCELED,       /**< Transfer canceled via cancel_token */
  FAH_ERR_NULL_HANDLE = -2001,       /**< Zero or uninitialized handle passed */
  FAH_ERR_BAD_HANDLE_TAG = -2002,    /**< Handle tag mismatches expected type */
  FAH_ERR_STALE_HANDLE = -2003,      /**< Freed or stale handle generation */
  FAH_ERR_HANDLE_TABLE_FULL = -2004, /**< 31-bit handle table capacity full */
  FAH_ERR_BUF_TOO_SMALL = -2005,     /**< Caller output buffer was truncated */
  FAH_ERR_INVALID_SLICE = -2006,     /**< Negative length or NULL buffer ptr */
  FAH_ERR_CALLBACK_FAULT = -2007,    /**< User Fortran callback returned err */
  FAH_ERR_CFI_DESCRIPTOR = -2008,    /**< Invalid TS 29113 CFI_cdesc_t */
  FAH_ERR_HOLLERITH_DECODE = -2009   /**< Invalid Hollerith/integer char deck */
} fah_error_code_t;

/**
 * @brief High-level semantic classification of `fah_error_code_t` variants.
 */
typedef enum fah_error_category {
  FAH_CAT_NONE = 0,       /**< No error (`FAH_OK`) */
  FAH_CAT_ARGUMENT = 1,   /**< Invalid argument, slice, or Hollerith input */
  FAH_CAT_MEMORY = 2,     /**< Heap or buffer capacity exhaustion */
  FAH_CAT_NETWORK_IO = 3, /**< Network, syscall, or routing failure */
  FAH_CAT_TIMEOUT = 4,    /**< Operation timeout */
  FAH_CAT_TLS_AUTH = 5,   /**< TLS pinning, CA trust, or permission error */
  FAH_CAT_PROTOCOL = 6,   /**< Redirect loop, WS framing/handshake, or SSE */
  FAH_CAT_FFI_HANDLE = 7, /**< Handle tag, null, stale, CFI, or callback err */
  FAH_CAT_CANCELED = 8    /**< Cooperative cancellation */
} fah_error_category_t;

/**
 * @brief Convert a `c_abstract_http_error_t` code into the unified
 * `fah_error_code_t` enum (`derive_more::From` equivalent).
 * @param[in] c_err Upstream `c-abstract-http` status code.
 * @return Corresponding `fah_error_code_t` value.
 */
FAH_API fah_error_code_t fah_error_from_c(c_abstract_http_error_t c_err);

/**
 * @brief Classify an `fah_error_code_t` into its `fah_error_category_t`.
 * @param[in] code Unified error code.
 * @return Semantic `fah_error_category_t` bucket.
 */
FAH_API fah_error_category_t fah_error_category_of(fah_error_code_t code);

/**
 * @brief Determine whether a failed operation with `code` is transient and
 * eligible for retry (`TIMEOUT`, `IO`, `CONNREFUSED`, `HOSTUNREACH`).
 * @param[in] code Unified error code.
 * @return `1` if transient/retryable, `0` otherwise.
 */
FAH_API int fah_error_is_retryable(fah_error_code_t code);

/**
 * @brief Return the static symbolic constant name of `code` (e.g., `"FAH_OK"`).
 * @param[in] code Unified error code.
 * @return Non-NULL static NUL-terminated symbolic name string.
 */
FAH_API const char *fah_error_name(fah_error_code_t code);

/**
 * @brief Return a static human-readable description of `code`.
 * @param[in] code Unified error code.
 * @return Non-NULL static NUL-terminated diagnostic string.
 */
FAH_API const char *fah_error_message(fah_error_code_t code);

/**
 * @brief Format a structured diagnostic line (`[SYMBOL] op: message`) into
 * `out_buf` (`derive_more::Display` equivalent).
 * @param[in] code Unified error code.
 * @param[in] op_name Optional originating operation name (may be `NULL`).
 * @param[out] out_buf Destination character buffer.
 * @param[in] buf_cap Byte capacity of `out_buf` (must be `>= 1`).
 * @param[out] out_len Optional pointer receiving full formatted length
 * excluding NUL.
 * @return `FAH_OK` on full write, `FAH_ERR_INVAL` if `out_buf == NULL` or
 * `buf_cap == 0`, or `FAH_ERR_BUF_TOO_SMALL` if truncated.
 */
FAH_NO_DISCARD FAH_API fah_error_code_t fah_error_format(fah_error_code_t code,
                                                         const char *op_name,
                                                         char *out_buf,
                                                         size_t buf_cap,
                                                         size_t *out_len);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* FAH_ERROR_H */
