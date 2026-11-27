/**
 * @file fah_alloc.h
 * @brief Strict ISO C89 heap allocation wrappers with deterministic OOM fault
 * injection for 100% branch coverage.
 */

#ifndef FAH_ALLOC_H
#define FAH_ALLOC_H

/* clang-format off */
#include <stddef.h>
#include "fah_error.h"
#include "fah_export.h"
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @brief Allocate `size` uninitialized bytes on the heap (minimum 1 byte).
 * @param[in] size Number of bytes requested.
 * @param[out] out_ptr Receives the allocated pointer on `FAH_OK`, or `NULL` on
 * failure.
 * @return `FAH_OK` on success, `FAH_ERR_INVAL` if `out_ptr` is `NULL`, or
 * `FAH_ERR_NOMEM` on allocation failure.
 */
FAH_NO_DISCARD FAH_API fah_error_code_t fah_malloc(size_t size, void **out_ptr);

/**
 * @brief Allocate `count * elem_size` zero-initialized bytes on the heap.
 * @param[in] count Number of elements.
 * @param[in] elem_size Byte size of each element.
 * @param[out] out_ptr Receives the zeroed pointer on `FAH_OK`, or `NULL` on
 * failure.
 * @return `FAH_OK` on success, `FAH_ERR_INVAL` if `out_ptr` is `NULL`, or
 * `FAH_ERR_NOMEM` on overflow or allocation failure.
 */
FAH_NO_DISCARD FAH_API fah_error_code_t fah_calloc(size_t count,
                                                   size_t elem_size,
                                                   void **out_ptr);

/**
 * @brief Resize an existing heap block `ptr` to `new_size` bytes.
 * @param[in,out] ptr Existing heap pointer (may be `NULL`).
 * @param[in] new_size Requested byte size (minimum 1 byte).
 * @param[out] out_ptr Receives resized pointer (`ptr` is untouched if
 * `FAH_ERR_NOMEM` is returned).
 * @return `FAH_OK` on success, `FAH_ERR_INVAL` if `out_ptr` is `NULL`, or
 * `FAH_ERR_NOMEM` on allocation failure.
 */
FAH_NO_DISCARD FAH_API fah_error_code_t fah_realloc(void *ptr, size_t new_size,
                                                    void **out_ptr);

/**
 * @brief Duplicate a NUL-terminated C string onto the heap.
 * @param[in] src Input NUL-terminated string.
 * @param[out] out_str Receives newly allocated NUL-terminated copy.
 * @return `FAH_OK` on success, `FAH_ERR_INVAL` if `src` or `out_str` is `NULL`,
 * or `FAH_ERR_NOMEM` on allocation failure.
 */
FAH_NO_DISCARD FAH_API fah_error_code_t fah_strdup(const char *src,
                                                   char **out_str);

/**
 * @brief Release a heap pointer previously allocated by `fah_*` or `stdlib`.
 * @param[in,out] ptr Heap block to release (`NULL`-safe).
 */
FAH_API void fah_free(void *ptr);

#if defined(FAH_TEST_OOM)
/**
 * @brief Configure the OOM injector to fail on the `step`-th allocation call
 * (`1` = fail immediately on next allocation, `<= 0` = disable failure).
 * @param[in] step 1-based allocation countdown trigger.
 */
FAH_API void fah_oom_set_fail_after(int step);

/**
 * @brief Reset OOM fault injection state and total allocation counter.
 */
FAH_API void fah_oom_reset(void);

/**
 * @brief Query the number of allocation attempts made since `fah_oom_reset()`.
 * @return Total allocation attempts observed.
 */
FAH_API size_t fah_oom_get_alloc_count(void);
#endif /* FAH_TEST_OOM */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* FAH_ALLOC_H */
