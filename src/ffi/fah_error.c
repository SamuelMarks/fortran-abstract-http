/**
 * @file fah_error.c
 * @brief Implementation of the unified `fah_error_code_t` enum mapping,
 * categories, retryability predicates, and `derive_more`-style formatting.
 */

/* clang-format off */
#include <string.h>
#include "fah_error.h"
/* clang-format on */

/**
 * @brief Table row mapping an `fah_error_code_t` to its name, category, and
 * human-readable diagnostic message.
 */
struct fah_error_entry {
  fah_error_code_t code;    /**< Enum code */
  fah_error_category_t cat; /**< Semantic category */
  int retryable;            /**< `1` if transient/retryable */
  const char *name;         /**< Symbolic constant name */
  const char *message;      /**< Diagnostic text */
};

/** @brief Exhaustive lookup table for all unified error variants. */
static const struct fah_error_entry k_fah_errors[] = {
    {FAH_OK, FAH_CAT_NONE, 0, "FAH_OK", "Operation succeeded"},
    {FAH_ERR_INVAL, FAH_CAT_ARGUMENT, 0, "FAH_ERR_INVAL", "Invalid argument"},
    {FAH_ERR_NOMEM, FAH_CAT_MEMORY, 0, "FAH_ERR_NOMEM", "Out of heap memory"},
    {FAH_ERR_IO, FAH_CAT_NETWORK_IO, 1, "FAH_ERR_IO", "Network or I/O failure"},
    {FAH_ERR_NOTSUP, FAH_CAT_PROTOCOL, 0, "FAH_ERR_NOTSUP",
     "Operation or transport backend not supported"},
    {FAH_ERR_TIMEOUT, FAH_CAT_TIMEOUT, 1, "FAH_ERR_TIMEOUT",
     "Network operation timed out"},
    {FAH_ERR_SYSCALL, FAH_CAT_NETWORK_IO, 0, "FAH_ERR_SYSCALL",
     "Underlying operating system call failed"},
    {FAH_ERR_WS_FRAMING, FAH_CAT_PROTOCOL, 0, "FAH_ERR_WS_FRAMING",
     "RFC 6455 WebSocket framing violation"},
    {FAH_ERR_WS_HANDSHAKE, FAH_CAT_PROTOCOL, 0, "FAH_ERR_WS_HANDSHAKE",
     "WebSocket 101 Switching Protocols handshake failed"},
    {FAH_ERR_SSE_PARSE, FAH_CAT_PROTOCOL, 0, "FAH_ERR_SSE_PARSE",
     "Server-Sent Events stream parse error"},
    {FAH_ERR_SSE_OOM, FAH_CAT_MEMORY, 0, "FAH_ERR_SSE_OOM",
     "Server-Sent Events line buffer limit exceeded"},
    {FAH_ERR_WS_CLOSED, FAH_CAT_PROTOCOL, 0, "FAH_ERR_WS_CLOSED",
     "WebSocket connection closed"},
    {FAH_ERR_ACCES, FAH_CAT_TLS_AUTH, 0, "FAH_ERR_ACCES",
     "Permission or TLS peer verification denied"},
    {FAH_ERR_EXIST, FAH_CAT_ARGUMENT, 0, "FAH_ERR_EXIST",
     "Resource already exists"},
    {FAH_ERR_LOOP, FAH_CAT_PROTOCOL, 0, "FAH_ERR_LOOP",
     "Maximum HTTP redirect count exceeded"},
    {FAH_ERR_MSGSIZE, FAH_CAT_PROTOCOL, 0, "FAH_ERR_MSGSIZE",
     "Message or frame exceeds maximum allowed size"},
    {FAH_ERR_CONNREFUSED, FAH_CAT_NETWORK_IO, 1, "FAH_ERR_CONNREFUSED",
     "Remote connection refused"},
    {FAH_ERR_HOSTUNREACH, FAH_CAT_NETWORK_IO, 1, "FAH_ERR_HOSTUNREACH",
     "Remote host unreachable"},
    {FAH_ERR_CANCELED, FAH_CAT_CANCELED, 0, "FAH_ERR_CANCELED",
     "Operation canceled"},
    {FAH_ERR_NULL_HANDLE, FAH_CAT_FFI_HANDLE, 0, "FAH_ERR_NULL_HANDLE",
     "Null or zero Fortran handle supplied"},
    {FAH_ERR_BAD_HANDLE_TAG, FAH_CAT_FFI_HANDLE, 0, "FAH_ERR_BAD_HANDLE_TAG",
     "Fortran handle type tag does not match expected object kind"},
    {FAH_ERR_STALE_HANDLE, FAH_CAT_FFI_HANDLE, 0, "FAH_ERR_STALE_HANDLE",
     "Freed, out-of-range, or stale Fortran handle generation"},
    {FAH_ERR_HANDLE_TABLE_FULL, FAH_CAT_MEMORY, 0, "FAH_ERR_HANDLE_TABLE_FULL",
     "Fortran 31-bit handle registry is full"},
    {FAH_ERR_BUF_TOO_SMALL, FAH_CAT_MEMORY, 0, "FAH_ERR_BUF_TOO_SMALL",
     "Caller Fortran output buffer is smaller than payload"},
    {FAH_ERR_INVALID_SLICE, FAH_CAT_ARGUMENT, 0, "FAH_ERR_INVALID_SLICE",
     "Negative slice length or NULL buffer pointer"},
    {FAH_ERR_CALLBACK_FAULT, FAH_CAT_FFI_HANDLE, 0, "FAH_ERR_CALLBACK_FAULT",
     "User Fortran callback returned an error code"},
    {FAH_ERR_CFI_DESCRIPTOR, FAH_CAT_FFI_HANDLE, 0, "FAH_ERR_CFI_DESCRIPTOR",
     "Invalid Fortran 2018 TS 29113 CFI_cdesc_t descriptor"},
    {FAH_ERR_HOLLERITH_DECODE, FAH_CAT_ARGUMENT, 0, "FAH_ERR_HOLLERITH_DECODE",
     "Out-of-range character code in Hollerith/integer buffer"}};

/**
 * @brief Locate the `fah_error_entry` row matching `code`, or `NULL` if
 * unknown.
 * @param[in] code Unified error code to look up.
 * @return Matching static table entry, or `NULL`.
 */
static const struct fah_error_entry *
fah_find_error_entry(fah_error_code_t code) {
  size_t i;
  size_t count = sizeof(k_fah_errors) / sizeof(k_fah_errors[0]);
  for (i = 0; i < count; i++) {
    if (k_fah_errors[i].code == code) {
      return &k_fah_errors[i];
    }
  }
  return NULL;
}

fah_error_code_t fah_error_from_c(c_abstract_http_error_t c_err) {
  const struct fah_error_entry *e =
      fah_find_error_entry((fah_error_code_t)((int)c_err));
  if (e != NULL) {
    return e->code;
  }
  return FAH_ERR_SYSCALL;
}

fah_error_category_t fah_error_category_of(fah_error_code_t code) {
  const struct fah_error_entry *e = fah_find_error_entry(code);
  return (e != NULL) ? e->cat : FAH_CAT_NETWORK_IO;
}

int fah_error_is_retryable(fah_error_code_t code) {
  const struct fah_error_entry *e = fah_find_error_entry(code);
  return (e != NULL) ? e->retryable : 0;
}

const char *fah_error_name(fah_error_code_t code) {
  const struct fah_error_entry *e = fah_find_error_entry(code);
  return (e != NULL) ? e->name : "FAH_ERR_UNKNOWN";
}

const char *fah_error_message(fah_error_code_t code) {
  const struct fah_error_entry *e = fah_find_error_entry(code);
  return (e != NULL) ? e->message : "Unrecognized error code";
}

/**
 * @brief Append `part` into bounded buffer `out_buf` while tracking total
 * untruncated length in `*io_total`.
 * @param[out] out_buf Destination character buffer (`buf_cap >= 1`).
 * @param[in] buf_cap Total capacity of `out_buf` including trailing NUL.
 * @param[in,out] io_total Running untruncated byte count.
 * @param[in] part NUL-terminated string fragment to append.
 */
static void fah_append_bounded(char *out_buf, size_t buf_cap, size_t *io_total,
                               const char *part) {
  size_t plen = strlen(part);
  size_t cur = *io_total;
  size_t avail = (cur + 1u < buf_cap) ? (buf_cap - 1u - cur) : 0u;
  if (avail > 0u) {
    size_t ncopy = (plen < avail) ? plen : avail;
    memcpy(out_buf + cur, part, ncopy);
  }
  *io_total = cur + plen;
}

fah_error_code_t fah_error_format(fah_error_code_t code, const char *op_name,
                                  char *out_buf, size_t buf_cap,
                                  size_t *out_len) {
  const char *sym;
  const char *msg;
  size_t total = 0u;
  size_t term_idx;

  if (out_buf == NULL || buf_cap == 0u) {
    return FAH_ERR_INVAL;
  }

  sym = fah_error_name(code);
  msg = fah_error_message(code);

  fah_append_bounded(out_buf, buf_cap, &total, "[");
  fah_append_bounded(out_buf, buf_cap, &total, sym);
  fah_append_bounded(out_buf, buf_cap, &total, "] ");
  if (op_name != NULL && op_name[0] != '\0') {
    fah_append_bounded(out_buf, buf_cap, &total, op_name);
    fah_append_bounded(out_buf, buf_cap, &total, ": ");
  }
  fah_append_bounded(out_buf, buf_cap, &total, msg);

  term_idx = (total < buf_cap - 1u) ? total : (buf_cap - 1u);
  out_buf[term_idx] = '\0';

  if (out_len != NULL) {
    *out_len = total;
  }
  return (total < buf_cap) ? FAH_OK : FAH_ERR_BUF_TOO_SMALL;
}
