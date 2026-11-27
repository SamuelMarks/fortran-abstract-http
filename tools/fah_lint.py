#!/usr/bin/env python3
"""Static no-panic / no-unwrap / unified-error-enum linter for fortran-abstract-http.

Enforces Global Quality Standards across all library code (include/, src/, and
non-test generated/):
  - Zero STOP, ERROR STOP, PAUSE, CALL ABORT, or CALL EXIT in Fortran library units.
  - Zero abort(), exit(), _Exit(), or assert() in C89 FFI headers/sources.
  - Unified error enum discipline: every public C bridge function in
    include/fah_c_bridge.h carries FAH_NO_DISCARD and returns fah_error_code_t.
"""

import argparse
import pathlib
import re
import sys
from typing import List


FORTRAN_PANIC_RE = re.compile(
    r"(?:^|[\s)])(?:ERROR\s+STOP|STOP|PAUSE|CALL\s+ABORT|CALL\s+EXIT)\b",
    re.IGNORECASE,
)
C_PANIC_RE = re.compile(r"\b(?:abort|exit|_Exit|assert)\s*\(")


def strip_c_comments_and_strings(src: str) -> str:
    """Remove C /* ... */ comments, // comments, and string literals."""
    src = re.sub(r"/\*.*?\*/", " ", src, flags=re.DOTALL)
    src = re.sub(r"//[^\n]*", " ", src)
    src = re.sub(r'"(?:\\.|[^"\\])*"', '""', src)
    return src


def check_c_file(path: pathlib.Path) -> List[str]:
    errs: List[str] = []
    cleaned = strip_c_comments_and_strings(path.read_text(encoding="utf-8"))
    for idx, line in enumerate(cleaned.splitlines(), start=1):
        m = C_PANIC_RE.search(line)
        if m:
            errs.append(
                f"{path}:{idx}: forbidden C termination/unwrap call {m.group(0)!r}"
            )
    return errs


def check_fortran_file(path: pathlib.Path, is_fixed: bool) -> List[str]:
    errs: List[str] = []
    for idx, raw in enumerate(path.read_text(encoding="utf-8").splitlines(), start=1):
        if is_fixed:
            if not raw or raw[0] in ("C", "c", "*"):
                continue
            code = raw[6:72]
        else:
            code = raw.split("!", 1)[0]
        # Strip quoted literals
        code = re.sub(r"'(?:''|[^'])*'", "''", code)
        code = re.sub(r'"(?:""|[^"])*"', '""', code)
        m = FORTRAN_PANIC_RE.search(code)
        if m:
            errs.append(
                f"{path}:{idx}: forbidden Fortran termination statement {m.group(0).strip()!r}"
            )
    return errs


def check_unified_error_return(bridge_hdr: pathlib.Path) -> List[str]:
    errs: List[str] = []
    cleaned = strip_c_comments_and_strings(bridge_hdr.read_text(encoding="utf-8"))
    for decl in cleaned.split(";"):
        decl_s = " ".join(decl.split())
        if "FAH_API" not in decl_s:
            continue
        if "FAH_NO_DISCARD" not in decl_s:
            errs.append(
                f"{bridge_hdr}: FAH_API function missing FAH_NO_DISCARD: {decl_s[:80]}"
            )
        if "fah_error_code_t" not in decl_s:
            errs.append(
                f"{bridge_hdr}: FAH_API function must return unified fah_error_code_t: {decl_s[:80]}"
            )
    return errs


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--check-all",
        action="store_true",
        help="Run all panic & error-enum lint checks",
    )
    args = parser.parse_args()
    if not args.check_all:
        parser.error("--check-all is required")

    repo = pathlib.Path(__file__).resolve().parent.parent
    errors: List[str] = []

    c_targets = (
        sorted((repo / "include").glob("*.h"))
        + sorted((repo / "src" / "ffi").glob("*.c"))
        + [
            repo / "generated" / "c_abstract_http_fortran_shim.h",
            repo / "generated" / "c_abstract_http_fortran_shim.c",
        ]
    )
    for cf in c_targets:
        if cf.exists():
            errors.extend(check_c_file(cf))

    for ff in sorted((repo / "src" / "legacy").glob("*.f")):
        errors.extend(check_fortran_file(ff, is_fixed=True))
    for inc in [
        repo / "include" / "fah77.inc",
        repo / "generated" / "c_abstract_http_f77.inc",
    ]:
        if inc.exists():
            errors.extend(check_fortran_file(inc, is_fixed=True))

    f90_targets = (
        sorted((repo / "src").rglob("*.f90"))
        + sorted((repo / "cmake").rglob("*.f90"))
        + [repo / "generated" / "c_abstract_http_bind_mod.f90"]
    )
    for f90 in f90_targets:
        if f90.exists():
            errors.extend(check_fortran_file(f90, is_fixed=False))

    errors.extend(check_unified_error_return(repo / "include" / "fah_c_bridge.h"))

    if errors:
        for e in errors:
            print(f"LINT ERROR: {e}", file=sys.stderr)
        return 1

    print(
        "fah_lint: OK (zero panics/unwraps/aborts; 100% unified fah_error_code_t discipline)"
    )
    return 0


if __name__ == "__main__":
    sys.exit(main())
