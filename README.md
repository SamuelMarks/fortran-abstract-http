# fortran-abstract-http

[![CI](https://github.com/SamuelMarks/fortran-abstract-http/actions/workflows/ci.yml/badge.svg)](.github/workflows/ci.yml)
[![Test Coverage](https://img.shields.io/badge/test%20coverage-100.0%25-brightgreen.svg)](#build-verification--coverage)
[![Doc Coverage](https://img.shields.io/badge/doc%20coverage-100.0%25-brightgreen.svg)](#verification--coverage)
[![C Standard](https://img.shields.io/badge/FFI%20Bridge-ISO%20C89-blue.svg)](ARCHITECTURE.md)
[![Fortran Standards](https://img.shields.io/badge/FORTRAN%20%2F%20Fortran-1957%E2%80%932023%20(12%20Standards)-purple.svg)](#12-standard-compatibility-matrix)
[![License](https://img.shields.io/badge/license-Apache--2.0%20OR%20MIT-informational.svg)](LICENSE-APACHE)

Multi-standard FORTRAN / Fortran FFI SDK exporting [`c-abstract-http`](../c-abstract-http) via [`cdd-c`](../cdd-c) across **every historical and modern specification of the language from 1957 to 2023**:

> **FORTRAN (I) · FORTRAN II · FORTRAN III · FORTRAN IV · FORTRAN 66 · FORTRAN 77 · Fortran 90 · Fortran 95 · Fortran 2003 · Fortran 2008 · Fortran 2018 · Fortran 2023**

---

## Key Capabilities

- **Full `c-abstract-http` Surface Area**:
  - Synchronous HTTP (`GET`, `POST`, `PUT`, `DELETE`, `PATCH`, `HEAD`, `OPTIONS`, `TRACE`, `QUERY`, `CONNECT`), custom headers, RFC 6265 `HttpCookieJar`, RFC 7578 `multipart/form-data`, chunked streaming uploads/downloads, automatic retries, and redirect following.
  - HTTP Basic, Bearer, and all **9 OAuth 2.0** flows (Password, Refresh Token, Authorization Code + PKCE, Client Credentials, RFC 7523 JWT Bearer, RFC 8628 Device Authorization & Token Poll, RFC 7009 Revocation, RFC 7662 Introspection, Auth URL builder, and localhost redirect capture).
  - Concurrent `HttpMultiRequest` batches & non-blocking/awaitable `HttpFuture` handles.
  - RFC 6455 **WebSockets** and W3C **Server-Sent Events (SSE)** (both blocking loops, non-blocking event-loop registration, and callback-less `FahPollQueue` pull loops for 1957–1961 FORTRAN I–III).
  - Execution & Concurrency Modalities: `ModalityEventLoop`, `AbstractHttpMutex`, `AbstractHttpCond`, `AbstractHttpThreadPool`, Thread-Local Storage (`TLS`), stackful `AbstractHttpCoroutine`, `AbstractHttpActor` / `AbstractHttpMessageBus`, OS IPC pipes + binary request/response serialization, and `c-multiplatform` (`CMP`) integration.
- **Zero Panics / Zero `unwrap`**: Library units never invoke `STOP`, `ERROR STOP`, `PAUSE`, `abort()`, `exit()`, or `assert()`. Every fallible entry point returns a single unified error enum (`fah_error_code_t` / `fah_error` / `fah90_error_t` / `fah95_error_t` / `IHE*`).
- **Strict ISO C89 FFI Bridge**: 100% of C bridge sources (`src/ffi/*.c`) compile cleanly under `-std=c89 -pedantic -Wall -Wextra -Werror`, including a pure C89 `CFI_cdesc_t` descriptor shim verified byte-for-byte against `<ISO_Fortran_binding.h>`.

---

## 12-Standard Compatibility Matrix

| # | Standard | Year | Source Form | Max ID | String / Payload Representation | SDK Layer & Entry Prefix | Compiler Verification |
| :- | :--- | :--- | :--- | :--- | :--- | :--- | :--- |
| 1 | **FORTRAN (I)** | 1957 | Fixed (1–72) | 6 | Unpacked `INTEGER` ASCII arrays | `src/legacy/fah_f57.f` (`IH1*`) | `-std=legacy` + `fah_dialect_lint.py` (`f57`) |
| 2 | **FORTRAN II** | 1958 | Fixed (1–72) | 6 | Unpacked `INTEGER` + `COMMON /FAH2CM/` | `src/legacy/fah_f58.f` (`FH2*`) | `-std=legacy` + `fah_dialect_lint.py` (`f58`) |
| 3 | **FORTRAN III** | 1958–61 | Fixed (1–72) | 6 | 4-char Hollerith words + `FahPollQueue` | `src/legacy/fah_f60.f` (`FH3*`) | `-std=legacy` + `fah_f3_preproc.py` + `f60` |
| 4 | **FORTRAN IV** | 1962 | Fixed (1–72) | 6 | `LOGICAL`, `BLOCK DATA`, `EXTERNAL` callbacks | `src/legacy/fah_f62.f` (`FH4*`) | `-std=legacy` + `fah_dialect_lint.py` (`f62`) |
| 5 | **FORTRAN 66** | 1966 | Fixed (1–72) | 6 | ANSI X3.9-1966 `4H` Hollerith + guarded `DO` | `src/legacy/fah_f66.f` (`FH6*`) | `-std=legacy` + `fah_dialect_lint.py` (`f66`) |
| 6 | **FORTRAN 77** | 1978 | Fixed (1–72) | 6 | `CHARACTER*(*)` + `INCLUDE 'fah77.inc'` | `src/legacy/fah_f77.f` (`FH7*`, `FHS*`) | `-std=legacy` + `fah_dialect_lint.py` (`f77`) |
| 7 | **Fortran 90** | 1991 | Free (1–132) | 31 | `MODULE`, private `TYPE` handles, `OPTIONAL` | `src/f90/fah_f90_mod.f90` (`fah90_*`) | `-std=f95` + `fah_dialect_lint.py` (`f90`) |
| 8 | **Fortran 95** | 1997 | Free (1–132) | 31 | Default init, `PURE` & `ELEMENTAL` | `src/f95/fah_f95_mod.f90` (`fah95_*`) | `-std=f95` + `fah_dialect_lint.py` (`f95`) |
| 9 | **Fortran 2003** | 2004 | Free (1–132) | 63 | `ISO_C_BINDING`, OOP classes, `FINAL` RAII | `src/f2003/fah_f2003_mod.f90` (`fah_*`) | `-std=f2003 -pedantic` + `f2003` |
| 10 | **Fortran 2008** | 2010 | Free (1–132) | 63 | `SUBMODULE`, `CONTIGUOUS` zero-copy `INT8(:)` | `src/f2008/fah_f2008_mod.f90` (`fah08_*`) | `-std=f2008 -pedantic` + `f2008` |
| 11 | **Fortran 2018** | 2018 | Free (1–132) | 63 | TS 29113 `TYPE(*)`, `DIMENSION(..)`, `CFI_cdesc_t` | `src/f2018/fah_f2018_mod.f90` (`fah18_*`) | `-std=f2018 -pedantic` + `f2018` |
| 12 | **Fortran 2023** | 2023 | Free (1–132) | 63 | `F_C_STRING` (`ASIS`) + zero-copy `LEN=:` ptr | `src/f2023/fah_f2023_mod.f90` (`fah23_*`) | `-std=f2023 -pedantic` + `f2023` |

---

## Quickstart

### Modern Fortran (Fortran 2003 – 2023)

```fortran
PROGRAM demo
  USE fortran_abstract_http
  IMPLICIT NONE
  TYPE(fah_client) :: cli
  TYPE(fah_response) :: res
  TYPE(fah_error) :: err
  CHARACTER(LEN=:), POINTER :: body_view

  err = fah03_global_init()
  err = cli%open()
  err = fah23_get(cli, 'http://example.com/', res)
  IF (err%is_ok()) THEN
    err = fah23_response_strpointer(res, body_view)
  END IF
  err = fah03_global_cleanup()
END PROGRAM demo
```

### Classical FORTRAN (I) (1957)

```fortran
C     IBM 704 FORTRAN (I) — IMPLICIT I-N TYPING & ARITHMETIC IF
      DIMENSION IURL(32), IBUF(256)
      IRC = IH1INI(0)
      IF (IRC) 90, 10, 90
   10 IRC = IH1CLI(1, ICLI)
      IRC = IH1GET(ICLI, IURL, NURL, IRES, ISTS, NBDY)
      IRC = IH1BDY(IRES, IBUF, 256, NOUT)
      IRC = IH1FIN(ICLI)
   90 IRC = IH1CLN(0)
      END
```

See [`USAGE.md`](USAGE.md) for complete recipes across all 12 standards and [`ARCHITECTURE.md`](ARCHITECTURE.md) for internal FFI design details.

---

## Build, Verification & Coverage

```sh
# Regenerate cdd-c ISO_C_BINDING module & C89 ABI shims deterministically
make regen-ffi

# Run clang-format, 12-era dialect lint, Doxygen, 100% doc-check, and panic lint
make lint

# Build all 12 Fortran layers + C89 bridge and run all 14 CTest suites
make test

# Enforce 100.0% line & function coverage across C89 and Fortran sources
make coverage

# Full end-to-end quality gate
make check-all
```

---

## License

Licensed under either of

- Apache License, Version 2.0 ([LICENSE-APACHE](LICENSE-APACHE) or <https://www.apache.org/licenses/Apache-2.0>)
- MIT license ([LICENSE-MIT](LICENSE-MIT) or <https://opensource.org/licenses/MIT>)

at your option.

### Contribution

Unless you explicitly state otherwise, any contribution intentionally submitted
for inclusion in the work by you, as defined in the Apache-2.0 license, shall be
dual licensed as above, without any additional terms or conditions.
