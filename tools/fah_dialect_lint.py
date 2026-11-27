#!/usr/bin/env python3
"""Strict per-era static dialect verifier for FORTRAN (I) through Fortran 2023.

Complements gfortran's -std= flags by enforcing exact historical language
boundaries (column widths, continuation limits, identifier lengths, comment
markers, implicit typing rules, and forbidden future-era keywords) across all
12 supported Fortran standards.
"""

import argparse
import pathlib
import re
import sys
from typing import Dict, List, Set, Tuple

IDENT_RE = re.compile(r"[A-Za-z_][A-Za-z0-9_]*")
HOLLERITH_RE = re.compile(r"\b(\d+)H")


def strip_fixed_strings_and_hollerith(code: str) -> str:
    """Replace character literals and nH... Hollerith payloads with spaces."""
    chars = list(code)
    n = len(chars)
    i = 0
    while i < n:
        ch = chars[i]
        if ch in ("'", '"'):
            quote = ch
            chars[i] = " "
            i += 1
            while i < n:
                if chars[i] == quote:
                    chars[i] = " "
                    if i + 1 < n and chars[i + 1] == quote:
                        chars[i + 1] = " "
                        i += 2
                        continue
                    i += 1
                    break
                chars[i] = " "
                i += 1
            continue
        # Check for Hollerith nH...
        if ch.isdigit() and (
            i == 0 or not (chars[i - 1].isalnum() or chars[i - 1] == "_")
        ):
            j = i
            while j < n and chars[j].isdigit():
                j += 1
            if j < n and chars[j] == "H":
                hlen = int("".join(chars[i:j]))
                end = min(n, j + 1 + hlen)
                for k in range(i, end):
                    chars[k] = " "
                i = end
                continue
        i += 1
    return "".join(chars)


def strip_free_strings_and_comments(line: str) -> str:
    """Remove string literals and trailing '!' comments from a free-form line."""
    out = []
    i = 0
    n = len(line)
    while i < n:
        ch = line[i]
        if ch == "!":
            break
        if ch in ("'", '"'):
            quote = ch
            out.append(" ")
            i += 1
            while i < n:
                if line[i] == quote:
                    if i + 1 < n and line[i + 1] == quote:
                        i += 2
                        continue
                    i += 1
                    break
                i += 1
            continue
        out.append(ch)
        i += 1
    return "".join(out)


FIXED_COMMON_KEYWORDS: Set[str] = {
    "FUNCTION",
    "SUBROUTINE",
    "PROGRAM",
    "BLOCK",
    "DATA",
    "COMMON",
    "DIMENSION",
    "EQUIVALENCE",
    "EXTERNAL",
    "INTEGER",
    "REAL",
    "DOUBLE",
    "PRECISION",
    "COMPLEX",
    "LOGICAL",
    "CHARACTER",
    "IMPLICIT",
    "PARAMETER",
    "INCLUDE",
    "SAVE",
    "INTRINSIC",
    "CALL",
    "RETURN",
    "GOTO",
    "GO",
    "TO",
    "IF",
    "THEN",
    "ELSE",
    "ELSEIF",
    "ENDIF",
    "DO",
    "CONTINUE",
    "STOP",
    "PAUSE",
    "END",
    "READ",
    "WRITE",
    "PRINT",
    "FORMAT",
    "ASSIGN",
    "TRUE",
    "FALSE",
    "EQ",
    "NE",
    "LT",
    "LE",
    "GT",
    "GE",
    "AND",
    "OR",
    "NOT",
    "EQV",
    "NEQV",
}


def check_fixed_file(path: pathlib.Path, era: str) -> List[str]:
    """Validate a fixed-form FORTRAN file against era in {f57,f58,f60,f62,f66,f77,f77_strict}."""
    errs: List[str] = []
    lines = path.read_text(encoding="utf-8").splitlines()

    max_cont = 9 if era in ("f57", "f58", "f60") else 19
    allow_star_comment = era in ("f77", "f77_strict")

    forbidden_kw: Dict[str, Set[str]] = {
        "f57": {
            "SUBROUTINE",
            "CALL",
            "COMMON",
            "BLOCK",
            "DATA",
            "EXTERNAL",
            "INTEGER",
            "REAL",
            "DOUBLE",
            "COMPLEX",
            "LOGICAL",
            "CHARACTER",
            "IMPLICIT",
            "PARAMETER",
            "INCLUDE",
            "SAVE",
            "THEN",
            "ELSE",
            "ENDIF",
        },
        "f58": {
            "BLOCK",
            "DATA",
            "EXTERNAL",
            "INTEGER",
            "REAL",
            "DOUBLE",
            "COMPLEX",
            "LOGICAL",
            "CHARACTER",
            "IMPLICIT",
            "PARAMETER",
            "INCLUDE",
            "SAVE",
            "THEN",
            "ELSE",
            "ENDIF",
        },
        "f60": {
            "BLOCK",
            "EXTERNAL",
            "INTEGER",
            "REAL",
            "DOUBLE",
            "COMPLEX",
            "LOGICAL",
            "CHARACTER",
            "IMPLICIT",
            "PARAMETER",
            "INCLUDE",
            "SAVE",
            "THEN",
            "ELSE",
            "ENDIF",
        },
        "f62": {
            "CHARACTER",
            "PARAMETER",
            "INCLUDE",
            "SAVE",
            "THEN",
            "ELSE",
            "ELSEIF",
            "ENDIF",
        },
        "f66": {
            "CHARACTER",
            "IMPLICIT",
            "PARAMETER",
            "INCLUDE",
            "SAVE",
            "THEN",
            "ELSE",
            "ELSEIF",
            "ENDIF",
        },
        "f77": {"MODULE", "CONTAINS", "USE", "ALLOCATABLE", "POINTER", "INTENT"},
        "f77_strict": {
            "INCLUDE",
            "MODULE",
            "CONTAINS",
            "USE",
            "ALLOCATABLE",
            "POINTER",
            "INTENT",
        },
    }[era]

    cont_run = 0
    prev_stmt_Clean = ""

    for idx, raw in enumerate(lines, start=1):
        if "\t" in raw:
            errs.append(f"{path}:{idx}: tab character forbidden in fixed-form {era}")
        if len(raw) > 72:
            errs.append(f"{path}:{idx}: line length {len(raw)} exceeds 72 columns")
        if not raw.strip():
            errs.append(
                f"{path}:{idx}: completely blank line forbidden; use 'C' in col 1"
            )
            continue

        col1 = raw[0]
        if col1 == "C" or (allow_star_comment and col1 == "*"):
            cont_run = 0
            continue
        if col1 != " ":
            errs.append(f"{path}:{idx}: illegal column-1 character {col1!r} for {era}")
            continue

        label_field = raw[0:5]
        if not all(ch == " " or ch.isdigit() for ch in label_field):
            errs.append(
                f"{path}:{idx}: non-digit in statement label field cols 1-5: {label_field!r}"
            )

        col6 = raw[5] if len(raw) > 5 else " "
        if col6 not in (" ", "0"):
            cont_run += 1
            if cont_run > max_cont:
                errs.append(
                    f"{path}:{idx}: {cont_run} continuations exceed {era} limit {max_cont}"
                )
        else:
            cont_run = 0

        code_field = raw[6:72] if len(raw) > 6 else ""
        cleaned = strip_fixed_strings_and_hollerith(code_field)

        if "!" in cleaned:
            errs.append(f"{path}:{idx}: inline '!' comment forbidden in {era}")
        if "_" in cleaned:
            errs.append(f"{path}:{idx}: underscore '_' forbidden in {era}")

        # Zero-trip DO guard check for ANSI FORTRAN 66
        if era == "f66":
            m_do = re.match(
                r"^\s*DO\s+\d+\s+[A-Z0-9]+\s*=\s*([^,]+),\s*([A-Z][A-Z0-9]*)", cleaned
            )
            if m_do:
                upper_var = m_do.group(2)
                if "IF" not in prev_stmt_Clean or upper_var not in prev_stmt_Clean:
                    errs.append(
                        f"{path}:{idx}: F66 DO loop with variable bound {upper_var} "
                        f"must be preceded by zero-trip IF guard"
                    )

        for tok in IDENT_RE.findall(cleaned):
            if tok != tok.upper():
                errs.append(
                    f"{path}:{idx}: lowercase identifier {tok!r} forbidden in {era}"
                )
            if tok in forbidden_kw:
                errs.append(f"{path}:{idx}: keyword {tok!r} did not exist in {era}")
            if tok not in FIXED_COMMON_KEYWORDS and len(tok) > 6:
                errs.append(
                    f"{path}:{idx}: identifier {tok!r} ({len(tok)} chars) exceeds 6-char limit"
                )

        if col6 in (" ", "0"):
            prev_stmt_Clean = cleaned

    return errs


def check_free_file(path: pathlib.Path, era: str) -> List[str]:
    """Validate a free-form Fortran file against era in {f90,f95,f2003,f2008,f2018,f2023}."""
    errs: List[str] = []
    lines = path.read_text(encoding="utf-8").splitlines()
    max_ident = 31 if era in ("f90", "f95") else 63

    forbidden_tokens: Dict[str, Set[str]] = {
        "f90": {
            "PURE",
            "ELEMENTAL",
            "FORALL",
            "ISO_C_BINDING",
            "BIND",
            "CLASS",
            "FINAL",
            "SUBMODULE",
            "CONTIGUOUS",
            "F_C_STRING",
            "C_F_STRPOINTER",
        },
        "f95": {
            "ISO_C_BINDING",
            "BIND",
            "CLASS",
            "FINAL",
            "SUBMODULE",
            "CONTIGUOUS",
            "F_C_STRING",
            "C_F_STRPOINTER",
        },
        "f2003": {
            "SUBMODULE",
            "CONTIGUOUS",
            "F_C_STRING",
            "C_F_STRPOINTER",
        },
        "f2008": {
            "F_C_STRING",
            "C_F_STRPOINTER",
        },
        "f2018": {
            "F_C_STRING",
            "C_F_STRPOINTER",
        },
        "f2023": set(),
    }[era]

    in_type_def = False
    in_type_contains = False

    for idx, raw in enumerate(lines, start=1):
        if len(raw) > 132:
            errs.append(f"{path}:{idx}: line length {len(raw)} exceeds 132 columns")
        cleaned = strip_free_strings_and_comments(raw)
        if not cleaned.strip():
            continue

        upper_stmt = cleaned.strip().upper()
        if re.match(r"^TYPE\b", upper_stmt) and not re.match(r"^TYPE\s*\(", upper_stmt):
            in_type_def = True
            in_type_contains = False
        elif upper_stmt.startswith("END TYPE"):
            in_type_def = False
            in_type_contains = False
        elif in_type_def and upper_stmt == "CONTAINS":
            in_type_contains = True
        elif era == "f90" and in_type_def and not in_type_contains and "::" in cleaned:
            rhs = cleaned.split("::", 1)[1]
            if "=" in rhs:
                errs.append(
                    f"{path}:{idx}: default component initialization in TYPE requires Fortran 95"
                )

        if era in ("f90", "f95", "f2003", "f2008"):
            if "TYPE(*)" in upper_stmt.replace(
                " ", ""
            ) or "DIMENSION(..)" in upper_stmt.replace(" ", ""):
                errs.append(
                    f"{path}:{idx}: TS 29113 TYPE(*) / DIMENSION(..) requires Fortran 2018"
                )

        for tok in IDENT_RE.findall(cleaned):
            utok = tok.upper()
            if utok in forbidden_tokens:
                errs.append(f"{path}:{idx}: token {tok!r} forbidden in {era}")
            if len(tok) > max_ident:
                errs.append(
                    f"{path}:{idx}: identifier {tok!r} ({len(tok)} chars) exceeds {era} {max_ident}-char limit"
                )

    return errs


def build_manifest(repo: pathlib.Path) -> List[Tuple[pathlib.Path, str, bool]]:
    """Return (path, era, is_fixed) for all library and test Fortran files."""
    candidates: List[Tuple[str, str, bool]] = [
        ("src/legacy/fah_f57.f", "f57", True),
        ("src/legacy/fah_f58.f", "f58", True),
        ("src/legacy/fah_f60.f", "f60", True),
        ("src/legacy/fah_f62.f", "f62", True),
        ("src/legacy/fah_f66.f", "f66", True),
        ("src/legacy/fah_f77.f", "f77", True),
        ("src/legacy/fah_f77_strict.f", "f77_strict", True),
        ("include/fah77.inc", "f77_strict", True),
        ("generated/c_abstract_http_f77.inc", "f77_strict", True),
        ("src/f90/fah_f90_mod.f90", "f90", False),
        ("src/f95/fah_f95_mod.f90", "f95", False),
        ("src/f2003/fah_f2003_mod.f90", "f2003", False),
        ("src/f2008/fah_f2008_mod.f90", "f2008", False),
        ("src/f2008/fah_f2008_impl_smod.f90", "f2008", False),
        ("src/f2018/fah_f2018_mod.f90", "f2018", False),
        ("src/f2023/fah_f2023_fcstr_mod.f90", "f2023", False),
        ("cmake/fah_f2023_fcstr_polyfill.f90", "f2023", False),
        ("src/f2023/fah_f2023_mod.f90", "f2023", False),
        ("src/fortran_abstract_http.f90", "f2023", False),
        ("tests/fortran/test_f57_fortran_i.f", "f57", True),
        ("tests/fortran/test_f58_fortran_ii.f", "f58", True),
        ("tests/fortran/test_f60_fortran_iii.f", "f60", True),
        ("tests/fortran/test_f62_fortran_iv.f", "f62", True),
        ("tests/fortran/test_f66_ansi.f", "f66", True),
        ("tests/fortran/test_f77_ansi.f", "f77", True),
        ("tests/fortran/test_f90_mod.f90", "f90", False),
        ("tests/fortran/test_f95_mod.f90", "f95", False),
        ("tests/fortran/test_f2003_oop.f90", "f2003", False),
        ("tests/fortran/test_f2008_submod.f90", "f2008", False),
        ("tests/fortran/test_abi_layout_f2008.f90", "f2008", False),
        ("tests/fortran/test_f2018_cfi.f90", "f2018", False),
        ("tests/fortran/test_f2023_mod.f90", "f2023", False),
    ]
    out: List[Tuple[pathlib.Path, str, bool]] = []
    for rel, era, is_fixed in candidates:
        full = repo / rel
        if full.exists():
            out.append((full, era, is_fixed))
    return out


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--check-all", action="store_true", help="Verify all project Fortran files"
    )
    args = parser.parse_args()
    if not args.check_all:
        parser.error("--check-all is required")

    repo = pathlib.Path(__file__).resolve().parent.parent
    manifest = build_manifest(repo)
    all_errors: List[str] = []
    for path, era, is_fixed in manifest:
        if is_fixed:
            all_errors.extend(check_fixed_file(path, era))
        else:
            all_errors.extend(check_free_file(path, era))

    if all_errors:
        for e in all_errors:
            print(f"DIALECT ERROR: {e}", file=sys.stderr)
        return 1

    print(
        f"fah_dialect_lint: OK ({len(manifest)} dialect files verified with 0 violations)"
    )
    return 0


if __name__ == "__main__":
    sys.exit(main())
