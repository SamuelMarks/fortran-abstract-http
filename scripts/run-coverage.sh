#!/bin/sh
# Build with unified GNU gcc + gfortran gcov instrumentation, run CTest across
# all 14 targets, and enforce 100.0% line and function coverage across all C89
# FFI bridge files and all 12 Fortran standard layers.
set -eu

COV_DIR="${1:-build-coverage}"
REPO_ROOT="$(CDPATH='' cd -- "$(dirname -- "$0")/.." && pwd)"

# Select matching GNU (gcc, gfortran, gcov) triple (ignoring GNU make's default CC=cc)
C_COMP="${FAH_CC:-}"
FC_COMP="${FAH_FC:-}"
GCOV_BIN="${FAH_GCOV:-}"
if [ -z "${C_COMP}" ] || [ -z "${FC_COMP}" ] || [ -z "${GCOV_BIN}" ]; then
  for ver in 16 15 14 13; do
    if command -v "gcc-${ver}" >/dev/null 2>&1 && \
       command -v "gfortran-${ver}" >/dev/null 2>&1 && \
       command -v "gcov-${ver}" >/dev/null 2>&1; then
      C_COMP="${C_COMP:-$(command -v "gcc-${ver}")}"
      FC_COMP="${FC_COMP:-$(command -v "gfortran-${ver}")}"
      GCOV_BIN="${GCOV_BIN:-$(command -v "gcov-${ver}")}"
      break
    fi
  done
fi
if [ -z "${C_COMP}" ]; then
  C_COMP="$(command -v gcc || command -v cc)"
fi
if [ -z "${FC_COMP}" ]; then
  FC_COMP="$(command -v gfortran)"
fi
if [ -z "${GCOV_BIN}" ]; then
  GCOV_BIN="$(command -v gcov)"
fi

if [ ! -f "${REPO_ROOT}/${COV_DIR}/CMakeCache.txt" ] || \
   ! grep -q "CMAKE_C_COMPILER:.*=${C_COMP}$" "${REPO_ROOT}/${COV_DIR}/CMakeCache.txt" 2>/dev/null || \
   ! grep -q "CMAKE_Fortran_COMPILER:.*=${FC_COMP}$" "${REPO_ROOT}/${COV_DIR}/CMakeCache.txt" 2>/dev/null; then
  rm -rf "${REPO_ROOT}/${COV_DIR}"
  cmake -S "${REPO_ROOT}" -B "${REPO_ROOT}/${COV_DIR}" \
    -DCMAKE_C_COMPILER="${C_COMP}" \
    -DCMAKE_Fortran_COMPILER="${FC_COMP}" \
    -DCMAKE_BUILD_TYPE=Debug \
    -DBUILD_TESTING=ON \
    -DFAH_BUILD_TESTING=ON \
    -DFAH_ENABLE_COVERAGE=ON \
    -DC_ABSTRACT_HTTP_USE_RAW_SOCKETS=ON
fi

find "${REPO_ROOT}/${COV_DIR}" -name '*.gcda' -delete

cmake --build "${REPO_ROOT}/${COV_DIR}" -j
ctest --test-dir "${REPO_ROOT}/${COV_DIR}" --output-on-failure

python3 - "${REPO_ROOT}" "${REPO_ROOT}/${COV_DIR}" "${GCOV_BIN}" <<'PY'
import glob
import json
import pathlib
import subprocess
import sys

repo = pathlib.Path(sys.argv[1]).resolve()
bdir = pathlib.Path(sys.argv[2]).resolve()
gcov_bin = sys.argv[3]

gcnos = sorted(glob.glob(str(bdir / "src" / "CMakeFiles" / "**" / "*.gcno"), recursive=True))

file_lines = {}
file_funcs = {}

for gcno in gcnos:
    if "fah_bind_raw.dir" in gcno:
        continue
    p = pathlib.Path(gcno)
    res = subprocess.run(
        [gcov_bin, "--json-format", "--stdout", "-o", str(p.parent), str(p)],
        capture_output=True,
        check=True,
    )
    payload = json.loads(res.stdout.decode("utf-8"))
    for fobj in payload.get("files", []):
        fpath = pathlib.Path(fobj["file"]).resolve()
        try:
            rel = str(fpath.relative_to(repo))
        except ValueError:
            continue
        if not (rel.startswith("src/") or rel == "generated/c_abstract_http_fortran_shim.c"):
            continue
        lmap = file_lines.setdefault(rel, {})
        for ln in fobj.get("lines", []):
            lno = ln["line_number"]
            lmap[lno] = lmap.get(lno, 0) + int(ln["count"])
        fmap = file_funcs.setdefault(rel, {})
        for fn in fobj.get("functions", []):
            fname = fn["name"]
            fmap[fname] = fmap.get(fname, 0) + int(fn["execution_count"])

print("==============================================================================")
print("               fortran-abstract-http Unified Coverage Report")
print("==============================================================================")
print(f"{'File':<46} {'Lines':>11} {'Funcs':>11} {'Cover':>7}")
print("------------------------------------------------------------------------------")

tot_l = 0
hit_l = 0
tot_f = 0
hit_f = 0
incomplete = []

for rel in sorted(file_lines.keys()):
    lmap = file_lines[rel]
    fmap = file_funcs.get(rel, {})
    nl = len(lmap)
    hl = sum(1 for v in lmap.values() if v > 0)
    nf = len(fmap)
    hf = sum(1 for v in fmap.values() if v > 0)
    tot_l += nl
    hit_l += hl
    tot_f += nf
    hit_f += hf
    pct = (100.0 * hl / nl) if nl else 100.0
    print(f"{rel:<46} {hl:>5}/{nl:<5} {hf:>5}/{nf:<5} {pct:>6.1f}%")
    if hl < nl or hf < nf:
        missed_l = sorted(k for k, v in lmap.items() if v == 0)
        missed_f = sorted(k for k, v in fmap.items() if v == 0)
        incomplete.append((rel, missed_l, missed_f))

print("------------------------------------------------------------------------------")
lpct = (100.0 * hit_l / tot_l) if tot_l else 100.0
fpct = (100.0 * hit_f / tot_f) if tot_f else 100.0
print(f"{'TOTAL':<46} {hit_l:>5}/{tot_l:<5} {hit_f:>5}/{tot_f:<5} {lpct:>6.1f}%")
print(f"Line Coverage:     {lpct:.2f}% ({hit_l}/{tot_l})")
print(f"Function Coverage: {fpct:.2f}% ({hit_f}/{tot_f})")
print("==============================================================================")

if incomplete:
    for rel, ml, mf in incomplete:
        if mf:
            print(f"MISSED FUNCS in {rel}: {mf}", file=sys.stderr)
        if ml:
            print(f"MISSED LINES in {rel}: {ml}", file=sys.stderr)
    sys.exit(1)
PY
