/**
 * @file fah_f77_strings.h
 * @brief Space-padded `CHARACTER*(*)` and explicit-length binary buffer
 * marshaling helpers for FORTRAN 77, Fortran 90, and Fortran 95.
 */

#ifndef FAH_F77_STRINGS_H
#define FAH_F77_STRINGS_H

/* clang-format off */
#include <stddef.h>
#include "fah_error.h"
#include "fah_export.h"
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @brief Convert a non-NUL-terminated Fortran `CHARACTER` buffer into a heap
 * NUL-terminated C string, optionally stripping trailing ASCII spaces (`' '`).
 * @param[in] f_buf Fortran character buffer (`NULL` allowed when `f_len == 0`).
 * @param[in] f_len Byte length of `f_buf`.
 * @param[in] trim_trailing_spaces Non-zero to strip trailing `' '` characters.
 * @param[out] out_cstr Receives newly allocated NUL-terminated C string.
 * @return `FAH_OK` on success, `FAH_ERR_INVAL` if `out_cstr == NULL`,
 * `FAH_ERR_INVALID_SLICE` if `f_buf == NULL` with `f_len > 0`, or
 * `FAH_ERR_NOMEM` on OOM.
 */
FAH_NO_DISCARD FAH_API fah_error_code_t fah_fstr_to_cstr(
    const char *f_buf, size_t f_len, int trim_trailing_spaces, char **out_cstr);

/**
 * @brief Copy `src_len` bytes from C into a Fortran `CHARACTER` or byte buffer
 * of capacity `f_cap`, optionally padding trailing unused capacity with `' '`.
 * @param[in] src Source C byte/string buffer (`NULL` allowed when
 * `src_len == 0`).
 * @param[in] src_len Exact byte length to copy.
 * @param[out] f_buf Destination Fortran buffer (`NULL` allowed when
 * `f_cap == 0`).
 * @param[in] f_cap Byte capacity of `f_buf`.
 * @param[out] out_actual_len Optional pointer receiving full `src_len`.
 * @param[in] space_pad Non-zero to pad `f_buf[src_len .. f_cap-1]` with `' '`.
 * @return `FAH_OK` if `src_len <= f_cap`, `FAH_ERR_INVALID_SLICE` on NULL slice
 * with positive length, or `FAH_ERR_BUF_TOO_SMALL` when truncated.
 */
FAH_NO_DISCARD FAH_API fah_error_code_t
fah_cbytes_to_fstr(const void *src, size_t src_len, char *f_buf, size_t f_cap,
                   size_t *out_actual_len, int space_pad);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* FAH_F77_STRINGS_H */
