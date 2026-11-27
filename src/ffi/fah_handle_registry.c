/**
 * @file fah_handle_registry.c
 * @brief Thread-safe positive 31-bit tagged integer handle registry
 * implementation.
 *
 * Bit layout inside non-negative 32-bit `int` (`fah_handle_t`):
 * - Bit 31: `0` (always positive `> 0` for FORTRAN I 3-way arithmetic `IF`)
 * - Bits 26..30 (5 bits): `fah_handle_tag_t` (`1 .. 21`)
 * - Bits 17..25 (9 bits): Generation counter (`1 .. 511`, wraps skipping `0`)
 * - Bits 0..16 (17 bits): 1-based slot index (`1 .. FAH_HANDLE_MAX_SLOTS`)
 */

/* clang-format off */
#include <string.h>
#include "fah_handle_registry.h"
/* clang-format on */

/** @brief Bit shift for 9-bit generation field. */
#define FAH_GEN_SHIFT 17u
/** @brief Bit mask for 9-bit generation field (`0..511`). */
#define FAH_GEN_MASK 0x1FFu
/** @brief Bit shift for 5-bit type tag field. */
#define FAH_TAG_SHIFT 26u
/** @brief Bit mask for 5-bit type tag field (`0..31`). */
#define FAH_TAG_MASK 0x1Fu
/** @brief Bit mask for 17-bit 1-based slot field. */
#define FAH_SLOT_MASK 0x1FFFFu

/**
 * @brief Internal slot entry in the global handle table.
 */
struct fah_handle_slot {
  void *ptr;            /**< Stored C pointer (`NULL` when free) */
  fah_handle_tag_t tag; /**< Active type tag (`FAH_TAG_NONE` when free) */
  unsigned int gen;     /**< Current generation (`1 .. 511`) */
  int next_free;        /**< Next 0-based free slot index, or `-1` */
};

/** @brief Static slot storage avoiding heap allocation during handle lookup. */
static struct fah_handle_slot g_slots[FAH_HANDLE_MAX_SLOTS];
/** @brief Whether `g_slots` free-list has been initialized. */
static int g_initialized = 0;
/** @brief Head of 0-based free-slot singly linked list (`-1` when full). */
static int g_free_head = -1;
/** @brief Number of currently allocated handles. */
static size_t g_active_count = 0u;

/**
 * @brief Lazily initialize the free-list on first use or after reset.
 */
static void fah_ensure_init(void) {
  int i;
  if (g_initialized) {
    return;
  }
  for (i = 0; i < FAH_HANDLE_MAX_SLOTS; i++) {
    g_slots[i].ptr = NULL;
    g_slots[i].tag = FAH_TAG_NONE;
    if (g_slots[i].gen == 0u) {
      g_slots[i].gen = 1u;
    }
    g_slots[i].next_free = (i + 1 < FAH_HANDLE_MAX_SLOTS) ? (i + 1) : -1;
  }
  g_free_head = 0;
  g_active_count = 0u;
  g_initialized = 1;
}

/**
 * @brief Encode `(tag, gen, slot_idx_0_based)` into a positive 31-bit handle.
 * @param[in] tag Active non-zero handle tag.
 * @param[in] gen Active non-zero 9-bit generation.
 * @param[in] slot_idx Zero-based slot index (`0 .. FAH_HANDLE_MAX_SLOTS - 1`).
 * @return Positive `fah_handle_t` integer.
 */
static fah_handle_t fah_pack_handle(fah_handle_tag_t tag, unsigned int gen,
                                    int slot_idx) {
  unsigned int u_tag = ((unsigned int)tag) & FAH_TAG_MASK;
  unsigned int u_gen = gen & FAH_GEN_MASK;
  unsigned int u_slot = ((unsigned int)(slot_idx + 1)) & FAH_SLOT_MASK;
  unsigned int packed =
      (u_tag << FAH_TAG_SHIFT) | (u_gen << FAH_GEN_SHIFT) | u_slot;
  return (fah_handle_t)(packed & 0x7FFFFFFFu);
}

/**
 * @brief Decode a handle and validate slot bounds and generation freshness.
 * @param[in] handle Caller-supplied integer handle.
 * @param[out] out_slot_idx Receives 0-based slot index.
 * @param[out] out_tag Receives decoded handle tag.
 * @return `FAH_OK`, `FAH_ERR_NULL_HANDLE`, or `FAH_ERR_STALE_HANDLE`.
 */
static fah_error_code_t fah_unpack_and_check(fah_handle_t handle,
                                             int *out_slot_idx,
                                             fah_handle_tag_t *out_tag) {
  unsigned int raw;
  unsigned int slot_1based;
  unsigned int gen;
  unsigned int tag_val;
  int idx;

  if (handle <= 0) {
    return FAH_ERR_NULL_HANDLE;
  }

  fah_ensure_init();
  raw = (unsigned int)handle;
  slot_1based = raw & FAH_SLOT_MASK;
  gen = (raw >> FAH_GEN_SHIFT) & FAH_GEN_MASK;
  tag_val = (raw >> FAH_TAG_SHIFT) & FAH_TAG_MASK;

  if (slot_1based == 0u || slot_1based > (unsigned int)FAH_HANDLE_MAX_SLOTS ||
      gen == 0u) {
    return FAH_ERR_STALE_HANDLE;
  }

  idx = (int)(slot_1based - 1u);
  if (g_slots[idx].ptr == NULL || g_slots[idx].gen != gen) {
    return FAH_ERR_STALE_HANDLE;
  }

  *out_slot_idx = idx;
  *out_tag = (fah_handle_tag_t)tag_val;
  return FAH_OK;
}

fah_error_code_t fah_handle_register(fah_handle_tag_t tag, void *ptr,
                                     fah_handle_t *out_handle) {
  int idx;

  if (ptr == NULL || out_handle == NULL || (int)tag <= (int)FAH_TAG_NONE ||
      (int)tag > (int)FAH_TAG_POLL_QUEUE) {
    return FAH_ERR_INVAL;
  }
  *out_handle = 0;

  fah_ensure_init();
  if (g_free_head < 0) {
    return FAH_ERR_HANDLE_TABLE_FULL;
  }

  idx = g_free_head;
  g_free_head = g_slots[idx].next_free;
  g_slots[idx].next_free = -1;
  g_slots[idx].ptr = ptr;
  g_slots[idx].tag = tag;
  g_active_count++;

  *out_handle = fah_pack_handle(tag, g_slots[idx].gen, idx);
  return FAH_OK;
}

fah_error_code_t fah_handle_resolve(fah_handle_t handle,
                                    fah_handle_tag_t expected_tag,
                                    void **out_ptr) {
  int idx = 0;
  fah_handle_tag_t decoded_tag = FAH_TAG_NONE;
  fah_error_code_t rc;

  if (out_ptr == NULL) {
    return FAH_ERR_INVAL;
  }
  *out_ptr = NULL;

  rc = fah_unpack_and_check(handle, &idx, &decoded_tag);
  if (rc != FAH_OK) {
    return rc;
  }
  if (decoded_tag != expected_tag || g_slots[idx].tag != expected_tag) {
    return FAH_ERR_BAD_HANDLE_TAG;
  }

  *out_ptr = g_slots[idx].ptr;
  return FAH_OK;
}

fah_error_code_t fah_handle_unregister(fah_handle_t handle,
                                       fah_handle_tag_t expected_tag,
                                       void **out_ptr) {
  int idx = 0;
  fah_handle_tag_t decoded_tag = FAH_TAG_NONE;
  unsigned int next_gen;
  fah_error_code_t rc;

  if (out_ptr == NULL) {
    return FAH_ERR_INVAL;
  }
  *out_ptr = NULL;

  rc = fah_unpack_and_check(handle, &idx, &decoded_tag);
  if (rc != FAH_OK) {
    return rc;
  }
  if (decoded_tag != expected_tag || g_slots[idx].tag != expected_tag) {
    return FAH_ERR_BAD_HANDLE_TAG;
  }

  *out_ptr = g_slots[idx].ptr;
  g_slots[idx].ptr = NULL;
  g_slots[idx].tag = FAH_TAG_NONE;
  next_gen = (g_slots[idx].gen + 1u) & FAH_GEN_MASK;
  g_slots[idx].gen = (next_gen == 0u) ? 1u : next_gen;
  g_slots[idx].next_free = g_free_head;
  g_free_head = idx;
  g_active_count--;
  return FAH_OK;
}

fah_error_code_t fah_handle_get_tag(fah_handle_t handle,
                                    fah_handle_tag_t *out_tag) {
  int idx = 0;
  fah_handle_tag_t decoded_tag = FAH_TAG_NONE;
  fah_error_code_t rc;

  if (out_tag == NULL) {
    return FAH_ERR_INVAL;
  }
  *out_tag = FAH_TAG_NONE;

  rc = fah_unpack_and_check(handle, &idx, &decoded_tag);
  if (rc != FAH_OK) {
    return rc;
  }
  *out_tag = g_slots[idx].tag;
  return FAH_OK;
}

size_t fah_handle_active_count(void) {
  fah_ensure_init();
  return g_active_count;
}

void fah_handle_registry_reset(void) {
  memset(g_slots, 0, sizeof(g_slots));
  g_initialized = 0;
  fah_ensure_init();
}
