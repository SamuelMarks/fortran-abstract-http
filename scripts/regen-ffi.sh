#!/bin/sh
# Deterministic FFI regeneration script for fortran-abstract-http.
# Invokes ../cdd-c on ../c-abstract-http headers and emits:
#   - generated/c_abstract_http_bind_mod.f90
#   - generated/c_abstract_http_bind_mod_test.f90
#   - generated/c_abstract_http_f77.inc
#   - generated/c_abstract_http_fortran_shim.h
#   - generated/c_abstract_http_fortran_shim.c
set -eu

REPO_ROOT="$(CDPATH='' cd -- "$(dirname -- "$0")/.." && pwd)"
CDD_C_BIN="${CDD_C_BIN:-${REPO_ROOT}/../cdd-c/bin/cdd-c}"
C_ABS_HTTP_DIR="${C_ABSTRACT_HTTP_DIR:-${REPO_ROOT}/../c-abstract-http}"
OUT_DIR="${REPO_ROOT}/generated"

if [ ! -x "${CDD_C_BIN}" ]; then
  echo "error: cdd-c binary not found or not executable at ${CDD_C_BIN}" >&2
  exit 1
fi

if [ ! -d "${C_ABS_HTTP_DIR}/include/c_abstract_http" ]; then
  echo "error: c-abstract-http headers not found at ${C_ABS_HTTP_DIR}/include/c_abstract_http" >&2
  exit 1
fi

mkdir -p "${OUT_DIR}"

"${CDD_C_BIN}" bind \
  -i "${C_ABS_HTTP_DIR}/include/c_abstract_http" \
  -o "${OUT_DIR}" \
  -l fortran \
  -n c-abstract-http \
  -m c_abstract_http_bind_mod \
  --generate-tests

# Remove standalone fpm.toml inside generated/ (root fpm.toml manages packaging)
rm -f "${OUT_DIR}/fpm.toml"

# Normalize Doxygen comment markers in c_abstract_http_bind_mod.f90 so every
# declaration satisfies strict 100% documentation checks.
python3 - "${OUT_DIR}/c_abstract_http_bind_mod.f90" <<'PY'
import sys

path = sys.argv[1]
with open(path, "r", encoding="utf-8") as fh:
    lines = fh.readlines()

out = []
for i, line in enumerate(lines):
    stripped = line.lstrip(" ")
    indent = line[: len(line) - len(stripped)]
    if stripped.startswith("! @") or stripped.startswith("! <"):
        rest = stripped[2:]
        if rest.startswith("< "):
            out.append(f"{indent}!< {rest[2:]}")
        else:
            out.append(f"{indent}!> {rest}")
    else:
        out.append(line)

with open(path, "w", encoding="utf-8") as fh:
    fh.writelines(out)
PY

# Emit strict ANSI X3.9-1978 (FORTRAN 77, <= 72 columns, <= 6-char names) include
cat > "${OUT_DIR}/c_abstract_http_f77.inc" <<'EOF'
C     ==================================================================
C     FILE: c_abstract_http_f77.inc
C     AUTO-GENERATED ANSI X3.9-1978 FORTRAN 77 CONSTANTS FOR
C     C-ABSTRACT-HTTP (STRICT 72-COLUMN FIXED-FORM, <= 6-CHAR NAMES).
C     ==================================================================
      INTEGER CAHOK, CAHINV, CAHNOM, CAHIO, CAHNSP, CAHTMO, CAHSYS
      PARAMETER (CAHOK  = 0)
      PARAMETER (CAHINV = -1)
      PARAMETER (CAHNOM = -2)
      PARAMETER (CAHIO  = -3)
      PARAMETER (CAHNSP = -4)
      PARAMETER (CAHTMO = -5)
      PARAMETER (CAHSYS = -6)
      INTEGER CAHWSF, CAHWSH, CAHSSE, CAHSSO, CAHWSC, CAHCNF, CAHCAN
      PARAMETER (CAHWSF = -1001)
      PARAMETER (CAHWSH = -1002)
      PARAMETER (CAHSSE = -1003)
      PARAMETER (CAHSSO = -1004)
      PARAMETER (CAHWSC = -1005)
      PARAMETER (CAHCNF = 111)
      PARAMETER (CAHCAN = 22)
      INTEGER CAHGET, CAHPST, CAHPUT, CAHDEL, CAHHED, CAHPAT, CAHOPT
      PARAMETER (CAHGET = 0)
      PARAMETER (CAHPST = 1)
      PARAMETER (CAHPUT = 2)
      PARAMETER (CAHDEL = 3)
      PARAMETER (CAHHED = 4)
      PARAMETER (CAHPAT = 5)
      PARAMETER (CAHOPT = 6)
EOF

# Emit strict ISO C89 ABI sizeof query header & implementation
cat > "${OUT_DIR}/c_abstract_http_fortran_shim.h" <<'EOF'
/**
 * @file c_abstract_http_fortran_shim.h
 * @brief Auto-generated strict ISO C89 ABI struct size queries for verifying
 * `C_SIZEOF` parity between `c-abstract-http` C structs and
 * `c_abstract_http_bind_mod.f90`.
 */

#ifndef C_ABSTRACT_HTTP_FORTRAN_SHIM_H
#define C_ABSTRACT_HTTP_FORTRAN_SHIM_H

/* clang-format off */
#include <stddef.h>
#include "fah_export.h"
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @brief Return `sizeof(struct HttpConfig)` in bytes.
 * @return Byte size of `struct HttpConfig`.
 */
FAH_NO_DISCARD FAH_API size_t fah_abi_sizeof_http_config(void);

/**
 * @brief Return `sizeof(struct HttpClient)` in bytes.
 * @return Byte size of `struct HttpClient`.
 */
FAH_NO_DISCARD FAH_API size_t fah_abi_sizeof_http_client(void);

/**
 * @brief Return `sizeof(struct HttpHeaders)` in bytes.
 * @return Byte size of `struct HttpHeaders`.
 */
FAH_NO_DISCARD FAH_API size_t fah_abi_sizeof_http_headers(void);

/**
 * @brief Return `sizeof(struct HttpCookieJar)` in bytes.
 * @return Byte size of `struct HttpCookieJar`.
 */
FAH_NO_DISCARD FAH_API size_t fah_abi_sizeof_http_cookie_jar(void);

/**
 * @brief Return `sizeof(struct HttpRequest)` in bytes.
 * @return Byte size of `struct HttpRequest`.
 */
FAH_NO_DISCARD FAH_API size_t fah_abi_sizeof_http_request(void);

/**
 * @brief Return `sizeof(struct HttpResponse)` in bytes.
 * @return Byte size of `struct HttpResponse`.
 */
FAH_NO_DISCARD FAH_API size_t fah_abi_sizeof_http_response(void);

/**
 * @brief Return `sizeof(struct HttpMultiRequest)` in bytes.
 * @return Byte size of `struct HttpMultiRequest`.
 */
FAH_NO_DISCARD FAH_API size_t fah_abi_sizeof_http_multi_request(void);

/**
 * @brief Return `sizeof(struct HttpFuture)` in bytes.
 * @return Byte size of `struct HttpFuture`.
 */
FAH_NO_DISCARD FAH_API size_t fah_abi_sizeof_http_future(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* C_ABSTRACT_HTTP_FORTRAN_SHIM_H */
EOF

cat > "${OUT_DIR}/c_abstract_http_fortran_shim.c" <<'EOF'
/**
 * @file c_abstract_http_fortran_shim.c
 * @brief Auto-generated strict ISO C89 implementation of ABI struct size
 * queries for `c-abstract-http`.
 */

/* clang-format off */
#include "c_abstract_http_fortran_shim.h"
#include <c_abstract_http/http_types.h>
/* clang-format on */

size_t fah_abi_sizeof_http_config(void) { return sizeof(struct HttpConfig); }

size_t fah_abi_sizeof_http_client(void) { return sizeof(struct HttpClient); }

size_t fah_abi_sizeof_http_headers(void) { return sizeof(struct HttpHeaders); }

size_t fah_abi_sizeof_http_cookie_jar(void) {
  return sizeof(struct HttpCookieJar);
}

size_t fah_abi_sizeof_http_request(void) { return sizeof(struct HttpRequest); }

size_t fah_abi_sizeof_http_response(void) {
  return sizeof(struct HttpResponse);
}

size_t fah_abi_sizeof_http_multi_request(void) {
  return sizeof(struct HttpMultiRequest);
}

size_t fah_abi_sizeof_http_future(void) { return sizeof(struct HttpFuture); }
EOF

if command -v clang-format >/dev/null 2>&1; then
  clang-format -i --style=LLVM \
    "${OUT_DIR}/c_abstract_http_fortran_shim.h" \
    "${OUT_DIR}/c_abstract_http_fortran_shim.c"
fi
