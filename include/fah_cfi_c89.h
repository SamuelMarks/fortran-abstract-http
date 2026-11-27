/**
 * @file fah_cfi_c89.h
 * @brief Strict ISO C89 ABI-compatible shim for ISO/IEC TS 29113 / Fortran 2018
 * `<ISO_Fortran_binding.h>` (`CFI_cdesc_t`) descriptors.
 *
 * Avoids `<stdint.h>`, flexible array members (`dim[]`), and `long long` so
 * 100% of library C translation units compile cleanly under `-std=c89
 * -pedantic`, while matching GCC/LLVM `CFI_cdesc_t` byte-for-byte (verified by
 * `tests/c/test_abi_layout.c` against `<ISO_Fortran_binding.h>`).
 */

#ifndef FAH_CFI_C89_H
#define FAH_CFI_C89_H

/* clang-format off */
#include <stddef.h>
#include "fah_error.h"
#include "fah_export.h"
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** @brief ISO Fortran 2018 C descriptor version constant (`CFI_VERSION`). */
#define FAH_CFI_VERSION 1

/** @brief Maximum array rank supported by Fortran 2018 (`CFI_MAX_RANK`). */
#define FAH_CFI_MAX_RANK 15

/** @brief Descriptor represents a `POINTER` variable. */
#define FAH_CFI_ATTRIBUTE_POINTER 0
/** @brief Descriptor represents an `ALLOCATABLE` variable. */
#define FAH_CFI_ATTRIBUTE_ALLOCATABLE 1
/** @brief Descriptor represents a non-pointer, non-allocatable object. */
#define FAH_CFI_ATTRIBUTE_OTHER 2

/** @brief `CFI_SUCCESS` return value from runtime `CFI_*` functions. */
#define FAH_CFI_SUCCESS 0

/** @brief Intrinsic type mask (`CFI_type_mask`). */
#define FAH_CFI_TYPE_MASK 0xFF
/** @brief Kind shift (`CFI_type_kind_shift`). */
#define FAH_CFI_TYPE_KIND_SHIFT 8
/** @brief Intrinsic integer category (`CFI_type_Integer`). */
#define FAH_CFI_TYPE_INTEGER 1
/** @brief Intrinsic character category (`CFI_type_Character`). */
#define FAH_CFI_TYPE_CHARACTER 5
/** @brief `CFI_type_int8_t` descriptor type code. */
#define FAH_CFI_TYPE_INT8                                                      \
  (FAH_CFI_TYPE_INTEGER + (1 << FAH_CFI_TYPE_KIND_SHIFT))
/** @brief `CFI_type_char` descriptor type code. */
#define FAH_CFI_TYPE_CHAR                                                      \
  (FAH_CFI_TYPE_CHARACTER + (1 << FAH_CFI_TYPE_KIND_SHIFT))

/** @brief C89 equivalent of `CFI_index_t` (`ptrdiff_t` from `<stddef.h>`). */
typedef ptrdiff_t fah_cfi_index_t;
/** @brief C89 equivalent of `CFI_rank_t` (`signed char` == `int8_t`). */
typedef signed char fah_cfi_rank_t;
/** @brief C89 equivalent of `CFI_attribute_t` (`signed char` == `int8_t`). */
typedef signed char fah_cfi_attribute_t;
/** @brief C89 equivalent of `CFI_type_t` (`short` == `int16_t`). */
typedef short fah_cfi_type_t;

/**
 * @brief Per-dimension bounds and byte-stride triple (`CFI_dim_t`).
 */
typedef struct fah_cfi_dim_t {
  fah_cfi_index_t lower_bound; /**< Lower subscript bound */
  fah_cfi_index_t extent;      /**< Number of elements along dimension */
  fah_cfi_index_t sm;          /**< Signed memory stride in bytes */
} fah_cfi_dim_t;

/**
 * @brief Strict ISO C89 mirror of `CFI_cdesc_t` (`dim[1]` matches `dim[]`
 * offset and struct header size identically across LP64, LLP64, and ILP32).
 */
typedef struct fah_cfi_cdesc_t {
  void *base_addr;     /**< Base address of data (`NULL` if unalloc) */
  size_t elem_len;     /**< Storage size in bytes of a single item */
  int version;         /**< Descriptor version (`FAH_CFI_VERSION`) */
  fah_cfi_rank_t rank; /**< Array rank (`0 .. FAH_CFI_MAX_RANK`) */
  fah_cfi_attribute_t attribute; /**< Pointer/allocatable/other attribute */
  fah_cfi_type_t type;           /**< Encoded intrinsic type and kind */
  fah_cfi_dim_t dim[1];          /**< Trailing dimension triples */
} fah_cfi_cdesc_t;

/**
 * @brief Storage-allocating helper macro matching `CFI_CDESC_T(rank)` for
 * rank >= 1 descriptors in C89 tests and shims.
 * @param r Compile-time positive rank (`1 .. FAH_CFI_MAX_RANK`).
 */
#define FAH_CFI_CDESC_STORAGE_T(r)                                             \
  struct {                                                                     \
    void *base_addr;                                                           \
    size_t elem_len;                                                           \
    int version;                                                               \
    fah_cfi_rank_t rank;                                                       \
    fah_cfi_attribute_t attribute;                                             \
    fah_cfi_type_t type;                                                       \
    fah_cfi_dim_t dim[(r)];                                                    \
  }

/**
 * @brief Compute total byte size and extract a contiguous view or heap-gathered
 * copy from any rank `0..15` Fortran 2018 `TYPE(*), DIMENSION(..)` descriptor.
 *
 * When the descriptor is already contiguous in memory, `*out_owned_buf` is set
 * to `NULL` and `*out_bytes` aliases `desc->base_addr` directly (zero copy).
 * When strided/non-contiguous, a contiguous temporary buffer is allocated via
 * `fah_malloc` and returned in both `*out_bytes` and `*out_owned_buf` so the
 * caller can `fah_free(*out_owned_buf)` without casting away `const`.
 *
 * @param[in] desc Pointer to Fortran 2018 `CFI_cdesc_t` descriptor.
 * @param[out] out_bytes Receives contiguous byte pointer (or `NULL` if empty).
 * @param[out] out_len Receives total payload byte count.
 * @param[out] out_owned_buf Receives heap buffer to pass to `fah_free` (`NULL`
 * when zero-copy).
 * @return `FAH_OK` on success, `FAH_ERR_INVAL` on NULL out-params,
 * `FAH_ERR_CFI_DESCRIPTOR` on malformed descriptor, or `FAH_ERR_NOMEM` on OOM.
 */
FAH_NO_DISCARD FAH_API fah_error_code_t
fah_cfi_extract_bytes(const fah_cfi_cdesc_t *desc, const void **out_bytes,
                      size_t *out_len, void **out_owned_buf);

/**
 * @brief Allocate (or reallocate) a rank-1 `INTEGER(C_INT8_T), ALLOCATABLE ::
 * buf(:)` Fortran 2018 descriptor via the Fortran runtime `CFI_allocate` /
 * `CFI_deallocate` ABI and populate it with `src_len` bytes from `src`.
 * @param[in,out] desc Rank-1 `FAH_CFI_ATTRIBUTE_ALLOCATABLE` descriptor.
 * @param[in] src Source byte payload (`NULL` allowed when `src_len == 0`).
 * @param[in] src_len Number of bytes to copy.
 * @return `FAH_OK` on success, `FAH_ERR_CFI_DESCRIPTOR` if `desc` is invalid or
 * not rank-1 allocatable, or `FAH_ERR_NOMEM` on allocation failure.
 */
FAH_NO_DISCARD FAH_API fah_error_code_t fah_cfi_populate_allocatable_bytes(
    fah_cfi_cdesc_t *desc, const void *src, size_t src_len);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* FAH_CFI_C89_H */
