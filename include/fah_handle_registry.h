/**
 * @file fah_handle_registry.h
 * @brief Thread-safe, generation-counted, type-tagged positive 31-bit INTEGER
 * handle registry bridging 64-bit C pointers to FORTRAN I through Fortran 95.
 */

#ifndef FAH_HANDLE_REGISTRY_H
#define FAH_HANDLE_REGISTRY_H

/* clang-format off */
#include <stddef.h>
#include "fah_error.h"
#include "fah_export.h"
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @brief Positive 31-bit integer handle (`> 0` when valid, `0` when null)
 * compatible with signed 32-bit `INTEGER` across FORTRAN I (1957) through
 * Fortran 2023.
 */
typedef int fah_handle_t;

/** @brief Maximum number of concurrent live handles in the registry. */
#define FAH_HANDLE_MAX_SLOTS 4096

/**
 * @brief Runtime object type tag encoded into bits `[26..30]` of
 * `fah_handle_t`.
 */
typedef enum fah_handle_tag {
  FAH_TAG_NONE = 0,         /**< Unallocated / invalid slot */
  FAH_TAG_CLIENT = 1,       /**< Boxed `struct FahClientBox` */
  FAH_TAG_CONFIG = 2,       /**< Boxed `struct HttpConfig` */
  FAH_TAG_REQUEST = 3,      /**< Boxed `struct FahRequestBox` */
  FAH_TAG_RESPONSE = 4,     /**< Boxed `struct FahResponseBox` */
  FAH_TAG_HEADERS = 5,      /**< Boxed `struct HttpHeaders` */
  FAH_TAG_COOKIE_JAR = 6,   /**< Boxed `struct HttpCookieJar` */
  FAH_TAG_PARTS = 7,        /**< Boxed `struct HttpParts` */
  FAH_TAG_MULTI_REQ = 8,    /**< Boxed `struct HttpMultiRequest` */
  FAH_TAG_FUTURE = 9,       /**< Boxed `struct HttpFuture` */
  FAH_TAG_EVENT_LOOP = 10,  /**< `struct ModalityEventLoop *` */
  FAH_TAG_THREAD_POOL = 11, /**< `struct AbstractHttpThreadPool *` */
  FAH_TAG_MUTEX = 12,       /**< `struct AbstractHttpMutex *` */
  FAH_TAG_COND = 13,        /**< `struct AbstractHttpCond *` */
  FAH_TAG_TLS_KEY = 14,     /**< `struct AbstractHttpTlsKey *` */
  FAH_TAG_COROUTINE = 15,   /**< Boxed `struct FahCoroutineBox` */
  FAH_TAG_MSG_BUS = 16,     /**< `struct AbstractHttpMessageBus *` */
  FAH_TAG_ACTOR = 17,       /**< Boxed `struct FahActorBox` */
  FAH_TAG_PROCESS = 18,     /**< `struct AbstractHttpProcess *` */
  FAH_TAG_IPC_PIPE = 19,    /**< Boxed `struct AbstractHttpIpcPipe` */
  FAH_TAG_CMP_BINDING = 20, /**< Boxed `struct CmpProgressBinding` */
  FAH_TAG_POLL_QUEUE = 21   /**< Boxed `struct FahPollQueue` (FORTRAN I-III) */
} fah_handle_tag_t;

/**
 * @brief Register a non-NULL C pointer under `tag` and return a positive 31-bit
 * integer handle (`*out_handle > 0`).
 * @param[in] tag Non-zero `fah_handle_tag_t` identifying the pointer type.
 * @param[in] ptr Non-NULL C object pointer to store.
 * @param[out] out_handle Receives newly minted positive integer handle.
 * @return `FAH_OK` on success, `FAH_ERR_INVAL` on invalid tag/NULL args, or
 * `FAH_ERR_HANDLE_TABLE_FULL` when all `FAH_HANDLE_MAX_SLOTS` slots are active.
 */
FAH_NO_DISCARD FAH_API fah_error_code_t
fah_handle_register(fah_handle_tag_t tag, void *ptr, fah_handle_t *out_handle);

/**
 * @brief Validate `handle` against `expected_tag` and retrieve its underlying C
 * pointer.
 * @param[in] handle Integer handle supplied by Fortran caller.
 * @param[in] expected_tag Expected `fah_handle_tag_t`.
 * @param[out] out_ptr Receives validated non-NULL C pointer.
 * @return `FAH_OK` on success, `FAH_ERR_INVAL` if `out_ptr == NULL`,
 * `FAH_ERR_NULL_HANDLE` if `handle <= 0`, `FAH_ERR_BAD_HANDLE_TAG` on tag
 * mismatch, or `FAH_ERR_STALE_HANDLE` if freed or out of range.
 */
FAH_NO_DISCARD FAH_API fah_error_code_t fah_handle_resolve(
    fah_handle_t handle, fah_handle_tag_t expected_tag, void **out_ptr);

/**
 * @brief Atomically validate and remove `handle` from the registry, advancing
 * its slot generation counter to invalidate stale copies.
 * @param[in] handle Integer handle to unregister.
 * @param[in] expected_tag Expected `fah_handle_tag_t`.
 * @param[out] out_ptr Receives the detached C pointer for caller cleanup.
 * @return `FAH_OK` on success, or `FAH_ERR_INVAL` / `FAH_ERR_NULL_HANDLE` /
 * `FAH_ERR_BAD_HANDLE_TAG` / `FAH_ERR_STALE_HANDLE`.
 */
FAH_NO_DISCARD FAH_API fah_error_code_t fah_handle_unregister(
    fah_handle_t handle, fah_handle_tag_t expected_tag, void **out_ptr);

/**
 * @brief Inspect the `fah_handle_tag_t` of a currently active `handle`.
 * @param[in] handle Integer handle to inspect.
 * @param[out] out_tag Receives active handle tag.
 * @return `FAH_OK` if active, or `FAH_ERR_INVAL` / `FAH_ERR_NULL_HANDLE` /
 * `FAH_ERR_STALE_HANDLE`.
 */
FAH_NO_DISCARD FAH_API fah_error_code_t
fah_handle_get_tag(fah_handle_t handle, fah_handle_tag_t *out_tag);

/**
 * @brief Return the number of currently active handles in the registry.
 * @return Count of live handles (`0 .. FAH_HANDLE_MAX_SLOTS`).
 */
FAH_API size_t fah_handle_active_count(void);

/**
 * @brief Reset all registry slots for deterministic unit test isolation.
 */
FAH_API void fah_handle_registry_reset(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* FAH_HANDLE_REGISTRY_H */
