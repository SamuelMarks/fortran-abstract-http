/**
 * @file fah_f77_strings.c
 * @brief Implementation of `CHARACTER*(*)` space-trimmed and binary-safe buffer
 * marshaling routines.
 */

/* clang-format off */
#include <string.h>
#include "fah_alloc.h"
#include "fah_f77_strings.h"
/* clang-format on */

fah_error_code_t fah_fstr_to_cstr(const char *f_buf, size_t f_len,
                                  int trim_trailing_spaces, char **out_cstr) {
  size_t eff_len = f_len;
  void *raw = NULL;
  char *buf;
  fah_error_code_t rc;

  if (out_cstr == NULL) {
    return FAH_ERR_INVAL;
  }
  *out_cstr = NULL;

  if (f_len > 0u && f_buf == NULL) {
    return FAH_ERR_INVALID_SLICE;
  }

  if (trim_trailing_spaces) {
    while (eff_len > 0u && f_buf[eff_len - 1u] == ' ') {
      eff_len--;
    }
  }

  rc = fah_malloc(eff_len + 1u, &raw);
  if (rc != FAH_OK) {
    return rc;
  }
  buf = (char *)raw;
  if (eff_len > 0u) {
    memcpy(buf, f_buf, eff_len);
  }
  buf[eff_len] = '\0';
  *out_cstr = buf;
  return FAH_OK;
}

fah_error_code_t fah_cbytes_to_fstr(const void *src, size_t src_len,
                                    char *f_buf, size_t f_cap,
                                    size_t *out_actual_len, int space_pad) {
  size_t ncopy;

  if ((src_len > 0u && src == NULL) || (f_cap > 0u && f_buf == NULL)) {
    return FAH_ERR_INVALID_SLICE;
  }

  if (out_actual_len != NULL) {
    *out_actual_len = src_len;
  }

  ncopy = (src_len < f_cap) ? src_len : f_cap;
  if (ncopy > 0u) {
    memcpy(f_buf, src, ncopy);
  }
  if (space_pad && ncopy < f_cap) {
    memset(f_buf + ncopy, ' ', f_cap - ncopy);
  }

  return (src_len <= f_cap) ? FAH_OK : FAH_ERR_BUF_TOO_SMALL;
}
