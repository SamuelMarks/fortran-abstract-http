#!/usr/bin/env python3
"""FORTRAN III (1958-1961) symbolic/Boolean card deck preprocessor and verifier.

Historical IBM 704/709 FORTRAN III supported:
  - Column 1 == 'B': Boolean octal arithmetic statements (treated as INTEGER
    word expressions on binary machines).
  - Column 1 == 'S': Symbolic machine-instruction cards (preserved as comments
    in portable FORTRAN III decks).
"""

import argparse
import pathlib
import sys


def preprocess_deck(text: str) -> str:
    """Transform a FORTRAN III .f3 deck into strict 72-column fixed-form."""
    out_lines = []
    for lineno, raw in enumerate(text.splitlines(), start=1):
        if len(raw) > 72:
            raise ValueError(f"line {lineno}: exceeds column 72 ({len(raw)} cols)")
        if not raw:
            out_lines.append("C")
            continue
        col1 = raw[0]
        if col1 == "B":
            # Strip 'B' indicator into blank col-1 standard statement card
            out_lines.append(" " + raw[1:])
        elif col1 == "S":
            # Convert non-portable IBM 704 SAP symbolic card into C comment
            out_lines.append("C [F-III SAP] " + raw[1:58])
        else:
            out_lines.append(raw)
    return "\n".join(out_lines) + "\n"


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--check", action="store_true", help="Verify src/legacy/fah_f60.f"
    )
    parser.add_argument("input", nargs="?", help="Optional .f3 input deck")
    parser.add_argument("-o", "--output", help="Optional .f output path")
    args = parser.parse_args()

    repo_root = pathlib.Path(__file__).resolve().parent.parent
    if args.check:
        target = repo_root / "src" / "legacy" / "fah_f60.f"
        src = target.read_text(encoding="utf-8")
        transformed = preprocess_deck(src)
        if transformed != src:
            print(
                "fah_f3_preproc: fah_f60.f contains unexpanded B/S cards",
                file=sys.stderr,
            )
            return 1
        print(f"fah_f3_preproc: OK ({target.relative_to(repo_root)})")
        return 0

    if not args.input:
        parser.error("either --check or input file is required")
    src_text = pathlib.Path(args.input).read_text(encoding="utf-8")
    out_text = preprocess_deck(src_text)
    if args.output:
        pathlib.Path(args.output).write_text(out_text, encoding="utf-8")
    else:
        sys.stdout.write(out_text)
    return 0


if __name__ == "__main__":
    sys.exit(main())
