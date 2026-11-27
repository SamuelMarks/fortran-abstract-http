/**
 * @file fah_hollerith.c
 * @brief Implementation of unpacked `INTEGER` character-code arrays and
 * 4-character-per-word Hollerith codecs for FORTRAN I through FORTRAN 66.
 */

/* clang-format off */
#include <string.h>
#include "fah_alloc.h"
#include "fah_hollerith.h"
/* clang-format on */

fah_error_code_t fah_icodes_to_cstr(const int *icodes, int nlen,
                                    char **out_cstr) {
  void *raw = NULL;
  char *buf;
  int i;
  fah_error_code_t rc;

  if (out_cstr == NULL) {
    return FAH_ERR_INVAL;
  }
  *out_cstr = NULL;

  if (nlen < 0 || (nlen > 0 && icodes == NULL)) {
    return FAH_ERR_INVALID_SLICE;
  }

  for (i = 0; i < nlen; i++) {
    if (icodes[i] < 0 || icodes[i] > 255) {
      return FAH_ERR_HOLLERITH_DECODE;
    }
  }

  rc = fah_malloc(((size_t)nlen) + 1u, &raw);
  if (rc != FAH_OK) {
    return rc;
  }
  buf = (char *)raw;
  for (i = 0; i < nlen; i++) {
    buf[i] = (char)((unsigned char)icodes[i]);
  }
  buf[nlen] = '\0';
  *out_cstr = buf;
  return FAH_OK;
}

fah_error_code_t fah_cbytes_to_icodes(const void *src, size_t src_len,
                                      int *out_icodes, int cap,
                                      int *out_actual_len) {
  const unsigned char *bytes = (const unsigned char *)src;
  size_t ncopy;
  size_t i;

  if (cap < 0 || (src_len > 0u && src == NULL) ||
      (cap > 0 && out_icodes == NULL)) {
    return FAH_ERR_INVALID_SLICE;
  }

  if (out_actual_len != NULL) {
    *out_actual_len = (int)src_len;
  }

  ncopy = (src_len < (size_t)cap) ? src_len : (size_t)cap;
  for (i = 0u; i < ncopy; i++) {
    out_icodes[i] = (int)bytes[i];
  }

  return (src_len <= (size_t)cap) ? FAH_OK : FAH_ERR_BUF_TOO_SMALL;
}

fah_error_code_t fah_hollerith4_to_cstr(const int *hwords, int nchars,
                                        char **out_cstr) {
  void *raw = NULL;
  char *buf;
  const unsigned char *word_bytes;
  int i;
  fah_error_code_t rc;

  if (out_cstr == NULL) {
    return FAH_ERR_INVAL;
  }
  *out_cstr = NULL;

  if (nchars < 0 || (nchars > 0 && hwords == NULL)) {
    return FAH_ERR_INVALID_SLICE;
  }

  rc = fah_malloc(((size_t)nchars) + 1u, &raw);
  if (rc != FAH_OK) {
    return rc;
  }
  buf = (char *)raw;
  /*
   * Fortran compilers store Hollerith constants (`4HGET `) in consecutive
   * memory bytes of the 32-bit `INTEGER` storage unit on both little-endian and
   * big-endian hosts. Reading the raw storage bytes preserves exact character
   * order across architectures.
   */
  word_bytes = (const unsigned char *)hwords;
  for (i = 0; i < nchars; i++) {
    buf[i] = (char)word_bytes[i];
  }
  buf[nchars] = '\0';
  *out_cstr = buf;
  return FAH_OK;
}

fah_error_code_t fah_cbytes_to_hollerith4(const void *src, size_t src_len,
                                          int *out_hwords, int word_cap,
                                          int *out_actual_chars) {
  const unsigned char *bytes = (const unsigned char *)src;
  unsigned char *dst_bytes;
  size_t words_needed;
  size_t words_to_write;
  size_t byte_budget;
  size_t i;

  if (word_cap < 0 || (src_len > 0u && src == NULL) ||
      (word_cap > 0 && out_hwords == NULL)) {
    return FAH_ERR_INVALID_SLICE;
  }

  if (out_actual_chars != NULL) {
    *out_actual_chars = (int)src_len;
  }

  words_needed = (src_len == 0u) ? 0u : ((src_len + 3u) / 4u);
  words_to_write =
      (words_needed < (size_t)word_cap) ? words_needed : (size_t)word_cap;
  byte_budget = words_to_write * 4u;

  dst_bytes = (unsigned char *)out_hwords;
  for (i = 0u; i < byte_budget; i++) {
    dst_bytes[i] = (i < src_len) ? bytes[i] : (unsigned char)' ';
  }

  return (words_needed <= (size_t)word_cap) ? FAH_OK : FAH_ERR_BUF_TOO_SMALL;
}
