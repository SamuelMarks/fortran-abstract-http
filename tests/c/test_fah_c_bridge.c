/**
 * @file test_fah_c_bridge.c
 * @brief Exhaustive strict ISO C89 unit, integration, edge-case, and OOM
 * fault-injection test suite exercising 100% of functions across `src/ffi/` and
 * `generated/c_abstract_http_fortran_shim.c`.
 */

/* clang-format off */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "c_abstract_http_fortran_shim.h"
#include "fah_alloc.h"
#include "fah_c_bridge.h"
#include "fah_callbacks.h"
#include "fah_cfi_c89.h"
#include "fah_error.h"
#include "fah_f77_strings.h"
#include "fah_handle_registry.h"
#include "fah_hollerith.h"
#include "mock_loopback_server.h"
/* clang-format on */

/* Forward declarations of Fortran-mangled entries in fah_fortran_entries.c */
extern int FAH_FC_GLOBAL(ihgini, IHGINI)(const int *dummy);
extern int FAH_FC_GLOBAL(ihgcln, IHGCLN)(const int *dummy);
extern int FAH_FC_GLOBAL(ihpqcr, IHPQCR)(int *out_q);
extern int FAH_FC_GLOBAL(ihpqfr, IHPQFR)(const int *q_h);
extern int FAH_FC_GLOBAL(ihpqpp, IHPQPP)(const int *q_h, int *out_kind,
                                         int *out_code, int *ibuf,
                                         const int *icap, int *out_len);
extern int FAH_FC_GLOBAL(ihcfcr, IHCFCR)(int *out_cfg);
extern int FAH_FC_GLOBAL(ihcffr, IHCFFR)(const int *cfg_h);
extern int FAH_FC_GLOBAL(ihcfst, IHCFST)(const int *cfg_h,
                                         const int *timeout_ms,
                                         const int *verify_peer,
                                         const int *follow_redirs,
                                         const int *retry_cnt);
extern int FAH_FC_GLOBAL(ihclcr, IHCLCR)(const int *cfg_h, const int *use_raw,
                                         int *out_cli);
extern int FAH_FC_GLOBAL(ihclfr, IHCLFR)(const int *cli_h);
extern int FAH_FC_GLOBAL(ihrqcr, IHRQCR)(int *out_req);
extern int FAH_FC_GLOBAL(ihrqfr, IHRQFR)(const int *req_h);
extern int FAH_FC_GLOBAL(ihrqtg, IHRQTG)(const int *req_h, const int *method,
                                         const int *mode, const int *iurl,
                                         const int *nurl);
extern int FAH_FC_GLOBAL(ihrqhd, IHRQHD)(const int *req_h, const int *mode,
                                         const int *ikey, const int *nkey,
                                         const int *ival, const int *nval);
extern int FAH_FC_GLOBAL(ihrqbd, IHRQBD)(const int *req_h, const int *mode,
                                         const int *ibdy, const int *nbdy);
extern int FAH_FC_GLOBAL(ihrqcb, IHRQCB)(const int *req_h, const int *q_h);
extern int FAH_FC_GLOBAL(ihsend, IHSEND)(const int *cli_h, const int *req_h,
                                         int *out_res);
extern int FAH_FC_GLOBAL(ihrsin, IHRSIN)(const int *res_h, int *out_status,
                                         int *out_len);
extern int FAH_FC_GLOBAL(ihrsbd, IHRSBD)(const int *res_h, const int *mode,
                                         int *ibuf, const int *icap,
                                         int *out_len);
extern int FAH_FC_GLOBAL(ihrshd, IHRSHD)(const int *res_h, const int *mode,
                                         const int *ikey, const int *nkey,
                                         int *ival, const int *icap,
                                         int *out_len);
extern int FAH_FC_GLOBAL(ihrsfr, IHRSFR)(const int *res_h);
extern int FAH_FC_GLOBAL(ihwsop, IHWSOP)(const int *cli_h, const int *req_h,
                                         const int *op, const int *arg,
                                         const int *mode, const int *ibuf,
                                         const int *nlen, const int *q_h);
extern int FAH_FC_GLOBAL(ihsseo, IHSSEO)(const int *cli_h, const int *req_h,
                                         const int *op, const int *retry_ms,
                                         const int *q_h);
extern int FAH_FC_GLOBAL(iherrc, IHERRC)(const int *code, int *out_cat,
                                         int *out_retry, int *ibuf,
                                         const int *icap, int *out_len);

extern void FAH_FC_GLOBAL(fhc4st, FHC4ST)(const int *req_h,
                                          fah_f77_on_chunk_cb on_chunk,
                                          const int *chunk_uid,
                                          fah_f77_on_progress_cb on_prog,
                                          const int *prog_uid, int *out_err);
extern void FAH_FC_GLOBAL(fhc4up, FHC4UP)(const int *req_h,
                                          fah_f77_read_chunk_cb read_chunk,
                                          const int *read_uid,
                                          const int *expected_len,
                                          int *out_err);
extern void FAH_FC_GLOBAL(fhc4ws, FHC4WS)(const int *cli_h, const int *req_h,
                                          fah_f77_ws_on_msg_cb on_msg,
                                          fah_f77_ws_on_err_cb on_err,
                                          fah_f77_ws_on_close_cb on_close,
                                          const int *uid, int *out_err);
extern void FAH_FC_GLOBAL(fhc4ss, FHC4SS)(const int *cli_h, const int *req_h,
                                          fah_f77_sse_on_evt_cb on_evt,
                                          fah_f77_sse_on_err_cb on_err,
                                          fah_f77_sse_on_close_cb on_close,
                                          const int *uid, int *out_err);

extern void FAH_FC_GLOBAL(fh7cfg, FH7CFG)(const int *cfg_h, const int *op,
                                          const int *a1, const int *a2,
                                          const int *a3, const int *a4,
                                          int *out_err);
extern void FAH_FC_GLOBAL(fh7cfs, FH7CFS)(const int *hnd, const int *slot,
                                          const char *str, int *out_err,
                                          fah_charlen_t str_len);
extern void FAH_FC_GLOBAL(fh7hcj, FH7HCJ)(int *io_h, const int *op,
                                          const char *s1, char *s2, int *out_n,
                                          int *out_err, fah_charlen_t s1_len,
                                          fah_charlen_t s2_len);
extern void FAH_FC_GLOBAL(fh7req, FH7REQ)(const int *req_h, const int *op,
                                          const int *iarg, const char *s1,
                                          const char *s2, const char *s3,
                                          const char *s4, int *out_err,
                                          fah_charlen_t l1, fah_charlen_t l2,
                                          fah_charlen_t l3, fah_charlen_t l4);
extern void FAH_FC_GLOBAL(fh7oa2, FH7OA2)(
    const int *req_h, const int *grant_kind, const char *ep, const char *p1,
    const char *p2, const char *p3, const char *p4, const char *p5,
    char *out_s1, int *out_l1, char *out_s2, int *out_l2, int *out_err,
    fah_charlen_t eplen, fah_charlen_t l1, fah_charlen_t l2, fah_charlen_t l3,
    fah_charlen_t l4, fah_charlen_t l5, fah_charlen_t ol1, fah_charlen_t ol2);
extern void FAH_FC_GLOBAL(fh7res, FH7RES)(const int *res_h, const int *op,
                                          const int *idx, char *s1, char *s2,
                                          int *out_n1, int *out_n2,
                                          int *out_err, fah_charlen_t l1,
                                          fah_charlen_t l2);
extern void FAH_FC_GLOBAL(fh7mul,
                          FH7MUL)(const int *cli_h, int *io_h, const int *op,
                                  const int *arg, int *fut_arr, const int *cap,
                                  int *out_n1, int *out_n2, int *out_err);
extern void FAH_FC_GLOBAL(fh7stm, FH7STM)(const int *cli_h, const int *loop_h,
                                          const int *req_h, const int *op,
                                          const int *iarg, const char *str,
                                          void (*cb1)(void), void (*cb2)(void),
                                          void (*cb3)(void), const int *uid,
                                          int *out_err, fah_charlen_t slen);
extern void FAH_FC_GLOBAL(fh7mod, FH7MOD)(const int *op, int *io_h1, int *io_h2,
                                          const int *a1, const int *a2,
                                          const int *a3, char *str,
                                          void (*cb)(void), int *out_n1,
                                          int *out_err, fah_charlen_t slen);
extern void FAH_FC_GLOBAL(fh7err, FH7ERR)(const int *code, const char *op_name,
                                          char *out_msg, int *out_cat,
                                          int *out_retry, int *out_len,
                                          int *out_err, fah_charlen_t oplen,
                                          fah_charlen_t msglen);

#define ASSERT_TRUE(cond)                                                      \
  do {                                                                         \
    if (!(cond)) {                                                             \
      fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond);          \
      exit(1);                                                                 \
    }                                                                          \
  } while (0)

/* --- Test Callbacks --- */

static int g_cb_hits = 0;

static int test_on_chunk(const int *uid, const char *buf, const int *len) {
  (void)buf;
  g_cb_hits += (*len >= 0) ? 1 : 0;
  return (*uid == -99) ? 1 : 0;
}

static int test_read_chunk(const int *uid, char *buf, const int *cap,
                           int *out_written) {
  if (*uid == -99) {
    *out_written = -1;
    return 1;
  }
  if (g_cb_hits == 0 && *cap >= 4) {
    memcpy(buf, "DATA", 4u);
    *out_written = 4;
    g_cb_hits = 1;
    return 0;
  }
  *out_written = 0;
  return 0;
}

static int test_on_prog(const int *cur, const int *tot, const int *uid) {
  (void)cur;
  (void)tot;
  return (*uid == -99) ? 1 : 0;
}

static int test_ws_msg(const int *op, const char *pl, const int *len,
                       const int *fin, const int *uid) {
  (void)op;
  (void)pl;
  (void)len;
  (void)fin;
  g_cb_hits++;
  return (*uid == -99) ? 1 : 0;
}

static int test_ws_err(const int *ec, const int *uid) {
  (void)ec;
  return (*uid == -99) ? 1 : 0;
}

static int test_ws_cls(const int *sc, const int *uid) {
  (void)sc;
  return (*uid == -99) ? 1 : 0;
}

static int test_sse_evt(const char *id, const int *idl, const char *ev,
                        const int *evl, const char *dt, const int *dtl,
                        const int *uid) {
  (void)id;
  (void)idl;
  (void)ev;
  (void)evl;
  (void)dt;
  (void)dtl;
  g_cb_hits++;
  return (*uid == -99) ? 1 : 0;
}

static int test_sse_err(const int *ec, const int *uid) {
  (void)ec;
  return (*uid == -99) ? 1 : 0;
}

static int test_sse_cls(const int *uid) { return (*uid == -99) ? 1 : 0; }

static int test_timer_cb(const int *loop_h, const int *tid, const int *uid) {
  (void)tid;
  g_cb_hits += *uid;
  FAH_IGNORE_RC(fah_loop_drive(*loop_h, 2));
  return 0;
}

static int test_task_cb(const int *uid) {
  if (*uid == 77) {
    FAH_IGNORE_RC(fah_coro_op(0, 1, NULL));
  }
  g_cb_hits += *uid;
  return 0;
}

static int test_actor_cb(const int *self_h, const int *mtype, const int *mval,
                         const int *sh, const int *uid) {
  (void)self_h;
  (void)mtype;
  (void)sh;
  g_cb_hits += (*mval + *uid);
  return 0;
}

/* --- Sub-suites --- */

static void test_abi_shim_and_alloc(void) {
  void *p = NULL;
  char *s = NULL;

  ASSERT_TRUE(fah_abi_sizeof_http_config() > 0u);
  ASSERT_TRUE(fah_abi_sizeof_http_client() > 0u);
  ASSERT_TRUE(fah_abi_sizeof_http_headers() > 0u);
  ASSERT_TRUE(fah_abi_sizeof_http_cookie_jar() > 0u);
  ASSERT_TRUE(fah_abi_sizeof_http_request() > 0u);
  ASSERT_TRUE(fah_abi_sizeof_http_response() > 0u);
  ASSERT_TRUE(fah_abi_sizeof_http_multi_request() > 0u);
  ASSERT_TRUE(fah_abi_sizeof_http_future() > 0u);

  fah_oom_reset();
  ASSERT_TRUE(fah_oom_get_alloc_count() == 0u);
  ASSERT_TRUE(fah_malloc(16u, NULL) == FAH_ERR_INVAL);
  ASSERT_TRUE(fah_calloc(1u, 16u, NULL) == FAH_ERR_INVAL);
  ASSERT_TRUE(fah_calloc(((size_t)-1), 4u, &p) == FAH_ERR_NOMEM);
  ASSERT_TRUE(fah_realloc(NULL, 16u, NULL) == FAH_ERR_INVAL);
  ASSERT_TRUE(fah_strdup(NULL, &s) == FAH_ERR_INVAL);
  ASSERT_TRUE(fah_strdup("hi", NULL) == FAH_ERR_INVAL);

  ASSERT_TRUE(fah_malloc(0u, &p) == FAH_OK);
  ASSERT_TRUE(fah_realloc(p, 0u, &p) == FAH_OK);
  fah_free(p);
  fah_free(NULL);

  ASSERT_TRUE(fah_calloc(0u, 0u, &p) == FAH_OK);
  fah_free(p);

  fah_oom_set_fail_after(1);
  ASSERT_TRUE(fah_malloc(8u, &p) == FAH_ERR_NOMEM);
  fah_oom_set_fail_after(1);
  ASSERT_TRUE(fah_calloc(1u, 8u, &p) == FAH_ERR_NOMEM);
  fah_oom_set_fail_after(1);
  ASSERT_TRUE(fah_realloc(NULL, 8u, &p) == FAH_ERR_NOMEM);
  fah_oom_set_fail_after(1);
  ASSERT_TRUE(fah_strdup("x", &s) == FAH_ERR_NOMEM);
  ASSERT_TRUE(fah_oom_get_alloc_count() >= 4u);
  fah_oom_reset();
}

static void test_errors_and_registry(void) {
  char buf[128];
  char tiny[6];
  size_t flen = 0u;
  int dummy_val = 42;
  fah_handle_t h = 0;
  fah_handle_t handles[FAH_HANDLE_MAX_SLOTS];
  fah_handle_tag_t tag = FAH_TAG_NONE;
  void *out = NULL;
  int i;

  ASSERT_TRUE(fah_error_from_c(C_ABSTRACT_HTTP_SUCCESS) == FAH_OK);
  ASSERT_TRUE(fah_error_from_c((c_abstract_http_error_t)-9999) ==
              FAH_ERR_SYSCALL);
  ASSERT_TRUE(fah_error_category_of(FAH_OK) == FAH_CAT_NONE);
  ASSERT_TRUE(fah_error_category_of((fah_error_code_t)-9999) ==
              FAH_CAT_NETWORK_IO);
  ASSERT_TRUE(fah_error_is_retryable(FAH_ERR_TIMEOUT) == 1);
  ASSERT_TRUE(fah_error_is_retryable((fah_error_code_t)-9999) == 0);
  ASSERT_TRUE(strcmp(fah_error_name(FAH_OK), "FAH_OK") == 0);
  ASSERT_TRUE(
      strcmp(fah_error_name((fah_error_code_t)-9999), "FAH_ERR_UNKNOWN") == 0);
  ASSERT_TRUE(strlen(fah_error_message((fah_error_code_t)-9999)) > 0u);

  ASSERT_TRUE(fah_error_format(FAH_OK, NULL, NULL, 10u, &flen) ==
              FAH_ERR_INVAL);
  ASSERT_TRUE(fah_error_format(FAH_OK, NULL, buf, 0u, &flen) == FAH_ERR_INVAL);
  ASSERT_TRUE(fah_error_format(FAH_ERR_INVAL, "op", buf, sizeof(buf), &flen) ==
              FAH_OK);
  ASSERT_TRUE(fah_error_format(FAH_ERR_INVAL, "", tiny, sizeof(tiny), NULL) ==
              FAH_ERR_BUF_TOO_SMALL);

  fah_handle_registry_reset();
  ASSERT_TRUE(fah_handle_active_count() == 0u);
  ASSERT_TRUE(fah_handle_register(FAH_TAG_NONE, &dummy_val, &h) ==
              FAH_ERR_INVAL);
  ASSERT_TRUE(fah_handle_register(FAH_TAG_CLIENT, NULL, &h) == FAH_ERR_INVAL);
  ASSERT_TRUE(fah_handle_register(FAH_TAG_CLIENT, &dummy_val, NULL) ==
              FAH_ERR_INVAL);
  ASSERT_TRUE(fah_handle_resolve(1, FAH_TAG_CLIENT, NULL) == FAH_ERR_INVAL);
  ASSERT_TRUE(fah_handle_unregister(1, FAH_TAG_CLIENT, NULL) == FAH_ERR_INVAL);
  ASSERT_TRUE(fah_handle_get_tag(1, NULL) == FAH_ERR_INVAL);
  ASSERT_TRUE(fah_handle_resolve(0, FAH_TAG_CLIENT, &out) ==
              FAH_ERR_NULL_HANDLE);
  ASSERT_TRUE(fah_handle_resolve(9999, FAH_TAG_CLIENT, &out) ==
              FAH_ERR_STALE_HANDLE);
  ASSERT_TRUE(fah_handle_get_tag(0, &tag) == FAH_ERR_NULL_HANDLE);

  /* Cycle generation counter > 512 times to test 9-bit wrap skipping 0 */
  for (i = 0; i < 520; i++) {
    ASSERT_TRUE(fah_handle_register(FAH_TAG_CLIENT, &dummy_val, &h) == FAH_OK);
    ASSERT_TRUE(h > 0);
    ASSERT_TRUE(fah_handle_get_tag(h, &tag) == FAH_OK && tag == FAH_TAG_CLIENT);
    ASSERT_TRUE(fah_handle_resolve(h, FAH_TAG_REQUEST, &out) ==
                FAH_ERR_BAD_HANDLE_TAG);
    ASSERT_TRUE(fah_handle_unregister(h, FAH_TAG_REQUEST, &out) ==
                FAH_ERR_BAD_HANDLE_TAG);
    ASSERT_TRUE(fah_handle_unregister(h, FAH_TAG_CLIENT, &out) == FAH_OK);
    ASSERT_TRUE(fah_handle_resolve(h, FAH_TAG_CLIENT, &out) ==
                FAH_ERR_STALE_HANDLE);
  }

  /* Exhaust table capacity */
  for (i = 0; i < FAH_HANDLE_MAX_SLOTS; i++) {
    ASSERT_TRUE(fah_handle_register(FAH_TAG_CONFIG, &dummy_val, &handles[i]) ==
                FAH_OK);
  }
  ASSERT_TRUE(fah_handle_register(FAH_TAG_CONFIG, &dummy_val, &h) ==
              FAH_ERR_HANDLE_TABLE_FULL);
  for (i = 0; i < FAH_HANDLE_MAX_SLOTS; i++) {
    ASSERT_TRUE(fah_handle_unregister(handles[i], FAH_TAG_CONFIG, &out) ==
                FAH_OK);
  }
  ASSERT_TRUE(fah_handle_active_count() == 0u);
}

static void test_codecs_cfi_and_callbacks(void) {
  int icodes[4] = {65, 66, 67, 300};
  int out_ic[4];
  int hwords[2];
  int actual = 0;
  char *cs = NULL;
  char fbuf[8];
  size_t sz = 0u;
  unsigned char mat[6] = {1, 2, 3, 4, 5, 6};
  struct {
    fah_cfi_cdesc_t hdr;
    fah_cfi_dim_t extra_dims[2];
  } cfi_box;
  const void *bytes = NULL;
  void *owned = NULL;

  ASSERT_TRUE(fah_icodes_to_cstr(icodes, 3, NULL) == FAH_ERR_INVAL);
  ASSERT_TRUE(fah_icodes_to_cstr(NULL, 1, &cs) == FAH_ERR_INVALID_SLICE);
  ASSERT_TRUE(fah_icodes_to_cstr(icodes, 4, &cs) == FAH_ERR_HOLLERITH_DECODE);
  ASSERT_TRUE(fah_icodes_to_cstr(icodes, 3, &cs) == FAH_OK);
  ASSERT_TRUE(strcmp(cs, "ABC") == 0);
  fah_free(cs);
  fah_oom_set_fail_after(1);
  ASSERT_TRUE(fah_icodes_to_cstr(icodes, 3, &cs) == FAH_ERR_NOMEM);

  ASSERT_TRUE(fah_cbytes_to_icodes("ABCD", 4u, NULL, 2, &actual) ==
              FAH_ERR_INVALID_SLICE);
  ASSERT_TRUE(fah_cbytes_to_icodes("ABCD", 4u, out_ic, 2, &actual) ==
              FAH_ERR_BUF_TOO_SMALL);
  ASSERT_TRUE(fah_cbytes_to_icodes("AB", 2u, out_ic, 4, NULL) == FAH_OK);

  ASSERT_TRUE(fah_hollerith4_to_cstr(NULL, 2, NULL) == FAH_ERR_INVAL);
  ASSERT_TRUE(fah_hollerith4_to_cstr(NULL, 2, &cs) == FAH_ERR_INVALID_SLICE);
  ASSERT_TRUE(fah_cbytes_to_hollerith4("HELLO", 5u, NULL, 1, &actual) ==
              FAH_ERR_INVALID_SLICE);
  ASSERT_TRUE(fah_cbytes_to_hollerith4("HELLO", 5u, hwords, 1, &actual) ==
              FAH_ERR_BUF_TOO_SMALL);
  ASSERT_TRUE(fah_cbytes_to_hollerith4("HELLO", 5u, hwords, 2, &actual) ==
              FAH_OK);
  ASSERT_TRUE(fah_hollerith4_to_cstr(hwords, 5, &cs) == FAH_OK);
  ASSERT_TRUE(strcmp(cs, "HELLO") == 0);
  fah_free(cs);
  fah_oom_set_fail_after(1);
  ASSERT_TRUE(fah_hollerith4_to_cstr(hwords, 5, &cs) == FAH_ERR_NOMEM);

  ASSERT_TRUE(fah_fstr_to_cstr("abc", 3u, 1, NULL) == FAH_ERR_INVAL);
  ASSERT_TRUE(fah_fstr_to_cstr(NULL, 3u, 1, &cs) == FAH_ERR_INVALID_SLICE);
  ASSERT_TRUE(fah_fstr_to_cstr("hi  ", 4u, 0, &cs) == FAH_OK &&
              strlen(cs) == 4u);
  fah_free(cs);
  fah_oom_set_fail_after(1);
  ASSERT_TRUE(fah_fstr_to_cstr("abc", 3u, 1, &cs) == FAH_ERR_NOMEM);
  fah_oom_reset();
  ASSERT_TRUE(fah_cbytes_to_fstr(NULL, 2u, fbuf, 4u, &sz, 1) ==
              FAH_ERR_INVALID_SLICE);
  ASSERT_TRUE(fah_cbytes_to_fstr("hi", 2u, fbuf, 8u, &sz, 1) == FAH_OK);
  ASSERT_TRUE(fah_cbytes_to_fstr("0123456789", 10u, fbuf, 4u, NULL, 0) ==
              FAH_ERR_BUF_TOO_SMALL);

  /* CFI descriptor unit tests (contiguous + strided + error branches) */
  ASSERT_TRUE(fah_cfi_extract_bytes(NULL, NULL, &sz, &owned) == FAH_ERR_INVAL);
  ASSERT_TRUE(fah_cfi_extract_bytes(NULL, &bytes, &sz, &owned) ==
              FAH_ERR_CFI_DESCRIPTOR);
  memset(&cfi_box, 0, sizeof(cfi_box));
  cfi_box.hdr.version = FAH_CFI_VERSION;
  cfi_box.hdr.rank = 0;
  cfi_box.hdr.elem_len = 1u;
  ASSERT_TRUE(fah_cfi_extract_bytes(&cfi_box.hdr, &bytes, &sz, &owned) ==
              FAH_ERR_CFI_DESCRIPTOR);
  cfi_box.hdr.rank = 1;
  ASSERT_TRUE(fah_cfi_extract_bytes(&cfi_box.hdr, &bytes, &sz, &owned) ==
              FAH_OK);

  /* Strided 2D slice + OOM + overflow checks */
  cfi_box.hdr.base_addr = mat;
  cfi_box.hdr.rank = 2;
  cfi_box.hdr.elem_len = 1u;
  cfi_box.hdr.dim[0].extent = 2;
  cfi_box.hdr.dim[0].sm = 2; /* non-unit stride! */
  cfi_box.extra_dims[0].extent = 2;
  cfi_box.extra_dims[0].sm = 3;
  ASSERT_TRUE(fah_cfi_extract_bytes(&cfi_box.hdr, &bytes, &sz, &owned) ==
              FAH_OK);
  ASSERT_TRUE(sz == 4u && owned != NULL);
  fah_free(owned);
  fah_oom_set_fail_after(1);
  ASSERT_TRUE(fah_cfi_extract_bytes(&cfi_box.hdr, &bytes, &sz, &owned) ==
              FAH_ERR_NOMEM);
  fah_oom_reset();

  cfi_box.hdr.dim[0].extent = 65536;
  cfi_box.extra_dims[0].extent = (ptrdiff_t)(((size_t)-1) / 32768u);
  ASSERT_TRUE(fah_cfi_extract_bytes(&cfi_box.hdr, &bytes, &sz, &owned) ==
              FAH_ERR_NOMEM);
  cfi_box.hdr.rank = 1;
  cfi_box.hdr.dim[0].extent = 4;
  cfi_box.hdr.elem_len = ((size_t)-1) / 2u;
  ASSERT_TRUE(fah_cfi_extract_bytes(&cfi_box.hdr, &bytes, &sz, &owned) ==
              FAH_ERR_NOMEM);

  cfi_box.hdr.rank = 2;
  cfi_box.hdr.dim[0].extent = 2;
  cfi_box.extra_dims[0].extent = 2;
  cfi_box.hdr.elem_len = 0u;
  cfi_box.hdr.dim[0].sm = 0;
  cfi_box.extra_dims[0].sm = 0;
  ASSERT_TRUE(fah_cfi_extract_bytes(&cfi_box.hdr, &bytes, &sz, &owned) ==
              FAH_OK);
  cfi_box.hdr.dim[0].extent = 0;
  ASSERT_TRUE(fah_cfi_extract_bytes(&cfi_box.hdr, &bytes, &sz, &owned) ==
              FAH_OK);

  ASSERT_TRUE(fah_cfi_populate_allocatable_bytes(NULL, "a", 1u) ==
              FAH_ERR_CFI_DESCRIPTOR);
  cfi_box.hdr.rank = 1;
  cfi_box.hdr.elem_len = 1u;
  cfi_box.hdr.type = 1;
  cfi_box.hdr.attribute = FAH_CFI_ATTRIBUTE_ALLOCATABLE;
  cfi_box.hdr.base_addr = NULL;
  ASSERT_TRUE(fah_cfi_populate_allocatable_bytes(&cfi_box.hdr, NULL, 2u) ==
              FAH_ERR_INVALID_SLICE);
  ASSERT_TRUE(fah_cfi_populate_allocatable_bytes(&cfi_box.hdr, "ab", 2u) ==
                  FAH_OK &&
              cfi_box.hdr.base_addr != NULL);
  /* Second call exercises non-NULL base_addr CFI_deallocate path */
  ASSERT_TRUE(fah_cfi_populate_allocatable_bytes(&cfi_box.hdr, "", 0u) ==
              FAH_OK);
  if (cfi_box.hdr.base_addr != NULL) {
    free(cfi_box.hdr.base_addr);
    cfi_box.hdr.base_addr = NULL;
  }
  ASSERT_TRUE(fah_cfi_populate_allocatable_bytes(
                  &cfi_box.hdr, "",
                  ((size_t)1 << (sizeof(size_t) * 8u - 2u))) == FAH_ERR_NOMEM);

  /* Callback trampolines direct unit checks */
  {
    FahReqCallbackCtx rctx;
    FahWsCallbackCtx wctx;
    FahSseCallbackCtx sctx;
    struct c_abstract_http_ws_event wev;
    struct c_abstract_http_sse_event sev;
    fah_handle_t qh = 0;
    char ubuf[8];
    size_t uout = 0u;

    ASSERT_TRUE(fah_poll_queue_create(&qh) == FAH_OK);
    ASSERT_TRUE(fah_trampoline_on_chunk(NULL, "x", 1u) ==
                C_ABSTRACT_HTTP_ERR_INVAL);
    ASSERT_TRUE(fah_trampoline_read_chunk(NULL, ubuf, 8u, &uout) ==
                C_ABSTRACT_HTTP_ERR_INVAL);
    ASSERT_TRUE(fah_trampoline_on_progress(0u, 0u, NULL) ==
                C_ABSTRACT_HTTP_ERR_INVAL);
    ASSERT_TRUE(fah_trampoline_ws_on_msg(NULL, NULL) ==
                C_ABSTRACT_HTTP_ERR_INVAL);
    ASSERT_TRUE(fah_trampoline_ws_on_err(1, NULL) == C_ABSTRACT_HTTP_ERR_INVAL);
    ASSERT_TRUE(fah_trampoline_ws_on_close(1000, NULL) ==
                C_ABSTRACT_HTTP_ERR_INVAL);
    ASSERT_TRUE(fah_trampoline_sse_on_evt(NULL, NULL) ==
                C_ABSTRACT_HTTP_ERR_INVAL);
    ASSERT_TRUE(fah_trampoline_sse_on_err(1, NULL) ==
                C_ABSTRACT_HTTP_ERR_INVAL);
    ASSERT_TRUE(fah_trampoline_sse_on_close(NULL) == C_ABSTRACT_HTTP_ERR_INVAL);

    memset(&rctx, 0, sizeof(rctx));
    ASSERT_TRUE(fah_trampoline_on_progress(1u, 2u, &rctx) ==
                C_ABSTRACT_HTTP_SUCCESS);
    rctx.on_chunk = test_on_chunk;
    rctx.on_chunk_uid = -99;
    rctx.poll_queue_h = qh;
    ASSERT_TRUE(fah_trampoline_on_chunk(&rctx, "a", 1u) ==
                C_ABSTRACT_HTTP_ERR_CANCELED);
    rctx.read_chunk = test_read_chunk;
    rctx.read_chunk_uid = -99;
    ASSERT_TRUE(fah_trampoline_read_chunk(&rctx, ubuf, 8u, &uout) ==
                C_ABSTRACT_HTTP_ERR_CANCELED);
    rctx.on_progress = test_on_prog;
    rctx.on_progress_uid = -99;
    ASSERT_TRUE(fah_trampoline_on_progress(1u, 2u, &rctx) ==
                C_ABSTRACT_HTTP_ERR_CANCELED);

    memset(&wctx, 0, sizeof(wctx));
    memset(&wev, 0, sizeof(wev));
    wev.opcode = C_ABSTRACT_HTTP_WS_OPCODE_TEXT;
    wev.payload = (const unsigned char *)"m";
    wev.payload_len = 1u;
    wev.is_fin = 1;
    wctx.on_msg = test_ws_msg;
    wctx.on_err = test_ws_err;
    wctx.on_close = test_ws_cls;
    wctx.user_id = 1;
    wctx.poll_queue_h = qh;
    ASSERT_TRUE(fah_trampoline_ws_on_msg(&wev, &wctx) ==
                C_ABSTRACT_HTTP_SUCCESS);
    ASSERT_TRUE(fah_trampoline_ws_on_err(-3, &wctx) == C_ABSTRACT_HTTP_SUCCESS);
    ASSERT_TRUE(fah_trampoline_ws_on_close(1000, &wctx) ==
                C_ABSTRACT_HTTP_SUCCESS);
    wctx.user_id = -99;
    ASSERT_TRUE(fah_trampoline_ws_on_msg(&wev, &wctx) ==
                C_ABSTRACT_HTTP_ERR_CANCELED);
    ASSERT_TRUE(fah_trampoline_ws_on_err(-3, &wctx) ==
                C_ABSTRACT_HTTP_ERR_CANCELED);
    ASSERT_TRUE(fah_trampoline_ws_on_close(1000, &wctx) ==
                C_ABSTRACT_HTTP_ERR_CANCELED);

    memset(&sctx, 0, sizeof(sctx));
    memset(&sev, 0, sizeof(sev));
    sev.id = "1";
    sev.event = "ev";
    sev.data = "d";
    sev.data_len = 1u;
    sctx.on_evt = test_sse_evt;
    sctx.on_err = test_sse_err;
    sctx.on_close = test_sse_cls;
    sctx.user_id = 1;
    sctx.poll_queue_h = qh;
    ASSERT_TRUE(fah_trampoline_sse_on_evt(&sev, &sctx) ==
                C_ABSTRACT_HTTP_SUCCESS);
    ASSERT_TRUE(fah_trampoline_sse_on_err(-3, &sctx) ==
                C_ABSTRACT_HTTP_SUCCESS);
    ASSERT_TRUE(fah_trampoline_sse_on_close(&sctx) == C_ABSTRACT_HTTP_SUCCESS);
    sctx.user_id = -99;
    ASSERT_TRUE(fah_trampoline_sse_on_evt(&sev, &sctx) ==
                C_ABSTRACT_HTTP_ERR_CANCELED);
    ASSERT_TRUE(fah_trampoline_sse_on_err(-3, &sctx) ==
                C_ABSTRACT_HTTP_ERR_CANCELED);
    ASSERT_TRUE(fah_trampoline_sse_on_close(&sctx) ==
                C_ABSTRACT_HTTP_ERR_CANCELED);

    /* Stale poll_queue_h triggers queue-push failure across all 7 trampolines
     */
    rctx.poll_queue_h = 999999;
    wctx.poll_queue_h = 999999;
    sctx.poll_queue_h = 999999;
    ASSERT_TRUE(fah_trampoline_on_chunk(&rctx, "a", 1u) ==
                C_ABSTRACT_HTTP_ERR_NOMEM);
    ASSERT_TRUE(fah_trampoline_ws_on_msg(&wev, &wctx) ==
                C_ABSTRACT_HTTP_ERR_NOMEM);
    ASSERT_TRUE(fah_trampoline_ws_on_err(-3, &wctx) ==
                C_ABSTRACT_HTTP_ERR_NOMEM);
    ASSERT_TRUE(fah_trampoline_ws_on_close(1000, &wctx) ==
                C_ABSTRACT_HTTP_ERR_NOMEM);
    ASSERT_TRUE(fah_trampoline_sse_on_evt(&sev, &sctx) ==
                C_ABSTRACT_HTTP_ERR_NOMEM);
    ASSERT_TRUE(fah_trampoline_sse_on_err(-3, &sctx) ==
                C_ABSTRACT_HTTP_ERR_NOMEM);
    ASSERT_TRUE(fah_trampoline_sse_on_close(&sctx) ==
                C_ABSTRACT_HTTP_ERR_NOMEM);

    {
      int pk = 0, pc = 0;
      char pev[16], pdat[16];
      size_t pevl = 0u, pdatl = 0u;
      while (fah_poll_queue_pop(qh, &pk, &pc, pev, sizeof(pev), &pevl, pdat,
                                sizeof(pdat), &pdatl) == FAH_OK &&
             pk != (int)FAH_POLL_NONE) {
      }
    }
    ASSERT_TRUE(fah_poll_queue_destroy(qh) == FAH_OK);
  }
}

static void test_bridge_and_fortran_entries(int port) {
  fah_handle_t cfg_h = 0, cli_h = 0, cli_def = 0, hdr_h = 0, jar_h = 0;
  fah_handle_t req_h = 0, res_h = 0, multi_h = 0, futs[2];
  char url[128], buf[512], kbuf[64], vbuf[64];
  size_t sz = 0u, sz2 = 0u;
  int st = 0, i, dummy = 0, rc_i = 0;

  ASSERT_TRUE(FAH_FC_GLOBAL(ihgini, IHGINI)(&dummy) == 0);

  /* Config, Headers, Cookie Jar */
  ASSERT_TRUE(fah_config_create(&cfg_h) == FAH_OK);
  ASSERT_TRUE(fah_cookie_jar_create(&jar_h) == FAH_OK);
  ASSERT_TRUE(fah_cookie_jar_set(jar_h, "k", "v") == FAH_OK);
  ASSERT_TRUE(fah_cookie_jar_get(jar_h, "k", buf, sizeof(buf), &sz, 0) ==
              FAH_OK);
  ASSERT_TRUE(fah_cookie_jar_parse_set(jar_h, "s2=v2; Path=/") == FAH_OK);
  ASSERT_TRUE(fah_cookie_jar_build_hdr(jar_h, "http://127.0.0.1/hello", buf,
                                       sizeof(buf), &sz, 0) == FAH_OK);
  ASSERT_TRUE(fah_config_set_timeouts(cfg_h, 5000L, 1000L, 2000L, 2000L) ==
              FAH_OK);
  ASSERT_TRUE(fah_config_set_tls_flags(cfg_h, 0, 0, 1, 1) == FAH_OK);
  ASSERT_TRUE(fah_config_set_versions_and_retry(cfg_h, 0u, 0u, 2, 1) == FAH_OK);
  ASSERT_TRUE(fah_config_set_modality_and_pool(cfg_h, 0, 1u, 2u, 1, 5000L,
                                               4u) == FAH_OK);
  ASSERT_TRUE(fah_config_set_strings(cfg_h, "fah-test/0.1", NULL, NULL, NULL,
                                     NULL, NULL, NULL, NULL) == FAH_OK);
  ASSERT_TRUE(fah_config_set_cookie_jar(cfg_h, jar_h) == FAH_OK);
  ASSERT_TRUE(fah_config_set_cookie_jar(cfg_h, 0) == FAH_OK);

  ASSERT_TRUE(fah_headers_create(&hdr_h) == FAH_OK);
  ASSERT_TRUE(fah_headers_add(hdr_h, "X-One", "1") == FAH_OK);
  ASSERT_TRUE(fah_headers_count(hdr_h, &sz) == FAH_OK && sz == 1u);
  ASSERT_TRUE(fah_headers_get(hdr_h, "x-one", buf, sizeof(buf), &sz, 0) ==
              FAH_OK);
  ASSERT_TRUE(fah_headers_remove(hdr_h, "X-One") == FAH_OK);
  ASSERT_TRUE(fah_headers_destroy(hdr_h) == FAH_OK);

  /* Raw client + OS factory client */
  ASSERT_TRUE(fah_client_create(cfg_h, 1, &cli_h) == FAH_OK);
  ASSERT_TRUE(fah_client_create(0, 0, &cli_def) == FAH_OK);
  sprintf(url, "http://127.0.0.1:%d", port);
  ASSERT_TRUE(fah_client_set_base_url(cli_h, url) == FAH_OK);
  ASSERT_TRUE(fah_client_destroy(cli_def) == FAH_OK);

  /* Redirect + Retry (/flaky) + Multipart + Auth + OAuth2 */
  ASSERT_TRUE(fah_request_create(&req_h) == FAH_OK);
  sprintf(url, "http://127.0.0.1:%d/redirect", port);
  ASSERT_TRUE(fah_request_set_target(req_h, 0, url) == FAH_OK);
  ASSERT_TRUE(fah_send(cli_h, req_h, &res_h) == FAH_OK);
  ASSERT_TRUE(fah_response_status(res_h, &st) == FAH_OK && st == 200);
  ASSERT_TRUE(fah_response_body_len(res_h, &sz) == FAH_OK && sz == 13u);
  ASSERT_TRUE(fah_response_copy_body(res_h, buf, sizeof(buf), &sz, 0) ==
              FAH_OK);
  ASSERT_TRUE(fah_response_header_count(res_h, &sz) == FAH_OK && sz >= 1u);
  ASSERT_TRUE(fah_response_header_at(res_h, 0u, kbuf, sizeof(kbuf), &sz, vbuf,
                                     sizeof(vbuf), &sz2, 0) == FAH_OK);
  ASSERT_TRUE(fah_response_get_header(res_h, "X-Fah-Server", buf, sizeof(buf),
                                      &sz, 0) == FAH_OK);
  ASSERT_TRUE(fah_response_save_file(res_h, "build_test_resp.tmp") == FAH_OK);
  (void)remove("build_test_resp.tmp");

  /* IPC roundtrips on live request & response */
  {
    fah_handle_t dup_req = 0, dup_res = 0;
    char pbuf[16];
    ASSERT_TRUE(fah_ipc_op(0, 0, "ping", 4u, pbuf, sizeof(pbuf), NULL) ==
                FAH_OK);
    ASSERT_TRUE(memcmp(pbuf, "ping", 4u) == 0);
    ASSERT_TRUE(fah_ipc_op(1, req_h, NULL, 0u, NULL, 0u, &dup_req) == FAH_OK);
    ASSERT_TRUE(fah_request_destroy(dup_req) == FAH_OK);
    ASSERT_TRUE(fah_ipc_op(2, res_h, NULL, 0u, NULL, 0u, &dup_res) == FAH_OK);
    ASSERT_TRUE(fah_response_destroy(dup_res) == FAH_OK);
  }
  ASSERT_TRUE(fah_response_destroy(res_h) == FAH_OK);

  /* Retry on /flaky (503 -> 200) */
  sprintf(url, "http://127.0.0.1:%d/flaky", port);
  ASSERT_TRUE(fah_request_set_target(req_h, 0, url) == FAH_OK);
  ASSERT_TRUE(fah_send(cli_h, req_h, &res_h) == FAH_OK);
  ASSERT_TRUE(fah_response_status(res_h, &st) == FAH_OK && st == 200);
  ASSERT_TRUE(fah_response_destroy(res_h) == FAH_OK);

  /* Multipart + Basic/Bearer + Cancel */
  sprintf(url, "http://127.0.0.1:%d/echo", port);
  ASSERT_TRUE(fah_request_set_target(req_h, 1, url) == FAH_OK);
  ASSERT_TRUE(fah_auth_bearer(req_h, "tok1") == FAH_OK);
  ASSERT_TRUE(fah_auth_basic(req_h, "dXNlcjpwYXNz") == FAH_OK);
  ASSERT_TRUE(fah_auth_basic_userpwd(req_h, "alice", "secret") == FAH_OK);
  ASSERT_TRUE(fah_request_add_part(req_h, "field1", "a.txt", "text/plain",
                                   "hello", 5u) == FAH_OK);
  ASSERT_TRUE(fah_request_add_part_header(req_h, "X-Part", "yes") == FAH_OK);
  ASSERT_TRUE(fah_request_flatten_parts(req_h) == FAH_OK);
  ASSERT_TRUE(fah_send(cli_h, req_h, &res_h) == FAH_OK);
  ASSERT_TRUE(fah_response_destroy(res_h) == FAH_OK);

  ASSERT_TRUE(fah_request_set_cancel(req_h, 1) == FAH_OK);
  ASSERT_TRUE(fah_request_set_cancel(req_h, 0) == FAH_OK);

  /* All 9 OAuth2 grants + build_auth_url + localhost_intercept */
  sprintf(url, "http://127.0.0.1:%d/oauth/token", port);
  for (i = 1; i <= 9; i++) {
    ASSERT_TRUE(fah_oauth2_grant(req_h, (fah_oauth2_grant_kind_t)i, url, "p1",
                                 "p2", "p3", "p4", "p5") == FAH_OK);
  }
  ASSERT_TRUE(fah_oauth2_build_auth_url("https://ex.com/auth", "cid", "code",
                                        "http://127.0.0.1/cb", "read", "st1",
                                        "ch", "S256", buf, sizeof(buf), &sz,
                                        0) == FAH_OK);
  {
    int oa_port = 0;
    char cbuf[32], sbuf[32];
    size_t cl = 0u, sl = 0u;
    ASSERT_TRUE(fah_mock_spawn_oauth_redirect_client(&oa_port) == 0);
    ASSERT_TRUE(fah_oauth2_localhost_intercept(
                    (unsigned short)oa_port,
                    "HTTP/1.1 200 OK\r\nContent-Length: 2\r\n\r\nOK", cbuf,
                    sizeof(cbuf), &cl, sbuf, sizeof(sbuf), &sl, 0) == FAH_OK);
    ASSERT_TRUE(cl == 6u && strncmp(cbuf, "auth77", 6u) == 0);
  }

  /* Multi-request & futures */
  {
    fah_handle_t r2 = 0, out_r = 0;
    fah_error_code_t xerr = FAH_OK;
    int rdy = 0, ec = 0;
    ASSERT_TRUE(fah_request_create(&r2) == FAH_OK);
    sprintf(url, "http://127.0.0.1:%d/hello", port);
    ASSERT_TRUE(fah_request_set_target(req_h, 0, url) == FAH_OK);
    ASSERT_TRUE(fah_request_set_target(r2, 0, url) == FAH_OK);
    ASSERT_TRUE(fah_multi_create(&multi_h) == FAH_OK);
    ASSERT_TRUE(fah_multi_add(multi_h, req_h) == FAH_OK);
    ASSERT_TRUE(fah_multi_add(multi_h, r2) == FAH_OK);
    ASSERT_TRUE(fah_multi_send(cli_h, multi_h, 0, futs, 2u, &sz) == FAH_OK &&
                sz == 2u);
    ASSERT_TRUE(fah_future_poll(futs[0], &rdy, &ec) == FAH_OK);
    ASSERT_TRUE(fah_future_await(futs[0], &xerr) == FAH_OK && xerr == FAH_OK);
    ASSERT_TRUE(fah_future_take_response(futs[0], &out_r) == FAH_OK);
    ASSERT_TRUE(fah_response_destroy(out_r) == FAH_OK);
    FAH_IGNORE_RC(fah_future_cancel(futs[1]));
    ASSERT_TRUE(fah_future_destroy(futs[0]) == FAH_OK);
    ASSERT_TRUE(fah_future_destroy(futs[1]) == FAH_OK);
    ASSERT_TRUE(fah_multi_destroy(multi_h) == FAH_OK);
    ASSERT_TRUE(fah_request_destroy(r2) == FAH_OK);
  }

  /* WebSockets & SSE (sync + async + FH7STM + IHWSOP + IHSSEO + FHC4*) */
  {
    fah_handle_t loop_h = 0, ws_req = 0, sse_req = 0, qh = 0;
    int tid = 0, bk = 0, iop, iarg, imode = 0, ilen = 2;
    int iping[2] = {72, 73};

    ASSERT_TRUE(fah_poll_queue_create(&qh) == FAH_OK);
    ASSERT_TRUE(fah_loop_create(0, &loop_h) == FAH_OK);
    ASSERT_TRUE(fah_loop_get_backend(loop_h, &bk) == FAH_OK);
    ASSERT_TRUE(fah_loop_add_timer(loop_h, 5L, test_timer_cb, 1, &tid) ==
                FAH_OK);
    ASSERT_TRUE(fah_loop_cancel_timer(loop_h, tid) == FAH_OK);
    ASSERT_TRUE(fah_loop_add_timer(loop_h, 1L, test_timer_cb, 1, &tid) ==
                FAH_OK);
    ASSERT_TRUE(fah_loop_drive(loop_h, 0) == FAH_OK);
    ASSERT_TRUE(fah_loop_drive(loop_h, 3) == FAH_OK);
    ASSERT_TRUE(fah_loop_drive(loop_h, 1) == FAH_OK);

    ASSERT_TRUE(fah_request_create(&ws_req) == FAH_OK);
    sprintf(url, "ws://127.0.0.1:%d/ws?echo=1", port);
    ASSERT_TRUE(fah_request_set_target(ws_req, 0, url) == FAH_OK);
    iop = 0;
    iarg = 1;
    ASSERT_TRUE(FAH_FC_GLOBAL(ihwsop, IHWSOP)(&cli_h, &ws_req, &iop, &iarg,
                                              &imode, iping, &ilen, &qh) == 0);
    ASSERT_TRUE(fah_ws_async_connect(cli_h, loop_h, ws_req, test_ws_msg,
                                     test_ws_err, test_ws_cls, 1,
                                     qh) == FAH_OK);
    iop = 2;
    iarg = 1;
    FAH_IGNORE_RC(FAH_FC_GLOBAL(ihwsop, IHWSOP)(&cli_h, &ws_req, &iop, &iarg,
                                                &imode, iping, &ilen, &qh));
    FAH_IGNORE_RC(fah_ws_send(ws_req, 1, "hi", 2u, 1));
    iop = 3;
    iarg = 1000;
    FAH_IGNORE_RC(FAH_FC_GLOBAL(ihwsop, IHWSOP)(&cli_h, &ws_req, &iop, &iarg,
                                                &imode, iping, &ilen, &qh));
    iop = 4;
    ASSERT_TRUE(FAH_FC_GLOBAL(ihwsop, IHWSOP)(&cli_h, &ws_req, &iop, &iarg,
                                              &imode, iping, &ilen, &qh) == 0);

    /* Sync WS via IHWSOP op=1 */
    sprintf(url, "ws://127.0.0.1:%d/ws", port);
    ASSERT_TRUE(fah_request_set_target(ws_req, 0, url) == FAH_OK);
    iop = 0;
    iarg = 1;
    ASSERT_TRUE(FAH_FC_GLOBAL(ihwsop, IHWSOP)(&cli_h, &ws_req, &iop, &iarg,
                                              &imode, iping, &ilen, &qh) == 0);
    iop = 1;
    ASSERT_TRUE(FAH_FC_GLOBAL(ihwsop, IHWSOP)(&cli_h, &ws_req, &iop, &iarg,
                                              &imode, iping, &ilen, &qh) == 0);
    iop = 4;
    FAH_IGNORE_RC(FAH_FC_GLOBAL(ihwsop, IHWSOP)(&cli_h, &ws_req, &iop, &iarg,
                                                &imode, iping, &ilen, &qh));

    /* Sync + Async SSE via IHSSEO */
    ASSERT_TRUE(fah_request_create(&sse_req) == FAH_OK);
    sprintf(url, "http://127.0.0.1:%d/sse", port);
    ASSERT_TRUE(fah_request_set_target(sse_req, 0, url) == FAH_OK);
    iop = 0;
    iarg = 50;
    ASSERT_TRUE(
        FAH_FC_GLOBAL(ihsseo, IHSSEO)(&cli_h, &sse_req, &iop, &iarg, &qh) == 0);
    iop = 1;
    ASSERT_TRUE(
        FAH_FC_GLOBAL(ihsseo, IHSSEO)(&cli_h, &sse_req, &iop, &iarg, &qh) == 0);
    iop = 2;
    ASSERT_TRUE(
        FAH_FC_GLOBAL(ihsseo, IHSSEO)(&cli_h, &sse_req, &iop, &iarg, &qh) == 0);

    ASSERT_TRUE(fah_sse_init(sse_req, "e0", 50) == FAH_OK);
    FAH_IGNORE_RC(fah_sse_async_register(cli_h, loop_h, sse_req, test_sse_evt,
                                         test_sse_err, test_sse_cls, 1, qh));
    ASSERT_TRUE(fah_sse_free(sse_req) == FAH_OK);

    ASSERT_TRUE(fah_request_destroy(ws_req) == FAH_OK);
    ASSERT_TRUE(fah_request_destroy(sse_req) == FAH_OK);
    ASSERT_TRUE(fah_loop_destroy(loop_h) == FAH_OK);
    ASSERT_TRUE(fah_poll_queue_destroy(qh) == FAH_OK);
  }

  /* Concurrency Modalities: Time, Mutex, Cond, Pool, TLS, Coro, Bus/Actor, CMP
   */
  {
    abstract_http_int64_t now_ms = 0;
    fah_handle_t mtx = 0, cnd = 0, pool = 0, tls = 0, coro = 0, bus = 0,
                 act = 0, cmp_b = 0;
    int tv = 123, done = 0, proc = 0, emod = 0;

    ASSERT_TRUE(fah_time_ms(&now_ms) == FAH_OK && now_ms > 0);
    ASSERT_TRUE(fah_mutex_op(&mtx, 0) == FAH_OK);
    ASSERT_TRUE(fah_mutex_op(&mtx, 1) == FAH_OK);
    ASSERT_TRUE(fah_mutex_op(&mtx, 2) == FAH_OK);
    ASSERT_TRUE(fah_mutex_op(&mtx, 3) == FAH_OK);

    ASSERT_TRUE(fah_cond_op(&cnd, 0) == FAH_OK);
    ASSERT_TRUE(fah_cond_op(&cnd, 1) == FAH_OK);
    ASSERT_TRUE(fah_cond_op(&cnd, 2) == FAH_OK);
    ASSERT_TRUE(fah_cond_op(&cnd, 3) == FAH_OK);

    ASSERT_TRUE(fah_pool_create(2u, &pool) == FAH_OK);
    ASSERT_TRUE(fah_pool_submit(pool, test_task_cb, 5) == FAH_OK);
    ASSERT_TRUE(fah_pool_destroy(pool) == FAH_OK);

    ASSERT_TRUE(fah_tls_op(&tls, 0, NULL) == FAH_OK);
    ASSERT_TRUE(fah_tls_op(&tls, 1, &tv) == FAH_OK);
    tv = 456;
    ASSERT_TRUE(fah_tls_op(&tls, 1, &tv) == FAH_OK);
    tv = 0;
    ASSERT_TRUE(fah_tls_op(&tls, 2, &tv) == FAH_OK && tv == 456);
    ASSERT_TRUE(fah_tls_op(&tls, 3, NULL) == FAH_OK);

    ASSERT_TRUE(fah_coro_create(0u, test_task_cb, 77, &coro) == FAH_OK);
    ASSERT_TRUE(fah_coro_op(coro, 0, NULL) == FAH_OK);
    ASSERT_TRUE(fah_coro_op(coro, 2, &done) == FAH_OK && done == 0);
    ASSERT_TRUE(fah_coro_op(coro, 0, NULL) == FAH_OK);
    ASSERT_TRUE(fah_coro_op(coro, 2, &done) == FAH_OK && done == 1);
    ASSERT_TRUE(fah_coro_op(coro, 3, NULL) == FAH_OK);

    ASSERT_TRUE(fah_bus_create(&bus) == FAH_OK);
    ASSERT_TRUE(fah_actor_spawn(bus, "worker1", test_actor_cb, 10, &act) ==
                FAH_OK);
    ASSERT_TRUE(fah_actor_get_name(act, buf, sizeof(buf), &sz, 0) == FAH_OK);
    ASSERT_TRUE(fah_actor_send_int(bus, act, act, 1000, 32) == FAH_OK);
    ASSERT_TRUE(fah_bus_process(bus, &proc) == FAH_OK && proc == 1);
    ASSERT_TRUE(fah_actor_destroy(act) == FAH_OK);
    ASSERT_TRUE(fah_bus_destroy(bus) == FAH_OK);

    ASSERT_TRUE(fah_cmp_op(0, 1, 1u, 2u, 0, &emod, NULL) == FAH_OK);
    ASSERT_TRUE(fah_cmp_op(1, 1, 1u, 4u, cfg_h, NULL, NULL) == FAH_OK);
    ASSERT_TRUE(fah_cmp_op(2, 0, 0u, 0u, req_h, NULL, &cmp_b) == FAH_OK);
    ASSERT_TRUE(fah_cmp_op(3, 0, 0u, 0u, cmp_b, NULL, NULL) == FAH_OK);
    ASSERT_TRUE(fah_cmp_op(4, 0, 0u, 0u, cmp_b, NULL, NULL) == FAH_OK);
  }

  /* Exercise remaining FH7* Fortran entry dispatchers */
  {
    int op, a1, a2, a3, a4, h1 = 0, h2 = 0, n1 = 0, n2 = 0;
    char s1[64], s2[64];

    for (op = 1; op <= 6; op++) {
      a1 = 0;
      a2 = 1;
      a3 = 1;
      a4 = 1;
      FAH_FC_GLOBAL(fh7cfg, FH7CFG)(&cfg_h, &op, &a1, &a2, &a3, &a4, &rc_i);
    }
    for (op = 0; op <= 9; op++) {
      FAH_FC_GLOBAL(fh7cfs, FH7CFS)((op == 9) ? &cli_h : &cfg_h, &op,
                                    "http://127.0.0.1  ", &rc_i, 18u);
    }

    op = 1;
    FAH_FC_GLOBAL(fh7hcj, FH7HCJ)(&h1, &op, "K", s2, &n1, &rc_i, 1u, 64u);
    op = 3;
    FAH_FC_GLOBAL(fh7hcj, FH7HCJ)(&h1, &op, "K", "V", &n1, &rc_i, 1u, 1u);
    op = 4;
    FAH_FC_GLOBAL(fh7hcj, FH7HCJ)(&h1, &op, "K", s2, &n1, &rc_i, 1u, 64u);
    op = 6;
    FAH_FC_GLOBAL(fh7hcj, FH7HCJ)(&h1, &op, "K", s2, &n1, &rc_i, 1u, 64u);
    op = 5;
    FAH_FC_GLOBAL(fh7hcj, FH7HCJ)(&h1, &op, "K", s2, &n1, &rc_i, 1u, 64u);
    op = 2;
    FAH_FC_GLOBAL(fh7hcj, FH7HCJ)(&h1, &op, "K", s2, &n1, &rc_i, 1u, 64u);

    op = 11;
    FAH_FC_GLOBAL(fh7hcj, FH7HCJ)(&h1, &op, "K", s2, &n1, &rc_i, 1u, 64u);
    op = 13;
    FAH_FC_GLOBAL(fh7hcj, FH7HCJ)(&h1, &op, "C", "W", &n1, &rc_i, 1u, 1u);
    op = 14;
    FAH_FC_GLOBAL(fh7hcj, FH7HCJ)(&h1, &op, "C", s2, &n1, &rc_i, 1u, 64u);
    op = 15;
    FAH_FC_GLOBAL(fh7hcj, FH7HCJ)(&h1, &op, "D=Z", s2, &n1, &rc_i, 3u, 64u);
    op = 16;
    FAH_FC_GLOBAL(fh7hcj, FH7HCJ)(&h1, &op, " ", s2, &n1, &rc_i, 1u, 64u);
    op = 99;
    FAH_FC_GLOBAL(fh7hcj, FH7HCJ)(&h1, &op, " ", s2, &n1, &rc_i, 1u, 64u);
    op = 12;
    FAH_FC_GLOBAL(fh7hcj, FH7HCJ)(&h1, &op, " ", s2, &n1, &rc_i, 1u, 64u);

    for (op = 1; op <= 12; op++) {
      a1 = (op == 1) ? 0 : 2;
      FAH_FC_GLOBAL(fh7req, FH7REQ)(&req_h, &op, &a1, "http://127.0.0.1/hello",
                                    "V", "text/plain", "BD", &rc_i, 22u, 1u,
                                    10u, 2u);
    }

    op = 10;
    FAH_FC_GLOBAL(fh7oa2, FH7OA2)(&req_h, &op, "https://e.io/a", "cid", "code",
                                  " ", " ", " ", s1, &n1, s2, &n2, &rc_i, 14u,
                                  3u, 4u, 1u, 1u, 1u, 64u, 64u);
    op = 99;
    FAH_FC_GLOBAL(fh7oa2, FH7OA2)(&req_h, &op, "x", "y", " ", " ", " ", " ", s1,
                                  &n1, s2, &n2, &rc_i, 1u, 1u, 1u, 1u, 1u, 1u,
                                  64u, 64u);

    for (op = 1; op <= 14; op++) {
      a1 = 1;
      FAH_FC_GLOBAL(fh7stm, FH7STM)(
          &cli_h, &h1, &req_h, &op, &a1, "chat", (void (*)(void))test_ws_msg,
          (void (*)(void))test_ws_err, (void (*)(void))test_ws_cls, &a1, &rc_i,
          4u);
    }

    /* FH7MOD ops */
    {
      static const int mod_ops[] = {1,  2,  4,  5,  3,  6,  7,  10, 11,
                                    12, 13, 14, 15, 16, 17, 20, 21, 22,
                                    23, 24, 25, 26, 30, 31, 99};
      size_t mi;
      for (mi = 0u; mi < sizeof(mod_ops) / sizeof(mod_ops[0]); mi++) {
        op = mod_ops[mi];
        a1 = 0;
        a2 = 1;
        a3 = 2;
        if (op == 12)
          a1 = 1;
        memcpy(s1, "actor_ipc   ", 12u);
        FAH_FC_GLOBAL(fh7mod, FH7MOD)(&op, &h1, &h2, &a1, &a2, &a3, s1,
                                      (void (*)(void))test_task_cb, &n1, &rc_i,
                                      12u);
        if (op == 10 || op == 11 || op == 15 || op == 17) {
          a1 = 3;
          FAH_FC_GLOBAL(fh7mod, FH7MOD)(&op, &h1, &h2, &a1, &a2, &a3, s1,
                                        (void (*)(void))test_task_cb, &n1,
                                        &rc_i, 12u);
        }
      }
    }
  }

  /* Deterministic OOM, negative-input, and full-table rollback sweeps */
  {
    int step;
    int zero = 0, one = 1, neg1 = -1, two = 2, four = 4, eleven = 11, bad = 99;
    int bad_ic[2] = {300, 65};
    int ok_ic[2] = {65, 66};
    int ibuf[16], n1 = 0, n2 = 0, cat = 0, rty = 0;
    char sbuf1[64], sbuf2[64], tiny_ev[1];
    fah_handle_t th = 0, th2 = 0, c8 = 0, res_live = 0, bus_live = 0;
    fah_handle_t mtx_v = 0, cnd_v = 0, pool_v = 0, tls_v = 0, coro_v = 0;
    fah_handle_t q_v = 0, loop_v = 0, mh_v = 0, fut2[2];
    fah_handle_t fill[FAH_HANDLE_MAX_SLOTS];
    int nfill = 0, fill_dummy = 7;
    const void *bptr = NULL;
    fah_error_code_t xerr = FAH_OK;

    /* Poll queue negative & truncation paths */
    ASSERT_TRUE(fah_poll_queue_create(&q_v) == FAH_OK);
    ASSERT_TRUE(fah_poll_queue_create(NULL) == FAH_ERR_INVAL);
    ASSERT_TRUE(fah_poll_queue_destroy(0) == FAH_ERR_NULL_HANDLE);
    ASSERT_TRUE(fah_poll_queue_push(q_v, FAH_POLL_CHUNK, 0, NULL, NULL, 5u) ==
                FAH_ERR_INVALID_SLICE);
    ASSERT_TRUE(fah_poll_queue_push(0, FAH_POLL_CHUNK, 0, NULL, "a", 1u) ==
                FAH_ERR_NULL_HANDLE);
    ASSERT_TRUE(fah_poll_queue_pop(q_v, NULL, &n1, sbuf1, 8u, &sz, sbuf2, 8u,
                                   &sz2) == FAH_ERR_INVAL);
    ASSERT_TRUE(fah_poll_queue_pop(0, &n1, &n2, sbuf1, 8u, &sz, sbuf2, 8u,
                                   &sz2) == FAH_ERR_NULL_HANDLE);
    ASSERT_TRUE(fah_poll_queue_push(q_v, FAH_POLL_SSE_EVT, 0, "long_evt", "d",
                                    1u) == FAH_OK);
    ASSERT_TRUE(fah_poll_queue_pop(q_v, &n1, &n2, tiny_ev, sizeof(tiny_ev), &sz,
                                   sbuf2, sizeof(sbuf2),
                                   &sz2) == FAH_ERR_BUF_TOO_SMALL);

    /* Config & Client negative + 8-slot string OOM sweeps */
    ASSERT_TRUE(fah_config_create(NULL) == FAH_ERR_INVAL);
    fah_oom_set_fail_after(1);
    ASSERT_TRUE(fah_config_create(&th) == FAH_ERR_NOMEM);
    ASSERT_TRUE(fah_config_destroy(0) == FAH_ERR_NULL_HANDLE);
    ASSERT_TRUE(fah_config_set_timeouts(cfg_h, -1L, 0L, 0L, 0L) ==
                FAH_ERR_INVAL);
    ASSERT_TRUE(fah_config_set_timeouts(0, 1L, 0L, 0L, 0L) ==
                FAH_ERR_NULL_HANDLE);
    ASSERT_TRUE(fah_config_set_tls_flags(0, 0, 0, 0, 0) == FAH_ERR_NULL_HANDLE);
    ASSERT_TRUE(fah_config_set_versions_and_retry(0, 0u, 0u, 1, 0) ==
                FAH_ERR_NULL_HANDLE);
    ASSERT_TRUE(fah_config_set_modality_and_pool(cfg_h, 99, 0u, 0u, 0, 0L,
                                                 0u) == FAH_ERR_INVAL);
    ASSERT_TRUE(fah_config_set_modality_and_pool(0, 0, 0u, 0u, 0, 0L, 0u) ==
                FAH_ERR_NULL_HANDLE);
    ASSERT_TRUE(fah_config_set_strings(0, "a", NULL, NULL, NULL, NULL, NULL,
                                       NULL, NULL) == FAH_ERR_NULL_HANDLE);
    ASSERT_TRUE(fah_config_set_cookie_jar(0, 0) == FAH_ERR_NULL_HANDLE);
    ASSERT_TRUE(fah_config_set_cookie_jar(cfg_h, 999999) ==
                FAH_ERR_STALE_HANDLE);

    ASSERT_TRUE(fah_config_create(&c8) == FAH_OK);
    for (step = 1; step <= 8; step++) {
      fah_oom_set_fail_after(step);
      ASSERT_TRUE(fah_config_set_strings(c8, "ua", "http://127.0.0.1:8080", "u",
                                         "p", "ca", "cc", "ck",
                                         "pin") == FAH_ERR_NOMEM);
    }
    fah_oom_reset();
    ASSERT_TRUE(fah_config_set_strings(c8, "ua", "http://127.0.0.1:8080", "u",
                                       "p", "ca", "cc", "ck", "pin") == FAH_OK);
    for (step = 1; step <= 9; step++) {
      fah_oom_set_fail_after(step);
      ASSERT_TRUE(fah_client_create(c8, 1, &th) == FAH_ERR_NOMEM);
    }
    fah_oom_reset();
    ASSERT_TRUE(fah_config_set_strings(c8, NULL, "ftp://bad", NULL, NULL, NULL,
                                       NULL, NULL, NULL) == FAH_OK);
    ASSERT_TRUE(fah_client_create(c8, 1, &th) != FAH_OK);
    ASSERT_TRUE(fah_config_destroy(c8) == FAH_OK);

    ASSERT_TRUE(fah_client_create(0, 1, NULL) == FAH_ERR_INVAL);
    ASSERT_TRUE(fah_client_create(999999, 1, &th) == FAH_ERR_STALE_HANDLE);
    ASSERT_TRUE(fah_client_set_base_url(cli_h, NULL) == FAH_ERR_INVAL);
    ASSERT_TRUE(fah_client_set_base_url(0, "http://x") == FAH_ERR_NULL_HANDLE);
    ASSERT_TRUE(fah_client_destroy(0) == FAH_ERR_NULL_HANDLE);

    /* Headers & Cookie Jar negative paths */
    ASSERT_TRUE(fah_headers_create(NULL) == FAH_ERR_INVAL);
    fah_oom_set_fail_after(1);
    ASSERT_TRUE(fah_headers_create(&th) == FAH_ERR_NOMEM);
    ASSERT_TRUE(fah_headers_destroy(0) == FAH_ERR_NULL_HANDLE);
    ASSERT_TRUE(fah_headers_create(&th) == FAH_OK);
    ASSERT_TRUE(fah_headers_add(th, NULL, "v") == FAH_ERR_INVAL);
    ASSERT_TRUE(fah_headers_add(0, "k", "v") == FAH_ERR_NULL_HANDLE);
    ASSERT_TRUE(fah_headers_get(th, NULL, buf, sizeof(buf), &sz, 0) ==
                FAH_ERR_INVAL);
    ASSERT_TRUE(fah_headers_get(0, "k", buf, sizeof(buf), &sz, 0) ==
                FAH_ERR_NULL_HANDLE);
    ASSERT_TRUE(fah_headers_get(th, "NoSuch", buf, sizeof(buf), &sz, 0) !=
                FAH_OK);
    ASSERT_TRUE(fah_headers_remove(th, NULL) == FAH_ERR_INVAL);
    ASSERT_TRUE(fah_headers_remove(0, "k") == FAH_ERR_NULL_HANDLE);
    ASSERT_TRUE(fah_headers_count(th, NULL) == FAH_ERR_INVAL);
    ASSERT_TRUE(fah_headers_count(0, &sz) == FAH_ERR_NULL_HANDLE);
    ASSERT_TRUE(fah_headers_destroy(th) == FAH_OK);

    ASSERT_TRUE(fah_cookie_jar_create(NULL) == FAH_ERR_INVAL);
    fah_oom_set_fail_after(1);
    ASSERT_TRUE(fah_cookie_jar_create(&th) == FAH_ERR_NOMEM);
    ASSERT_TRUE(fah_cookie_jar_destroy(0) == FAH_ERR_NULL_HANDLE);
    ASSERT_TRUE(fah_cookie_jar_create(&th) == FAH_OK);
    /* Empty jar build_hdr exercises built == NULL path */
    ASSERT_TRUE(fah_cookie_jar_build_hdr(th, "http://127.0.0.1/", buf,
                                         sizeof(buf), &sz, 0) == FAH_OK);
    ASSERT_TRUE(fah_cookie_jar_build_hdr(0, "http://127.0.0.1/", buf,
                                         sizeof(buf), &sz,
                                         0) == FAH_ERR_NULL_HANDLE);
    ASSERT_TRUE(fah_cookie_jar_set(th, NULL, "v") == FAH_ERR_INVAL);
    ASSERT_TRUE(fah_cookie_jar_set(0, "k", "v") == FAH_ERR_NULL_HANDLE);
    ASSERT_TRUE(fah_cookie_jar_get(th, NULL, buf, sizeof(buf), &sz, 0) ==
                FAH_ERR_INVAL);
    ASSERT_TRUE(fah_cookie_jar_get(0, "k", buf, sizeof(buf), &sz, 0) ==
                FAH_ERR_NULL_HANDLE);
    ASSERT_TRUE(fah_cookie_jar_get(th, "NoSuch", buf, sizeof(buf), &sz, 0) !=
                FAH_OK);
    ASSERT_TRUE(fah_cookie_jar_parse_set(th, NULL) == FAH_ERR_INVAL);
    ASSERT_TRUE(fah_cookie_jar_parse_set(0, "a=b") == FAH_ERR_NULL_HANDLE);
    ASSERT_TRUE(fah_cookie_jar_destroy(th) == FAH_OK);

    /* Request, Auth, OAuth2, Send & Response negative paths */
    ASSERT_TRUE(fah_request_create(NULL) == FAH_ERR_INVAL);
    fah_oom_set_fail_after(1);
    ASSERT_TRUE(fah_request_create(&th) == FAH_ERR_NOMEM);
    ASSERT_TRUE(fah_request_destroy(0) == FAH_ERR_NULL_HANDLE);
    ASSERT_TRUE(fah_request_set_target(req_h, -1, "http://x") == FAH_ERR_INVAL);
    ASSERT_TRUE(fah_request_set_target(0, 0, "http://x") ==
                FAH_ERR_NULL_HANDLE);
    fah_oom_set_fail_after(1);
    ASSERT_TRUE(fah_request_set_target(req_h, 0, "http://x") == FAH_ERR_NOMEM);
    ASSERT_TRUE(fah_request_add_header(req_h, NULL, "v") == FAH_ERR_INVAL);
    ASSERT_TRUE(fah_request_add_header(0, "k", "v") == FAH_ERR_NULL_HANDLE);
    ASSERT_TRUE(fah_request_set_body(req_h, NULL, 5u) == FAH_ERR_INVALID_SLICE);
    ASSERT_TRUE(fah_request_set_body(0, "a", 1u) == FAH_ERR_NULL_HANDLE);
    fah_oom_set_fail_after(1);
    ASSERT_TRUE(fah_request_set_body(req_h, "abc", 3u) == FAH_ERR_NOMEM);
    /* Overwrite non-owned req.body (from OAuth2) with fah_request_set_body */
    ASSERT_TRUE(fah_oauth2_grant(req_h, FAH_OAUTH2_CLIENT_CREDENTIALS,
                                 "http://127.0.0.1/t", "c", "s", NULL, NULL,
                                 NULL) == FAH_OK);
    ASSERT_TRUE(fah_request_set_body(req_h, "owned", 5u) == FAH_OK);
    /* And overwrite owned_body via fah_oauth2_grant */
    ASSERT_TRUE(fah_oauth2_grant(req_h, FAH_OAUTH2_CLIENT_CREDENTIALS,
                                 "http://127.0.0.1/t", "c", "s", NULL, NULL,
                                 NULL) == FAH_OK);
    /* And overwrite owned_body via fah_request_flatten_parts */
    ASSERT_TRUE(fah_request_set_body(req_h, "owned2", 6u) == FAH_OK);
    ASSERT_TRUE(fah_request_add_part(req_h, "p", NULL, NULL, "x", 1u) ==
                FAH_OK);
    ASSERT_TRUE(fah_request_flatten_parts(req_h) == FAH_OK);
    ASSERT_TRUE(fah_request_set_body_cfi(req_h, NULL) ==
                FAH_ERR_CFI_DESCRIPTOR);
    ASSERT_TRUE(fah_request_add_part(req_h, NULL, NULL, NULL, "x", 1u) ==
                FAH_ERR_INVAL);
    ASSERT_TRUE(fah_request_add_part(0, "f", NULL, NULL, "x", 1u) ==
                FAH_ERR_NULL_HANDLE);
    ASSERT_TRUE(fah_request_add_part_header(req_h, NULL, "v") == FAH_ERR_INVAL);
    ASSERT_TRUE(fah_request_add_part_header(0, "k", "v") ==
                FAH_ERR_NULL_HANDLE);
    ASSERT_TRUE(fah_request_flatten_parts(0) == FAH_ERR_NULL_HANDLE);
    ASSERT_TRUE(fah_request_set_callbacks(0, NULL, 0, NULL, 0, 0u, NULL, 0,
                                          0) == FAH_ERR_NULL_HANDLE);
    ASSERT_TRUE(fah_request_set_callbacks(req_h, NULL, 0, NULL, 0, 0u, NULL, 0,
                                          999999) == FAH_ERR_STALE_HANDLE);
    ASSERT_TRUE(fah_request_set_cancel(0, 1) == FAH_ERR_NULL_HANDLE);

    ASSERT_TRUE(fah_auth_bearer(req_h, NULL) == FAH_ERR_INVAL);
    ASSERT_TRUE(fah_auth_bearer(0, "t") == FAH_ERR_NULL_HANDLE);
    ASSERT_TRUE(fah_auth_basic(req_h, NULL) == FAH_ERR_INVAL);
    ASSERT_TRUE(fah_auth_basic(0, "t") == FAH_ERR_NULL_HANDLE);
    ASSERT_TRUE(fah_auth_basic_userpwd(req_h, NULL, "p") == FAH_ERR_INVAL);
    ASSERT_TRUE(fah_auth_basic_userpwd(0, "u", "p") == FAH_ERR_NULL_HANDLE);

    ASSERT_TRUE(fah_oauth2_grant(req_h, FAH_OAUTH2_PASSWORD, NULL, "u", "p",
                                 NULL, NULL, NULL) == FAH_ERR_INVAL);
    ASSERT_TRUE(fah_oauth2_grant(0, FAH_OAUTH2_CLIENT_CREDENTIALS, "http://x",
                                 "c", NULL, NULL, NULL,
                                 NULL) == FAH_ERR_NULL_HANDLE);
    ASSERT_TRUE(fah_oauth2_grant(req_h, FAH_OAUTH2_PASSWORD, "http://x", "u",
                                 NULL, NULL, NULL, NULL) == FAH_ERR_INVAL);
    ASSERT_TRUE(fah_oauth2_grant(req_h, FAH_OAUTH2_DEVICE_TOKEN, "http://x",
                                 "c", NULL, NULL, NULL, NULL) == FAH_ERR_INVAL);
    ASSERT_TRUE(fah_oauth2_grant(req_h, (fah_oauth2_grant_kind_t)99, "http://x",
                                 "c", NULL, NULL, NULL, NULL) == FAH_ERR_INVAL);
    ASSERT_TRUE(fah_oauth2_build_auth_url(NULL, "c", "code", NULL, NULL, NULL,
                                          NULL, NULL, buf, sizeof(buf), &sz,
                                          0) == FAH_ERR_INVAL);
    ASSERT_TRUE(fah_oauth2_localhost_intercept(0, NULL, sbuf1, 16u, &sz, sbuf2,
                                               16u, &sz2, 0) == FAH_ERR_INVAL);
    ASSERT_TRUE(fah_oauth2_localhost_intercept((unsigned short)port, "OK",
                                               sbuf1, 16u, &sz, sbuf2, 16u,
                                               &sz2, 0) != FAH_OK);

    ASSERT_TRUE(fah_send(cli_h, req_h, NULL) == FAH_ERR_INVAL);
    ASSERT_TRUE(fah_send(0, req_h, &th) == FAH_ERR_NULL_HANDLE);
    ASSERT_TRUE(fah_send(cli_h, 0, &th) == FAH_ERR_NULL_HANDLE);
    ASSERT_TRUE(fah_request_set_target(req_h, 0, "badscheme://invalid") ==
                FAH_OK);
    ASSERT_TRUE(fah_send(cli_h, req_h, &th) != FAH_OK);

    sprintf(url, "http://127.0.0.1:%d/hello", port);
    ASSERT_TRUE(fah_request_create(&th2) == FAH_OK);
    ASSERT_TRUE(fah_request_set_target(th2, 0, url) == FAH_OK);
    ASSERT_TRUE(fah_send(cli_h, th2, &res_live) == FAH_OK);

    ASSERT_TRUE(fah_response_status(res_live, NULL) == FAH_ERR_INVAL);
    ASSERT_TRUE(fah_response_status(0, &st) == FAH_ERR_NULL_HANDLE);
    ASSERT_TRUE(fah_response_body_len(res_live, NULL) == FAH_ERR_INVAL);
    ASSERT_TRUE(fah_response_body_len(0, &sz) == FAH_ERR_NULL_HANDLE);
    ASSERT_TRUE(fah_response_copy_body(0, buf, sizeof(buf), &sz, 0) ==
                FAH_ERR_NULL_HANDLE);
    ASSERT_TRUE(fah_response_body_ptr(res_live, NULL, &sz) == FAH_ERR_INVAL);
    ASSERT_TRUE(fah_response_body_ptr(0, &bptr, &sz) == FAH_ERR_NULL_HANDLE);
    ASSERT_TRUE(fah_response_body_cfi(0, NULL) == FAH_ERR_NULL_HANDLE);
    ASSERT_TRUE(fah_response_get_header(res_live, NULL, buf, sizeof(buf), &sz,
                                        0) == FAH_ERR_INVAL);
    ASSERT_TRUE(fah_response_get_header(0, "k", buf, sizeof(buf), &sz, 0) ==
                FAH_ERR_NULL_HANDLE);
    ASSERT_TRUE(fah_response_get_header(res_live, "NoSuch", buf, sizeof(buf),
                                        &sz, 0) != FAH_OK);
    ASSERT_TRUE(fah_response_header_count(res_live, NULL) == FAH_ERR_INVAL);
    ASSERT_TRUE(fah_response_header_count(0, &sz) == FAH_ERR_NULL_HANDLE);
    ASSERT_TRUE(fah_response_header_at(0, 0u, kbuf, sizeof(kbuf), &sz, vbuf,
                                       sizeof(vbuf), &sz2,
                                       0) == FAH_ERR_NULL_HANDLE);
    ASSERT_TRUE(fah_response_header_at(res_live, 999u, kbuf, sizeof(kbuf), &sz,
                                       vbuf, sizeof(vbuf), &sz2,
                                       0) == FAH_ERR_INVAL);
    ASSERT_TRUE(fah_response_save_file(res_live, NULL) == FAH_ERR_INVAL);
    ASSERT_TRUE(fah_response_save_file(0, "x") == FAH_ERR_NULL_HANDLE);
    ASSERT_TRUE(fah_response_destroy(0) == FAH_ERR_NULL_HANDLE);

    /* Multi & Futures negative + OOM + fail_fast paths */
    ASSERT_TRUE(fah_multi_create(NULL) == FAH_ERR_INVAL);
    fah_oom_set_fail_after(1);
    ASSERT_TRUE(fah_multi_create(&th) == FAH_ERR_NOMEM);
    ASSERT_TRUE(fah_multi_create(&mh_v) == FAH_OK);
    ASSERT_TRUE(fah_multi_add(0, th2) == FAH_ERR_NULL_HANDLE);
    ASSERT_TRUE(fah_multi_add(mh_v, 0) == FAH_ERR_NULL_HANDLE);
    ASSERT_TRUE(fah_multi_send(cli_h, mh_v, 0, NULL, 2u, &sz) == FAH_ERR_INVAL);
    ASSERT_TRUE(fah_multi_send(0, mh_v, 0, fut2, 2u, &sz) ==
                FAH_ERR_NULL_HANDLE);
    ASSERT_TRUE(fah_multi_send(cli_h, 0, 0, fut2, 2u, &sz) ==
                FAH_ERR_NULL_HANDLE);
    ASSERT_TRUE(fah_multi_send(cli_h, mh_v, 0, fut2, 2u, &sz) == FAH_ERR_INVAL);
    ASSERT_TRUE(fah_multi_add(mh_v, th2) == FAH_OK);
    ASSERT_TRUE(fah_multi_add(mh_v, req_h) == FAH_OK);
    ASSERT_TRUE(fah_multi_send(cli_h, mh_v, 0, fut2, 1u, &sz) ==
                FAH_ERR_BUF_TOO_SMALL);
    fah_oom_set_fail_after(1);
    ASSERT_TRUE(fah_multi_send(cli_h, mh_v, 0, fut2, 2u, &sz) == FAH_ERR_NOMEM);
    fah_oom_set_fail_after(3);
    ASSERT_TRUE(fah_multi_send(cli_h, mh_v, 0, fut2, 2u, &sz) == FAH_ERR_NOMEM);
    fah_oom_reset();
    /* fail_fast=1: slot 0 (th2 -> /hello) succeeds, slot 1 (req_h -> :1/bad)
     * fails -> exercises send_multi failure cleanup with non-NULL response! */
    ASSERT_TRUE(fah_multi_send(cli_h, mh_v, 1, fut2, 2u, &sz) != FAH_OK);
    /* fail_fast=0: slot 0 has response, slot 1 has NULL response */
    ASSERT_TRUE(fah_multi_send(cli_h, mh_v, 0, fut2, 2u, &sz) == FAH_OK);
    ASSERT_TRUE(fah_future_await(fut2[0], NULL) == FAH_ERR_INVAL);
    ASSERT_TRUE(fah_future_await(0, &xerr) == FAH_ERR_NULL_HANDLE);
    ASSERT_TRUE(fah_future_cancel(0) == FAH_ERR_NULL_HANDLE);
    ASSERT_TRUE(fah_future_poll(fut2[0], NULL, &n1) == FAH_ERR_INVAL);
    ASSERT_TRUE(fah_future_poll(0, &n1, &n2) == FAH_ERR_NULL_HANDLE);
    ASSERT_TRUE(fah_future_take_response(fut2[0], NULL) == FAH_ERR_INVAL);
    ASSERT_TRUE(fah_future_take_response(0, &th) == FAH_ERR_NULL_HANDLE);
    ASSERT_TRUE(fah_future_take_response(fut2[1], &th) == FAH_ERR_INVAL);
    /*Destroy fut2[0] WITHOUT taking response -> exercises fut->response free!*/
    ASSERT_TRUE(fah_future_destroy(fut2[0]) == FAH_OK);
    ASSERT_TRUE(fah_future_destroy(fut2[1]) == FAH_OK);
    ASSERT_TRUE(fah_future_destroy(0) == FAH_ERR_NULL_HANDLE);
    ASSERT_TRUE(fah_multi_destroy(0) == FAH_ERR_NULL_HANDLE);

    /* fah_box_heap_response OOM path via fah_future_take_response */
    ASSERT_TRUE(fah_multi_destroy(mh_v) == FAH_OK);
    ASSERT_TRUE(fah_multi_create(&mh_v) == FAH_OK);
    ASSERT_TRUE(fah_multi_add(mh_v, th2) == FAH_OK);
    ASSERT_TRUE(fah_multi_add(mh_v, th2) == FAH_OK);
    ASSERT_TRUE(fah_multi_send(cli_h, mh_v, 0, fut2, 2u, &sz) == FAH_OK);
    fah_oom_set_fail_after(1);
    ASSERT_TRUE(fah_future_take_response(fut2[0], &th) == FAH_ERR_NOMEM);
    fah_oom_reset();
    ASSERT_TRUE(fah_future_destroy(fut2[0]) == FAH_OK);
    ASSERT_TRUE(fah_future_destroy(fut2[1]) == FAH_OK);

    /* WS & SSE negative handle paths */
    ASSERT_TRUE(fah_loop_create(0, &loop_v) == FAH_OK);
    ASSERT_TRUE(fah_ws_init(0, NULL, 1) == FAH_ERR_NULL_HANDLE);
    ASSERT_TRUE(fah_ws_sync_loop(0, req_h, NULL, NULL, NULL, 0, 0) ==
                FAH_ERR_NULL_HANDLE);
    ASSERT_TRUE(fah_ws_sync_loop(cli_h, 0, NULL, NULL, NULL, 0, 0) ==
                FAH_ERR_NULL_HANDLE);
    ASSERT_TRUE(fah_ws_sync_loop(cli_h, req_h, NULL, NULL, NULL, 0, 999999) ==
                FAH_ERR_STALE_HANDLE);
    ASSERT_TRUE(fah_ws_async_connect(0, loop_v, req_h, NULL, NULL, NULL, 0,
                                     0) == FAH_ERR_NULL_HANDLE);
    ASSERT_TRUE(fah_ws_async_connect(cli_h, 0, req_h, NULL, NULL, NULL, 0, 0) ==
                FAH_ERR_NULL_HANDLE);
    ASSERT_TRUE(fah_ws_async_connect(cli_h, loop_v, 0, NULL, NULL, NULL, 0,
                                     0) == FAH_ERR_NULL_HANDLE);
    ASSERT_TRUE(fah_ws_async_connect(cli_h, loop_v, req_h, NULL, NULL, NULL, 0,
                                     999999) == FAH_ERR_STALE_HANDLE);
    ASSERT_TRUE(fah_ws_send(req_h, 1, NULL, 5u, 0) == FAH_ERR_INVALID_SLICE);
    ASSERT_TRUE(fah_ws_send(0, 1, "x", 1u, 0) == FAH_ERR_NULL_HANDLE);
    ASSERT_TRUE(fah_ws_close(0, 1000) == FAH_ERR_NULL_HANDLE);
    ASSERT_TRUE(fah_ws_free(0) == FAH_ERR_NULL_HANDLE);

    ASSERT_TRUE(fah_sse_init(req_h, NULL, -1) == FAH_ERR_INVAL);
    ASSERT_TRUE(fah_sse_init(0, NULL, 0) == FAH_ERR_NULL_HANDLE);
    ASSERT_TRUE(fah_sse_sync_loop(0, req_h, NULL, NULL, NULL, 0, 0) ==
                FAH_ERR_NULL_HANDLE);
    ASSERT_TRUE(fah_sse_sync_loop(cli_h, 0, NULL, NULL, NULL, 0, 0) ==
                FAH_ERR_NULL_HANDLE);
    ASSERT_TRUE(fah_sse_sync_loop(cli_h, req_h, NULL, NULL, NULL, 0, 999999) ==
                FAH_ERR_STALE_HANDLE);
    ASSERT_TRUE(fah_sse_async_register(0, loop_v, req_h, NULL, NULL, NULL, 0,
                                       0) == FAH_ERR_NULL_HANDLE);
    ASSERT_TRUE(fah_sse_async_register(cli_h, 0, req_h, NULL, NULL, NULL, 0,
                                       0) == FAH_ERR_NULL_HANDLE);
    ASSERT_TRUE(fah_sse_async_register(cli_h, loop_v, 0, NULL, NULL, NULL, 0,
                                       0) == FAH_ERR_NULL_HANDLE);
    ASSERT_TRUE(fah_sse_async_register(cli_h, loop_v, req_h, NULL, NULL, NULL,
                                       0, 999999) == FAH_ERR_STALE_HANDLE);
    ASSERT_TRUE(fah_sse_free(0) == FAH_ERR_NULL_HANDLE);

    /* Modality negative & OOM paths */
    ASSERT_TRUE(fah_loop_create(0, NULL) == FAH_ERR_INVAL);
    ASSERT_TRUE(fah_loop_create(-1, &th) == FAH_ERR_INVAL);
    fah_oom_set_fail_after(1);
    ASSERT_TRUE(fah_loop_create(0, &th) == FAH_ERR_NOMEM);
    ASSERT_TRUE(fah_loop_create(2, &th) != FAH_OK);
    ASSERT_TRUE(fah_loop_get_backend(loop_v, NULL) == FAH_ERR_INVAL);
    ASSERT_TRUE(fah_loop_get_backend(0, &n1) == FAH_ERR_NULL_HANDLE);
    ASSERT_TRUE(fah_loop_drive(0, 0) == FAH_ERR_NULL_HANDLE);
    ASSERT_TRUE(fah_loop_drive(loop_v, 99) == FAH_ERR_INVAL);
    ASSERT_TRUE(fah_loop_add_timer(loop_v, 1L, NULL, 0, &n1) == FAH_ERR_INVAL);
    ASSERT_TRUE(fah_loop_add_timer(0, 1L, test_timer_cb, 0, &n1) ==
                FAH_ERR_NULL_HANDLE);
    fah_oom_set_fail_after(1);
    ASSERT_TRUE(fah_loop_add_timer(loop_v, 1L, test_timer_cb, 0, &n1) ==
                FAH_ERR_NOMEM);
    ASSERT_TRUE(fah_loop_cancel_timer(0, 1) == FAH_ERR_NULL_HANDLE);
    ASSERT_TRUE(fah_loop_destroy(0) == FAH_ERR_NULL_HANDLE);

    ASSERT_TRUE(fah_time_ms(NULL) == FAH_ERR_INVAL);

    ASSERT_TRUE(fah_mutex_op(NULL, 0) == FAH_ERR_INVAL);
    th = 0;
    ASSERT_TRUE(fah_mutex_op(&th, 3) == FAH_ERR_NULL_HANDLE);
    ASSERT_TRUE(fah_mutex_op(&th, 1) == FAH_ERR_NULL_HANDLE);
    ASSERT_TRUE(fah_mutex_op(&mtx_v, 0) == FAH_OK);
    ASSERT_TRUE(fah_mutex_op(&mtx_v, 99) == FAH_ERR_INVAL);
    ASSERT_TRUE(fah_mutex_op(&mtx_v, 3) == FAH_OK);

    ASSERT_TRUE(fah_cond_op(NULL, 0) == FAH_ERR_INVAL);
    th = 0;
    ASSERT_TRUE(fah_cond_op(&th, 3) == FAH_ERR_NULL_HANDLE);
    ASSERT_TRUE(fah_cond_op(&th, 1) == FAH_ERR_NULL_HANDLE);
    ASSERT_TRUE(fah_cond_op(&cnd_v, 0) == FAH_OK);
    ASSERT_TRUE(fah_cond_op(&cnd_v, 99) == FAH_ERR_INVAL);
    ASSERT_TRUE(fah_cond_op(&cnd_v, 3) == FAH_OK);

    ASSERT_TRUE(fah_pool_create(0u, &th) == FAH_ERR_INVAL);
    ASSERT_TRUE(fah_pool_create(1u, &pool_v) == FAH_OK);
    ASSERT_TRUE(fah_pool_submit(pool_v, NULL, 0) == FAH_ERR_INVAL);
    ASSERT_TRUE(fah_pool_submit(0, test_task_cb, 0) == FAH_ERR_NULL_HANDLE);
    fah_oom_set_fail_after(1);
    ASSERT_TRUE(fah_pool_submit(pool_v, test_task_cb, 0) == FAH_ERR_NOMEM);
    ASSERT_TRUE(fah_pool_destroy(0) == FAH_ERR_NULL_HANDLE);
    ASSERT_TRUE(fah_pool_destroy(pool_v) == FAH_OK);

    ASSERT_TRUE(fah_tls_op(NULL, 0, NULL) == FAH_ERR_INVAL);
    th = 0;
    ASSERT_TRUE(fah_tls_op(&th, 3, NULL) == FAH_ERR_NULL_HANDLE);
    ASSERT_TRUE(fah_tls_op(&tls_v, 0, NULL) == FAH_OK);
    ASSERT_TRUE(fah_tls_op(&tls_v, 1, NULL) == FAH_ERR_INVAL);
    ASSERT_TRUE(fah_tls_op(&th, 1, &n1) == FAH_ERR_NULL_HANDLE);
    fah_oom_set_fail_after(1);
    ASSERT_TRUE(fah_tls_op(&tls_v, 1, &n1) == FAH_ERR_NOMEM);
    ASSERT_TRUE(fah_tls_op(&tls_v, 99, &n1) == FAH_ERR_INVAL);
    ASSERT_TRUE(fah_tls_op(&tls_v, 3, NULL) == FAH_OK);

    ASSERT_TRUE(fah_coro_create(0u, NULL, 0, &th) == FAH_ERR_INVAL);
    fah_oom_set_fail_after(1);
    ASSERT_TRUE(fah_coro_create(0u, test_task_cb, 0, &th) == FAH_ERR_NOMEM);
    ASSERT_TRUE(fah_coro_op(0, 3, NULL) == FAH_ERR_NULL_HANDLE);
    ASSERT_TRUE(fah_coro_op(0, 0, NULL) == FAH_ERR_NULL_HANDLE);
    ASSERT_TRUE(fah_coro_create(0u, test_task_cb, 1, &coro_v) == FAH_OK);
    ASSERT_TRUE(fah_coro_op(coro_v, 2, NULL) == FAH_ERR_INVAL);
    ASSERT_TRUE(fah_coro_op(coro_v, 99, NULL) == FAH_ERR_INVAL);
    ASSERT_TRUE(fah_coro_op(coro_v, 3, NULL) == FAH_OK);

    ASSERT_TRUE(fah_bus_create(NULL) == FAH_ERR_INVAL);
    fah_oom_set_fail_after(1);
    ASSERT_TRUE(fah_bus_create(&th) == FAH_ERR_NOMEM);
    ASSERT_TRUE(fah_bus_create(&bus_live) == FAH_OK);
    ASSERT_TRUE(fah_actor_spawn(bus_live, NULL, test_actor_cb, 0, &th) ==
                FAH_ERR_INVAL);
    ASSERT_TRUE(fah_actor_spawn(0, "a", test_actor_cb, 0, &th) ==
                FAH_ERR_NULL_HANDLE);
    fah_oom_set_fail_after(1);
    ASSERT_TRUE(fah_actor_spawn(bus_live, "a", test_actor_cb, 0, &th) ==
                FAH_ERR_NOMEM);
    ASSERT_TRUE(fah_actor_spawn(bus_live, "live_act", test_actor_cb, 0, &th) ==
                FAH_OK);
    ASSERT_TRUE(fah_actor_send_int(0, 0, th, 1, 1) == FAH_ERR_NULL_HANDLE);
    ASSERT_TRUE(fah_actor_send_int(bus_live, 0, 0, 1, 1) ==
                FAH_ERR_NULL_HANDLE);
    ASSERT_TRUE(fah_actor_send_int(bus_live, 999999, th, 1, 1) ==
                FAH_ERR_STALE_HANDLE);
    fah_oom_set_fail_after(1);
    ASSERT_TRUE(fah_actor_send_int(bus_live, 0, th, 1, 1) == FAH_ERR_NOMEM);
    ASSERT_TRUE(fah_bus_process(bus_live, NULL) == FAH_ERR_INVAL);
    ASSERT_TRUE(fah_bus_process(0, &n1) == FAH_ERR_NULL_HANDLE);
    ASSERT_TRUE(fah_actor_get_name(0, buf, sizeof(buf), &sz, 0) ==
                FAH_ERR_NULL_HANDLE);
    ASSERT_TRUE(fah_actor_destroy(0) == FAH_ERR_NULL_HANDLE);
    ASSERT_TRUE(fah_bus_destroy(0) == FAH_ERR_NULL_HANDLE);

    ASSERT_TRUE(fah_ipc_op(0, 0, NULL, 0u, NULL, 0u, NULL) == FAH_ERR_INVAL);
    ASSERT_TRUE(fah_ipc_op(1, th2, NULL, 0u, NULL, 0u, NULL) == FAH_ERR_INVAL);
    ASSERT_TRUE(fah_ipc_op(1, 0, NULL, 0u, NULL, 0u, &n1) ==
                FAH_ERR_NULL_HANDLE);
    fah_oom_set_fail_after(1);
    ASSERT_TRUE(fah_ipc_op(1, th2, NULL, 0u, NULL, 0u, &n1) == FAH_ERR_NOMEM);
    ASSERT_TRUE(fah_ipc_op(2, res_live, NULL, 0u, NULL, 0u, NULL) ==
                FAH_ERR_INVAL);
    ASSERT_TRUE(fah_ipc_op(2, 0, NULL, 0u, NULL, 0u, &n1) ==
                FAH_ERR_NULL_HANDLE);
    fah_oom_set_fail_after(1);
    ASSERT_TRUE(fah_ipc_op(2, res_live, NULL, 0u, NULL, 0u, &n1) ==
                FAH_ERR_NOMEM);
    ASSERT_TRUE(fah_ipc_op(99, 0, NULL, 0u, NULL, 0u, NULL) == FAH_ERR_INVAL);

    ASSERT_TRUE(fah_cmp_op(0, 1, 1u, 2u, 0, NULL, NULL) == FAH_ERR_INVAL);
    ASSERT_TRUE(fah_cmp_op(1, 1, 1u, 2u, 0, NULL, NULL) == FAH_ERR_NULL_HANDLE);
    ASSERT_TRUE(fah_cmp_op(2, 0, 0u, 0u, th2, NULL, NULL) == FAH_ERR_INVAL);
    ASSERT_TRUE(fah_cmp_op(2, 0, 0u, 0u, 0, NULL, &n1) == FAH_ERR_NULL_HANDLE);
    fah_oom_set_fail_after(1);
    ASSERT_TRUE(fah_cmp_op(2, 0, 0u, 0u, th2, NULL, &n1) == FAH_ERR_NOMEM);
    ASSERT_TRUE(fah_cmp_op(3, 0, 0u, 0u, 0, NULL, NULL) == FAH_ERR_NULL_HANDLE);
    ASSERT_TRUE(fah_cmp_op(4, 0, 0u, 0u, 0, NULL, NULL) == FAH_ERR_NULL_HANDLE);
    ASSERT_TRUE(fah_cmp_op(99, 0, 0u, 0u, 0, NULL, NULL) == FAH_ERR_INVAL);
    fah_oom_reset();

    /* Full handle-table rollback sweep across all 20 handle constructors! */
    while (nfill < FAH_HANDLE_MAX_SLOTS &&
           fah_handle_register(FAH_TAG_CONFIG, &fill_dummy, &fill[nfill]) ==
               FAH_OK) {
      nfill++;
    }
    ASSERT_TRUE(fah_poll_queue_create(&n1) == FAH_ERR_HANDLE_TABLE_FULL);
    ASSERT_TRUE(fah_config_create(&n1) == FAH_ERR_HANDLE_TABLE_FULL);
    ASSERT_TRUE(fah_client_create(cfg_h, 1, &n1) == FAH_ERR_HANDLE_TABLE_FULL);
    ASSERT_TRUE(fah_headers_create(&n1) == FAH_ERR_HANDLE_TABLE_FULL);
    ASSERT_TRUE(fah_cookie_jar_create(&n1) == FAH_ERR_HANDLE_TABLE_FULL);
    ASSERT_TRUE(fah_request_create(&n1) == FAH_ERR_HANDLE_TABLE_FULL);
    ASSERT_TRUE(fah_send(cli_h, th2, &n1) == FAH_ERR_HANDLE_TABLE_FULL);
    ASSERT_TRUE(fah_multi_create(&n1) == FAH_ERR_HANDLE_TABLE_FULL);
    ASSERT_TRUE(fah_loop_create(0, &n1) == FAH_ERR_HANDLE_TABLE_FULL);
    ASSERT_TRUE(fah_mutex_op(&n1, 0) == FAH_ERR_HANDLE_TABLE_FULL);
    ASSERT_TRUE(fah_cond_op(&n1, 0) == FAH_ERR_HANDLE_TABLE_FULL);
    ASSERT_TRUE(fah_pool_create(1u, &n1) == FAH_ERR_HANDLE_TABLE_FULL);
    ASSERT_TRUE(fah_tls_op(&n1, 0, NULL) == FAH_ERR_HANDLE_TABLE_FULL);
    ASSERT_TRUE(fah_coro_create(0u, test_task_cb, 1, &n1) ==
                FAH_ERR_HANDLE_TABLE_FULL);
    ASSERT_TRUE(fah_bus_create(&n1) == FAH_ERR_HANDLE_TABLE_FULL);
    ASSERT_TRUE(fah_actor_spawn(bus_live, "full_act", test_actor_cb, 0, &n1) ==
                FAH_ERR_HANDLE_TABLE_FULL);
    ASSERT_TRUE(fah_ipc_op(2, res_live, NULL, 0u, NULL, 0u, &n1) ==
                FAH_ERR_HANDLE_TABLE_FULL);
    ASSERT_TRUE(fah_cmp_op(2, 0, 0u, 0u, th2, NULL, &n1) ==
                FAH_ERR_HANDLE_TABLE_FULL);
    /* Free 1 slot so fah_multi_send registers slot 0 and rolls back on slot 1
     */
    {
      void *u = NULL;
      nfill--;
      ASSERT_TRUE(fah_handle_unregister(fill[nfill], FAH_TAG_CONFIG, &u) ==
                  FAH_OK);
    }
    ASSERT_TRUE(fah_multi_send(cli_h, mh_v, 0, fut2, 2u, &sz) ==
                FAH_ERR_HANDLE_TABLE_FULL);
    while (nfill > 0) {
      void *u = NULL;
      nfill--;
      ASSERT_TRUE(fah_handle_unregister(fill[nfill], FAH_TAG_CONFIG, &u) ==
                  FAH_OK);
    }

    /* Destroy bus_live WITH live actor `th` still registered (lines 2910-11) */
    ASSERT_TRUE(fah_bus_destroy(bus_live) == FAH_OK);

    /* Remaining fah_fortran_entries.c IH* & FH7* branches */
    ASSERT_TRUE(FAH_FC_GLOBAL(ihpqpp, IHPQPP)(NULL, &n1, &n2, ibuf, &one,
                                              &n1) == FAH_ERR_INVAL);
    fah_oom_set_fail_after(1);
    ASSERT_TRUE(FAH_FC_GLOBAL(ihpqpp, IHPQPP)(&q_v, &n1, &n2, ibuf, &one,
                                              &n1) == FAH_ERR_NOMEM);
    ASSERT_TRUE(FAH_FC_GLOBAL(ihcfst, IHCFST)(NULL, &one, &zero, &zero,
                                              &zero) == FAH_ERR_INVAL);
    ASSERT_TRUE(FAH_FC_GLOBAL(ihcfst, IHCFST)(&zero, &one, &zero, &zero,
                                              &zero) == FAH_ERR_NULL_HANDLE);
    ASSERT_TRUE(FAH_FC_GLOBAL(ihclcr, IHCLCR)(NULL, &one, &n1) ==
                FAH_ERR_INVAL);
    ASSERT_TRUE(FAH_FC_GLOBAL(ihrqtg, IHRQTG)(NULL, &zero, &zero, ok_ic,
                                              &two) == FAH_ERR_INVAL);
    ASSERT_TRUE(
        FAH_FC_GLOBAL(ihrqtg, IHRQTG)(&th2, &zero, &zero, bad_ic, &two) ==
        FAH_ERR_HOLLERITH_DECODE);
    ASSERT_TRUE(FAH_FC_GLOBAL(ihrqhd, IHRQHD)(NULL, &zero, ok_ic, &two, ok_ic,
                                              &two) == FAH_ERR_INVAL);
    ASSERT_TRUE(
        FAH_FC_GLOBAL(ihrqhd, IHRQHD)(&th2, &zero, bad_ic, &two, ok_ic, &two) ==
        FAH_ERR_HOLLERITH_DECODE);
    ASSERT_TRUE(
        FAH_FC_GLOBAL(ihrqhd, IHRQHD)(&th2, &zero, ok_ic, &two, bad_ic, &two) ==
        FAH_ERR_HOLLERITH_DECODE);
    ASSERT_TRUE(FAH_FC_GLOBAL(ihrqbd, IHRQBD)(NULL, &zero, ok_ic, &two) ==
                FAH_ERR_INVAL);
    ASSERT_TRUE(FAH_FC_GLOBAL(ihrqbd, IHRQBD)(&th2, &zero, bad_ic, &two) ==
                FAH_ERR_HOLLERITH_DECODE);
    ASSERT_TRUE(FAH_FC_GLOBAL(ihrqcb, IHRQCB)(NULL, &q_v) == FAH_ERR_INVAL);
    ASSERT_TRUE(FAH_FC_GLOBAL(ihsend, IHSEND)(NULL, &th2, &n1) ==
                FAH_ERR_INVAL);
    ASSERT_TRUE(FAH_FC_GLOBAL(ihrsin, IHRSIN)(NULL, &n1, &n2) == FAH_ERR_INVAL);
    ASSERT_TRUE(FAH_FC_GLOBAL(ihrsin, IHRSIN)(&zero, &n1, &n2) ==
                FAH_ERR_NULL_HANDLE);
    ASSERT_TRUE(FAH_FC_GLOBAL(ihrsbd, IHRSBD)(NULL, &zero, ibuf, &four, &n1) ==
                FAH_ERR_INVAL);
    ASSERT_TRUE(FAH_FC_GLOBAL(ihrsbd, IHRSBD)(&zero, &zero, ibuf, &four, &n1) ==
                FAH_ERR_NULL_HANDLE);
    ASSERT_TRUE(FAH_FC_GLOBAL(ihrshd, IHRSHD)(NULL, &zero, ok_ic, &two, ibuf,
                                              &four, &n1) == FAH_ERR_INVAL);
    ASSERT_TRUE(FAH_FC_GLOBAL(ihrshd, IHRSHD)(&res_live, &zero, bad_ic, &two,
                                              ibuf, &four,
                                              &n1) == FAH_ERR_HOLLERITH_DECODE);
    ASSERT_TRUE(FAH_FC_GLOBAL(ihrshd, IHRSHD)(&zero, &zero, ok_ic, &two, ibuf,
                                              &four,
                                              &n1) == FAH_ERR_NULL_HANDLE);
    ASSERT_TRUE(FAH_FC_GLOBAL(ihwsop, IHWSOP)(&cli_h, NULL, &zero, &one, &zero,
                                              ok_ic, &two,
                                              &q_v) == FAH_ERR_INVAL);
    ASSERT_TRUE(FAH_FC_GLOBAL(ihwsop, IHWSOP)(NULL, &th2, &one, &one, &zero,
                                              ok_ic, &two,
                                              &q_v) == FAH_ERR_INVAL);
    ASSERT_TRUE(FAH_FC_GLOBAL(ihwsop, IHWSOP)(&cli_h, &th2, &two, &one, NULL,
                                              ok_ic, &two,
                                              &q_v) == FAH_ERR_INVAL);
    ASSERT_TRUE(FAH_FC_GLOBAL(ihwsop, IHWSOP)(&cli_h, &th2, &two, &one, &zero,
                                              bad_ic, &two, &q_v) ==
                FAH_ERR_HOLLERITH_DECODE);
    ASSERT_TRUE(FAH_FC_GLOBAL(ihwsop, IHWSOP)(&cli_h, &th2, &bad, &one, &zero,
                                              ok_ic, &two,
                                              &q_v) == FAH_ERR_INVAL);
    ASSERT_TRUE(FAH_FC_GLOBAL(ihsseo, IHSSEO)(&cli_h, NULL, &zero, &one,
                                              &q_v) == FAH_ERR_INVAL);
    ASSERT_TRUE(FAH_FC_GLOBAL(ihsseo, IHSSEO)(NULL, &th2, &one, &one, &q_v) ==
                FAH_ERR_INVAL);
    ASSERT_TRUE(FAH_FC_GLOBAL(ihsseo, IHSSEO)(&cli_h, &th2, &bad, &one, &q_v) ==
                FAH_ERR_INVAL);
    ASSERT_TRUE(FAH_FC_GLOBAL(iherrc, IHERRC)(NULL, &cat, &rty, ibuf, &four,
                                              &n1) == FAH_ERR_INVAL);

    fah_oom_set_fail_after(1);
    FAH_FC_GLOBAL(fh7cfs, FH7CFS)(&cfg_h, &one, "ua", &rc_i, 2u);
    ASSERT_TRUE(rc_i == FAH_ERR_NOMEM);
    fah_oom_set_fail_after(1);
    FAH_FC_GLOBAL(fh7hcj, FH7HCJ)(&zero, &four, "K", sbuf1, &n1, &rc_i, 1u,
                                  64u);
    ASSERT_TRUE(rc_i == FAH_ERR_NOMEM);
    fah_oom_set_fail_after(1);
    FAH_FC_GLOBAL(fh7req, FH7REQ)(&th2, &one, &zero, "http://x", " ", " ", " ",
                                  &rc_i, 8u, 1u, 1u, 1u);
    ASSERT_TRUE(rc_i == FAH_ERR_NOMEM);
    fah_oom_set_fail_after(1);
    FAH_FC_GLOBAL(fh7stm, FH7STM)(&cli_h, &loop_v, &th2, &one, &one, "chat",
                                  NULL, NULL, NULL, &one, &rc_i, 4u);
    ASSERT_TRUE(rc_i == FAH_ERR_NOMEM);
    fah_oom_set_fail_after(1);
    FAH_FC_GLOBAL(fh7err, FH7ERR)(&neg1, "op", sbuf1, &cat, &rty, &n1, &rc_i,
                                  2u, 64u);
    ASSERT_TRUE(rc_i == FAH_ERR_NOMEM);
    fah_oom_reset();

    FAH_FC_GLOBAL(fh7oa2, FH7OA2)(&th2, &four, "http://127.0.0.1/t", "cid",
                                  "sec", " ", " ", " ", sbuf1, &n1, sbuf2, &n2,
                                  &rc_i, 18u, 3u, 3u, 1u, 1u, 1u, 64u, 64u);
    FAH_FC_GLOBAL(fh7oa2, FH7OA2)(&port, &eleven, "OK", " ", " ", " ", " ", " ",
                                  sbuf1, &n1, sbuf2, &n2, &rc_i, 2u, 1u, 1u, 1u,
                                  1u, 1u, 64u, 64u);

    {
      int three = 3, six = 6, seven = 7, thirty = 30;
      FAH_FC_GLOBAL(fh7res, FH7RES)(&res_live, &three, &zero, sbuf1, sbuf2, &n1,
                                    &n2, &rc_i, 64u, 64u);
      FAH_FC_GLOBAL(fh7res, FH7RES)(&res_live, &four, &neg1, sbuf1, sbuf2, &n1,
                                    &n2, &rc_i, 64u, 64u);
      FAH_FC_GLOBAL(fh7res, FH7RES)(&res_live, &four, &zero, sbuf1, sbuf2, &n1,
                                    &n2, &rc_i, 64u, 64u);
      FAH_FC_GLOBAL(fh7res, FH7RES)(&res_live, &bad, &zero, sbuf1, sbuf2, &n1,
                                    &n2, &rc_i, 64u, 64u);

      FAH_FC_GLOBAL(fh7mul, FH7MUL)(&cli_h, &mh_v, &three, &zero, fut2, &neg1,
                                    &n1, &n2, &rc_i);
      FAH_FC_GLOBAL(fh7mul, FH7MUL)(&cli_h, &zero, &six, &zero, fut2, &two, &n1,
                                    &n2, &rc_i);
      FAH_FC_GLOBAL(fh7mul, FH7MUL)(&cli_h, &zero, &seven, &zero, fut2, &two,
                                    &n1, &n2, &rc_i);
      FAH_FC_GLOBAL(fh7mul, FH7MUL)(&cli_h, &zero, &bad, &zero, fut2, &two, &n1,
                                    &n2, &rc_i);

      memcpy(sbuf1, "    ", 4u);
      FAH_FC_GLOBAL(fh7mod, FH7MOD)(&thirty, &th2, &th, &zero, &zero, &zero,
                                    sbuf1, NULL, &n1, &rc_i, 4u);
      th = 0;
      FAH_FC_GLOBAL(fh7mod, FH7MOD)(&thirty, &th2, &th, &one, &zero, &zero,
                                    sbuf1, NULL, &n1, &rc_i, 4u);
      ASSERT_TRUE(fah_request_destroy(th) == FAH_OK);
    }

    ASSERT_TRUE(fah_loop_destroy(loop_v) == FAH_OK);
    ASSERT_TRUE(fah_multi_destroy(mh_v) == FAH_OK);
    ASSERT_TRUE(fah_response_destroy(res_live) == FAH_OK);
    ASSERT_TRUE(fah_request_destroy(th2) == FAH_OK);
    ASSERT_TRUE(fah_poll_queue_destroy(q_v) == FAH_OK);

    for (step = 1; step <= 18; step++) {
      fah_oom_set_fail_after(step);
      if (fah_poll_queue_create(&th) == FAH_OK) {
        FAH_IGNORE_RC(
            fah_poll_queue_push(th, FAH_POLL_SSE_EVT, 1, "ev", "abc", 3u));
        FAH_IGNORE_RC(fah_poll_queue_destroy(th));
      }
      fah_oom_set_fail_after(step);
      if (fah_client_create(cfg_h, 1, &th) == FAH_OK) {
        FAH_IGNORE_RC(fah_client_destroy(th));
      }
    }
    fah_oom_reset();
  }

  ASSERT_TRUE(fah_request_destroy(req_h) == FAH_OK);
  ASSERT_TRUE(fah_client_destroy(cli_h) == FAH_OK);
  ASSERT_TRUE(fah_cookie_jar_destroy(jar_h) == FAH_OK);
  ASSERT_TRUE(fah_config_destroy(cfg_h) == FAH_OK);
  ASSERT_TRUE(FAH_FC_GLOBAL(ihgcln, IHGCLN)(&dummy) == 0);
}

int main(void) {
  int port = 0;
  test_abi_shim_and_alloc();
  test_errors_and_registry();
  test_codecs_cfi_and_callbacks();

  ASSERT_TRUE(fah_mock_server_start(&port) == 0);
  test_bridge_and_fortran_entries(port);
  ASSERT_TRUE(fah_mock_server_stop(port) == 0);

  printf("test_fah_c_bridge: PASS (active handles remaining: %lu)\n",
         (unsigned long)fah_handle_active_count());
  ASSERT_TRUE(fah_handle_active_count() == 0u);
  return 0;
}
