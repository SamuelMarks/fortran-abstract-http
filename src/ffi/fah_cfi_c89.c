/**
 * @file fah_cfi_c89.c
 * @brief Strict ISO C89 implementation of Fortran 2018 TS 29113 `CFI_cdesc_t`
 * zero-copy & strided array extraction and `ALLOCATABLE` buffer population.
 */

/* clang-format off */
#include <string.h>
#include "fah_alloc.h"
#include "fah_cfi_c89.h"
/* clang-format on */

/**
 * @brief External Fortran 2018 runtime `CFI_allocate` symbol provided by
 * `libgfortran` / `libflang_rt`.
 * @param[in,out] dv Descriptor of an unallocated `ALLOCATABLE` or `POINTER`.
 * @param[in] lower_bounds Array of lower bounds (`rank` elements).
 * @param[in] upper_bounds Array of upper bounds (`rank` elements).
 * @param[in] elem_len Element byte length (for deferred character length).
 * @return `0` (`FAH_CFI_SUCCESS`) on success, non-zero CFI error otherwise.
 */
extern int CFI_allocate(fah_cfi_cdesc_t *dv,
                        const fah_cfi_index_t lower_bounds[],
                        const fah_cfi_index_t upper_bounds[], size_t elem_len);

/**
 * @brief External Fortran 2018 runtime `CFI_deallocate` symbol provided by
 * `libgfortran` / `libflang_rt`.
 * @param[in,out] dv Descriptor of an allocated `ALLOCATABLE` or `POINTER`.
 * @return `0` (`FAH_CFI_SUCCESS`) on success, non-zero CFI error otherwise.
 */
extern int CFI_deallocate(fah_cfi_cdesc_t *dv);

fah_error_code_t fah_cfi_extract_bytes(const fah_cfi_cdesc_t *desc,
                                       const void **out_bytes, size_t *out_len,
                                       void **out_owned_buf) {
  size_t elem_count = 1u;
  size_t total_bytes;
  int is_contig = 1;
  ptrdiff_t expected_stride;
  int r;
  int rank;

  if (out_bytes == NULL || out_len == NULL || out_owned_buf == NULL) {
    return FAH_ERR_INVAL;
  }
  *out_bytes = NULL;
  *out_len = 0u;
  *out_owned_buf = NULL;

  if (desc == NULL || desc->version != FAH_CFI_VERSION || desc->rank < 0 ||
      desc->rank > FAH_CFI_MAX_RANK) {
    return FAH_ERR_CFI_DESCRIPTOR;
  }

  rank = (int)desc->rank;
  if (desc->base_addr == NULL) {
    return (rank == 0) ? FAH_ERR_CFI_DESCRIPTOR : FAH_OK;
  }

  expected_stride = (ptrdiff_t)desc->elem_len;
  for (r = 0; r < rank; r++) {
    ptrdiff_t ext = desc->dim[r].extent;
    if (ext <= 0) {
      return FAH_OK;
    }
    if (desc->dim[r].sm != expected_stride) {
      is_contig = 0;
    }
    if ((size_t)ext > ((size_t)-1) / elem_count) {
      return FAH_ERR_NOMEM;
    }
    elem_count *= (size_t)ext;
    expected_stride *= ext;
  }

  if (desc->elem_len == 0u) {
    return FAH_OK;
  }
  if (elem_count > ((size_t)-1) / desc->elem_len) {
    return FAH_ERR_NOMEM;
  }
  total_bytes = elem_count * desc->elem_len;

  if (is_contig) {
    *out_bytes = desc->base_addr;
    *out_len = total_bytes;
    return FAH_OK;
  }

  /* Gather non-contiguous / strided array slice in column-major Fortran order
   */
  {
    void *raw_buf = NULL;
    unsigned char *dst;
    const unsigned char *base = (const unsigned char *)desc->base_addr;
    ptrdiff_t coord[FAH_CFI_MAX_RANK];
    size_t idx;
    fah_error_code_t rc;

    rc = fah_malloc(total_bytes, &raw_buf);
    if (rc != FAH_OK) {
      return rc;
    }
    dst = (unsigned char *)raw_buf;
    for (r = 0; r < rank; r++) {
      coord[r] = 0;
    }

    for (idx = 0u; idx < elem_count; idx++) {
      ptrdiff_t byte_off = 0;
      for (r = 0; r < rank; r++) {
        byte_off += coord[r] * desc->dim[r].sm;
      }
      memcpy(dst + (idx * desc->elem_len), base + byte_off, desc->elem_len);

      for (r = 0; r < rank; r++) {
        coord[r]++;
        if (coord[r] < desc->dim[r].extent) {
          break;
        }
        coord[r] = 0;
      }
    }

    *out_bytes = raw_buf;
    *out_len = total_bytes;
    *out_owned_buf = raw_buf;
  }
  return FAH_OK;
}

fah_error_code_t fah_cfi_populate_allocatable_bytes(fah_cfi_cdesc_t *desc,
                                                    const void *src,
                                                    size_t src_len) {
  fah_cfi_index_t lb[1];
  fah_cfi_index_t ub[1];
  int cfi_rc;

  if (desc == NULL || desc->version != FAH_CFI_VERSION || desc->rank != 1 ||
      desc->attribute != FAH_CFI_ATTRIBUTE_ALLOCATABLE ||
      desc->elem_len != 1u) {
    return FAH_ERR_CFI_DESCRIPTOR;
  }
  if (src_len > 0u && src == NULL) {
    return FAH_ERR_INVALID_SLICE;
  }

  if (desc->base_addr != NULL) {
    FAH_IGNORE_RC(CFI_deallocate(desc));
  }

  lb[0] = 1;
  ub[0] = (src_len == 0u) ? 0 : (fah_cfi_index_t)src_len;
  cfi_rc = CFI_allocate(desc, lb, ub, 1u);
  if (cfi_rc != FAH_CFI_SUCCESS) {
    return FAH_ERR_NOMEM;
  }

  if (src_len > 0u && desc->base_addr != NULL) {
    memcpy(desc->base_addr, src, src_len);
  }
  return FAH_OK;
}
