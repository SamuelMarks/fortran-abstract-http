/**
 * @file fah_hollerith.h
 * @brief Endian-aware 4-character Hollerith (`4HGET `) and unpacked integer
 * character-code codecs for FORTRAN I, II, III, IV, and 66.
 */

#ifndef FAH_HOLLERITH_H
#define FAH_HOLLERITH_H

/* clang-format off */
#include <stddef.h>
#include "fah_error.h"
#include "fah_export.h"
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @brief Convert an unpacked FORTRAN `INTEGER` character-code array (`1` char
 * per `INTEGER`, values `0..255`) into a heap-allocated NUL-terminated C
 * string.
 * @param[in] icodes Array of `nlen` integer byte/ASCII codes (`NULL` allowed
 * when `nlen == 0`).
 * @param[in] nlen Number of characters (`>= 0`).
 * @param[out] out_cstr Receives newly allocated NUL-terminated C string.
 * @return `FAH_OK` on success, `FAH_ERR_INVALID_SLICE` on negative `nlen` or
 * `NULL` buffer with `nlen > 0`, `FAH_ERR_INVAL` if `out_cstr == NULL`,
 * `FAH_ERR_HOLLERITH_DECODE` if an element is outside `0..255`, or
 * `FAH_ERR_NOMEM` on OOM.
 */
FAH_NO_DISCARD FAH_API fah_error_code_t fah_icodes_to_cstr(const int *icodes,
                                                           int nlen,
                                                           char **out_cstr);

/**
 * @brief Copy raw C bytes into an unpacked FORTRAN `INTEGER` character-code
 * array (`0..255` per element).
 * @param[in] src Source byte buffer (`NULL` allowed when `src_len == 0`).
 * @param[in] src_len Byte length of `src`.
 * @param[out] out_icodes Destination `INTEGER` array (`NULL` allowed when
 * `cap == 0`).
 * @param[in] cap Element capacity of `out_icodes` (`>= 0`).
 * @param[out] out_actual_len Optional pointer receiving exact `src_len` as
 * `int`.
 * @return `FAH_OK` on success, `FAH_ERR_INVALID_SLICE` on invalid slice/cap, or
 * `FAH_ERR_BUF_TOO_SMALL` if `src_len > (size_t)cap` (writes `cap` elements).
 */
FAH_NO_DISCARD FAH_API fah_error_code_t
fah_cbytes_to_icodes(const void *src, size_t src_len, int *out_icodes, int cap,
                     int *out_actual_len);

/**
 * @brief Decode a packed 4-characters-per-32-bit-`INTEGER` Hollerith array
 * (such as `DATA IMETH /4HGET /`) into a heap NUL-terminated C string in host
 * memory order.
 * @param[in] hwords Packed 32-bit Hollerith integer array.
 * @param[in] nchars Exact character count to extract (`>= 0`).
 * @param[out] out_cstr Receives newly allocated NUL-terminated C string.
 * @return `FAH_OK` on success, `FAH_ERR_INVALID_SLICE` if `nchars < 0` or
 * `hwords == NULL` when `nchars > 0`, `FAH_ERR_INVAL` if `out_cstr == NULL`, or
 * `FAH_ERR_NOMEM` on OOM.
 */
FAH_NO_DISCARD FAH_API fah_error_code_t
fah_hollerith4_to_cstr(const int *hwords, int nchars, char **out_cstr);

/**
 * @brief Pack raw C bytes into 4-characters-per-32-bit-`INTEGER` Hollerith
 * words, padding any partial final 4-byte word with ASCII spaces (`0x20`).
 * @param[in] src Source byte buffer (`NULL` allowed when `src_len == 0`).
 * @param[in] src_len Number of bytes in `src`.
 * @param[out] out_hwords Destination 32-bit `INTEGER` word array.
 * @param[in] word_cap Capacity of `out_hwords` in 32-bit `INTEGER` words.
 * @param[out] out_actual_chars Optional pointer receiving `src_len` as `int`.
 * @return `FAH_OK` on success, `FAH_ERR_INVALID_SLICE` on invalid arguments, or
 * `FAH_ERR_BUF_TOO_SMALL` if `(src_len + 3) / 4 > (size_t)word_cap`.
 */
FAH_NO_DISCARD FAH_API fah_error_code_t
fah_cbytes_to_hollerith4(const void *src, size_t src_len, int *out_hwords,
                         int word_cap, int *out_actual_chars);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* FAH_HOLLERITH_H */
