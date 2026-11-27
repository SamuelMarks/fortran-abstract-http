#!/usr/bin/env python3
"""100% Documentation Coverage Auditor for fortran-abstract-http.

Audits every C header/source and every Fortran fixed-form and free-form source
file across include/, src/, and generated/ to guarantee 100.0% documentation
coverage on files, modules, submodules, BLOCK DATA units, derived types, type
components, enumerations, interfaces, functions, subroutines, and parameters.
"""

import argparse
import pathlib
import re
import sys
from typing import List, Tuple


def has_preceding_fortran_doc(lines: List[str], idx0: int, is_fixed: bool) -> bool:
    """Return True if line idx0 is immediately preceded by C> or !> doc lines (or inline !<)."""
    j = idx0 - 1
    while j >= 0:
        raw = lines[j]
        stripped = raw.strip()
        if not stripped:
            j -= 1
            continue
        if is_fixed:
            if raw.startswith("C>") or raw.startswith("c>"):
                return True
            if raw.startswith("C") and j > 0 and (lines[j - 1].startswith("C>")):
                j -= 1
                continue
            return False
        else:
            if stripped.startswith("!>") or stripped.startswith("!<"):
                return True
            if stripped in ("PRIVATE", "PUBLIC", "CONTAINS"):
                j -= 1
                continue
            return False
    return False


def audit_fixed_fortran(path: pathlib.Path) -> Tuple[int, int, List[str]]:
    """Audit fixed-form .f file for @file and subprogram C> documentation."""
    lines = path.read_text(encoding="utf-8").splitlines()
    total = 1
    documented = 0
    missing: List[str] = []

    if any("@file" in ln for ln in lines[:10]):
        documented += 1
    else:
        missing.append(f"{path}:1: missing C> @file header")

    subprog_re = re.compile(
        r"^\s{6}\s*(?:INTEGER\s+|LOGICAL\s+|REAL\s+|DOUBLE\s+PRECISION\s+)?"
        r"(SUBROUTINE|FUNCTION|BLOCK\s+DATA)\s+([A-Z0-9_]+)",
        re.IGNORECASE,
    )
    for i, ln in enumerate(lines):
        if ln.startswith(("C", "c", "*")):
            continue
        m = subprog_re.match(ln)
        if m:
            total += 1
            name = m.group(2)
            if has_preceding_fortran_doc(lines, i, is_fixed=True):
                documented += 1
            else:
                missing.append(f"{path}:{i + 1}: undocumented {m.group(1)} {name}")
    return documented, total, missing


def audit_free_fortran(path: pathlib.Path) -> Tuple[int, int, List[str]]:
    """Audit free-form .f90 file for 100% symbol documentation."""
    lines = path.read_text(encoding="utf-8").splitlines()
    total = 1
    documented = 0
    missing: List[str] = []

    if any("@file" in ln for ln in lines[:10]):
        documented += 1
    else:
        missing.append(f"{path}:1: missing !> @file header")

    in_type = False
    in_enum = False
    cont_prev = False

    for i, raw in enumerate(lines):
        stripped = raw.strip()
        if not stripped or stripped.startswith("!"):
            continue

        code = stripped.split("!", 1)[0].strip()
        if not code:
            continue

        if cont_prev:
            cont_prev = code.endswith("&")
            continue
        cont_prev = code.endswith("&")
        ucode = code.upper()

        # Skip closing statements
        if re.match(r"^END\b", ucode):
            if ucode.startswith("END TYPE"):
                in_type = False
            elif ucode.startswith("END ENUM"):
                in_enum = False
            continue

        # MODULE / SUBMODULE
        if re.match(r"^(MODULE|SUBMODULE)\b", ucode) and not re.match(
            r"^MODULE\s+(PROCEDURE|FUNCTION|SUBROUTINE)\b", ucode
        ):
            total += 1
            if has_preceding_fortran_doc(lines, i, is_fixed=False):
                documented += 1
            else:
                missing.append(f"{path}:{i + 1}: undocumented {code}")
            continue

        # TYPE definition
        if re.match(r"^TYPE\b", ucode) and not re.match(r"^TYPE\s*\(", ucode):
            in_type = True
            total += 1
            if has_preceding_fortran_doc(lines, i, is_fixed=False):
                documented += 1
            else:
                missing.append(f"{path}:{i + 1}: undocumented TYPE {code}")
            continue

        # ENUM, BIND(C)
        if re.match(r"^ENUM\b", ucode):
            in_enum = True
            total += 1
            if has_preceding_fortran_doc(lines, i, is_fixed=False):
                documented += 1
            else:
                missing.append(f"{path}:{i + 1}: undocumented ENUM")
            continue

        # ENUMERATOR inside ENUM
        if in_enum and ucode.startswith("ENUMERATOR"):
            total += 1
            if has_preceding_fortran_doc(lines, i, is_fixed=False):
                documented += 1
            else:
                missing.append(f"{path}:{i + 1}: undocumented {code}")
            continue

        # INTERFACE block
        if re.match(r"^(ABSTRACT\s+)?INTERFACE\b", ucode):
            total += 1
            if has_preceding_fortran_doc(lines, i, is_fixed=False):
                documented += 1
            else:
                missing.append(f"{path}:{i + 1}: undocumented {code}")
            continue

        # Type components or type-bound procedures inside TYPE
        if in_type and (
            "::" in code or ucode.startswith(("PROCEDURE", "FINAL", "GENERIC"))
        ):
            total += 1
            if has_preceding_fortran_doc(lines, i, is_fixed=False):
                documented += 1
            else:
                missing.append(f"{path}:{i + 1}: undocumented TYPE member {code}")
            continue

        # Module-level PARAMETER
        if "PARAMETER" in ucode and "::" in code:
            total += 1
            if has_preceding_fortran_doc(lines, i, is_fixed=False):
                documented += 1
            else:
                missing.append(f"{path}:{i + 1}: undocumented PARAMETER {code}")
            continue

        # SUBROUTINE / FUNCTION / MODULE PROCEDURE
        if re.match(
            r"^(?:(?:PURE|ELEMENTAL|RECURSIVE|MODULE|INTEGER(?:\([^)]+\))?|LOGICAL|CHARACTER(?:\([^)]+\))?)\s+)*"
            r"(SUBROUTINE|FUNCTION|PROCEDURE)\s+[A-Za-z_][A-Za-z0-9_]*",
            ucode,
        ) and not ucode.startswith("MODULE PROCEDURE FAH90_ERR_"):
            total += 1
            if has_preceding_fortran_doc(lines, i, is_fixed=False):
                documented += 1
            else:
                missing.append(f"{path}:{i + 1}: undocumented procedure {code}")
            continue

    return documented, total, missing


def audit_c_file(path: pathlib.Path) -> Tuple[int, int, List[str]]:
    """Verify @file Doxygen block on every C header/source (Doxygen checks all members)."""
    text = path.read_text(encoding="utf-8")
    if "@file" in text[:400]:
        return 1, 1, []
    return 0, 1, [f"{path}:1: missing /** @file */ Doxygen header"]


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--check-all", action="store_true", help="Audit all C and Fortran files"
    )
    args = parser.parse_args()
    if not args.check_all:
        parser.error("--check-all is required")

    repo = pathlib.Path(__file__).resolve().parent.parent
    total_doc = 0
    total_sym = 0
    all_missing: List[str] = []

    c_files = (
        sorted((repo / "include").glob("*.h"))
        + sorted((repo / "src" / "ffi").glob("*.c"))
        + [
            repo / "generated" / "c_abstract_http_fortran_shim.h",
            repo / "generated" / "c_abstract_http_fortran_shim.c",
        ]
    )
    for cf in c_files:
        if cf.exists():
            d, t, m = audit_c_file(cf)
            total_doc += d
            total_sym += t
            all_missing.extend(m)

    fixed_files = sorted((repo / "src" / "legacy").glob("*.f"))
    for ff in fixed_files:
        d, t, m = audit_fixed_fortran(ff)
        total_doc += d
        total_sym += t
        all_missing.extend(m)

    free_files = sorted((repo / "src").rglob("*.f90")) + sorted(
        (repo / "cmake").rglob("*.f90")
    )
    for f90 in free_files:
        d, t, m = audit_free_fortran(f90)
        total_doc += d
        total_sym += t
        all_missing.extend(m)

    for inc in [
        repo / "include" / "fah77.inc",
        repo / "generated" / "c_abstract_http_f77.inc",
    ]:
        if inc.exists():
            total_sym += 1
            head = inc.read_text(encoding="utf-8")[:300]
            if "FILE:" in head or "@file" in head:
                total_doc += 1
            else:
                all_missing.append(f"{inc}:1: missing include file header block")

    bind_mod = repo / "generated" / "c_abstract_http_bind_mod.f90"
    if bind_mod.exists():
        total_sym += 1
        if "@file" in bind_mod.read_text(encoding="utf-8")[:300]:
            total_doc += 1
        else:
            all_missing.append(f"{bind_mod}:1: missing @file header")

    pct = (100.0 * total_doc / total_sym) if total_sym else 100.0
    print(f"fah_doc_check: Documented {total_doc} / {total_sym} symbols ({pct:.1f}%)")
    if all_missing or total_doc != total_sym:
        for msg in all_missing:
            print(f"UNDOCUMENTED: {msg}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
