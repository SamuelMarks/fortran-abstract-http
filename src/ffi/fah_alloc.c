/**
 * @file fah_alloc.c
 * @brief Strict ISO C89 heap allocator wrappers and deterministic OOM fault
 * injector implementation.
 */

/* clang-format off */
#include <stdlib.h>
#include <string.h>
#include "fah_alloc.h"
/* clang-format on */

#if defined(FAH_TEST_OOM)
/** @brief Remaining allocations before simulated OOM (`<= 0` = disabled). */
static int g_fah_oom_countdown = 0;

/** @brief Total allocation attempts recorded since `fah_oom_reset()`. */
static size_t g_fah_alloc_count = 0;

/**
 * @brief Check whether the current allocation attempt should fail under OOM
 * fault injection.
 * @return `1` to simulate `ENOMEM`, `0` to proceed with real allocation.
 */
static int fah_should_fail_alloc(void) {
  g_fah_alloc_count++;
  if (g_fah_oom_countdown > 0) {
    g_fah_oom_countdown--;
    if (g_fah_oom_countdown == 0) {
      return 1;
    }
  }
  return 0;
}

void fah_oom_set_fail_after(int step) { g_fah_oom_countdown = step; }

void fah_oom_reset(void) {
  g_fah_oom_countdown = 0;
  g_fah_alloc_count = 0;
}

size_t fah_oom_get_alloc_count(void) { return g_fah_alloc_count; }
#endif /* FAH_TEST_OOM */

fah_error_code_t fah_malloc(size_t size, void **out_ptr) {
  size_t eff_size;
  void *p;

  if (out_ptr == NULL) {
    return FAH_ERR_INVAL;
  }
  *out_ptr = NULL;

  eff_size = (size == 0) ? 1u : size;
#if defined(FAH_TEST_OOM)
  p = fah_should_fail_alloc() ? NULL : malloc(eff_size);
#else
  p = malloc(eff_size);
#endif
  if (p == NULL) {
    return FAH_ERR_NOMEM;
  }
  *out_ptr = p;
  return FAH_OK;
}

fah_error_code_t fah_calloc(size_t count, size_t elem_size, void **out_ptr) {
  size_t eff_count;
  size_t eff_elem;
  void *p;

  if (out_ptr == NULL) {
    return FAH_ERR_INVAL;
  }
  *out_ptr = NULL;

  if (elem_size != 0 && count > ((size_t)-1) / elem_size) {
    return FAH_ERR_NOMEM;
  }

  eff_count = (count == 0) ? 1u : count;
  eff_elem = (elem_size == 0) ? 1u : elem_size;
#if defined(FAH_TEST_OOM)
  p = fah_should_fail_alloc() ? NULL : calloc(eff_count, eff_elem);
#else
  p = calloc(eff_count, eff_elem);
#endif
  if (p == NULL) {
    return FAH_ERR_NOMEM;
  }
  *out_ptr = p;
  return FAH_OK;
}

fah_error_code_t fah_realloc(void *ptr, size_t new_size, void **out_ptr) {
  size_t eff_size;
  void *p;

  if (out_ptr == NULL) {
    return FAH_ERR_INVAL;
  }

  eff_size = (new_size == 0) ? 1u : new_size;
#if defined(FAH_TEST_OOM)
  p = fah_should_fail_alloc() ? NULL : realloc(ptr, eff_size);
#else
  p = realloc(ptr, eff_size);
#endif
  if (p == NULL) {
    return FAH_ERR_NOMEM;
  }
  *out_ptr = p;
  return FAH_OK;
}

fah_error_code_t fah_strdup(const char *src, char **out_str) {
  size_t len;
  void *raw = NULL;
  char *copy;
  fah_error_code_t rc;

  if (src == NULL || out_str == NULL) {
    return FAH_ERR_INVAL;
  }
  *out_str = NULL;

  len = strlen(src);
  rc = fah_malloc(len + 1u, &raw);
  if (rc != FAH_OK) {
    return rc;
  }
  copy = (char *)raw;
  memcpy(copy, src, len + 1u);
  *out_str = copy;
  return FAH_OK;
}

void fah_free(void *ptr) {
  if (ptr != NULL) {
    free(ptr);
  }
}
