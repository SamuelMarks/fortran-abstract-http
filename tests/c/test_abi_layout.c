/**
 * @file test_abi_layout.c
 * @brief Verifies byte-for-byte ABI parity between:
 *   1. Our strict ISO C89 `fah_cfi_cdesc_t` / `fah_cfi_dim_t` and the active
 *      Fortran compiler's `<ISO_Fortran_binding.h>` `CFI_cdesc_t` /
 * `CFI_dim_t`.
 *   2. `c-abstract-http` C structs and Fortran 2008 `C_SIZEOF` over
 *      `generated/c_abstract_http_bind_mod.f90`.
 */

#include <ISO_Fortran_binding.h>
#include <stddef.h>
#include <stdio.h>

#include "c_abstract_http_fortran_shim.h"
#include "fah_cfi_c89.h"

extern int fah_test_f2008_abi_sizes(void);

#define CHECK_EQ(label, a, b)                                                  \
  do {                                                                         \
    if ((size_t)(a) != (size_t)(b)) {                                          \
      fprintf(stderr, "ABI MISMATCH [%s]: %lu != %lu\n", (label),              \
              (unsigned long)(a), (unsigned long)(b));                         \
      return 1;                                                                \
    }                                                                          \
  } while (0)

int main(void) {
  CHECK_EQ("sizeof(CFI_dim_t)", sizeof(fah_cfi_dim_t), sizeof(CFI_dim_t));
  CHECK_EQ("offsetof(CFI_dim_t, lower_bound)",
           offsetof(fah_cfi_dim_t, lower_bound),
           offsetof(CFI_dim_t, lower_bound));
  CHECK_EQ("offsetof(CFI_dim_t, extent)", offsetof(fah_cfi_dim_t, extent),
           offsetof(CFI_dim_t, extent));
  CHECK_EQ("offsetof(CFI_dim_t, sm)", offsetof(fah_cfi_dim_t, sm),
           offsetof(CFI_dim_t, sm));

  CHECK_EQ("offsetof(CFI_cdesc_t, base_addr)",
           offsetof(fah_cfi_cdesc_t, base_addr),
           offsetof(CFI_cdesc_t, base_addr));
  CHECK_EQ("offsetof(CFI_cdesc_t, elem_len)",
           offsetof(fah_cfi_cdesc_t, elem_len),
           offsetof(CFI_cdesc_t, elem_len));
  CHECK_EQ("offsetof(CFI_cdesc_t, version)", offsetof(fah_cfi_cdesc_t, version),
           offsetof(CFI_cdesc_t, version));
  CHECK_EQ("offsetof(CFI_cdesc_t, rank)", offsetof(fah_cfi_cdesc_t, rank),
           offsetof(CFI_cdesc_t, rank));
  CHECK_EQ("offsetof(CFI_cdesc_t, attribute)",
           offsetof(fah_cfi_cdesc_t, attribute),
           offsetof(CFI_cdesc_t, attribute));
  CHECK_EQ("offsetof(CFI_cdesc_t, type)", offsetof(fah_cfi_cdesc_t, type),
           offsetof(CFI_cdesc_t, type));
  CHECK_EQ("offsetof(CFI_cdesc_t, dim)", offsetof(fah_cfi_cdesc_t, dim),
           offsetof(CFI_cdesc_t, dim));

  CHECK_EQ("CFI_VERSION", FAH_CFI_VERSION, CFI_VERSION);
  CHECK_EQ("CFI_MAX_RANK", FAH_CFI_MAX_RANK, CFI_MAX_RANK);
  CHECK_EQ("CFI_attribute_allocatable", FAH_CFI_ATTRIBUTE_ALLOCATABLE,
           CFI_attribute_allocatable);

  if (fah_test_f2008_abi_sizes() != 0) {
    fprintf(stderr, "Fortran 2008 C_SIZEOF ABI check failed\n");
    return 1;
  }

  printf(
      "test_abi_layout: PASS (CFI_cdesc_t + 8 BIND(C) struct sizes exact)\n");
  return 0;
}
