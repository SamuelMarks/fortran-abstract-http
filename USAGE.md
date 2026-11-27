# Usage Guide Across All 12 Fortran Standards

## 1. FORTRAN (I) (1957) — `fah_f57.f`
Uses external `IH1*` functions, implicit `I-N` integer typing, unpacked ASCII `INTEGER` arrays, and 3-way arithmetic `IF`:
```fortran
      DIMENSION IURL(64), IBUF(256)
      IRC = IH1INI(0)
      IRC = IH1CLI(1, ICLI)
      IRC = IH1GET(ICLI, IURL, NURL, IRES, ISTS, NBDY)
      IF (IRC) 90, 10, 90
   10 IRC = IH1BDY(IRES, IBUF, 256, NOUT)
   90 IRC = IH1FIN(ICLI)
      IRC = IH1CLN(0)
      END
```

## 2. FORTRAN II (1958) — `fah_f58.f`
Introduces `SUBROUTINE`, `CALL`, `RETURN`, and status tracking via `COMMON /FAH2CM/ LASTER, LASTST, NREQS`:
```fortran
      COMMON /FAH2CM/ LASTER, LASTST, NREQS
      DIMENSION IURL(64), IKEY(8), IVAL(8), IBDY(8), IOUT(256)
      CALL FH2INI(IERR)
      CALL FH2CLI(1, ICLI, IERR)
      CALL FH2REQ(ICLI, 0, IURL, NURL, IKEY, 0, IVAL, 0, IBDY, 0,
     1 IOUT, 256, ISTS, NOUT, IERR)
      CALL FH2FIN(ICLI, IERR)
      CALL FH2CLN(IERR)
      END
```

## 3. FORTRAN III (1958–1961) — `fah_f60.f`
Adds Boolean version bitmasks (`FH3BMS`), packed 4-char Hollerith words (`FH3HOL`), and `FahPollQueue` chunk streaming (`FH3POL`):
```fortran
      DIMENSION IHURL(16), IBUF(256)
      CALL FH3BMS(0, 1, 1, 0, IMASK, IERR)
      CALL FH3POL(ICLI, IHURL, NURL, IBUF, 256, ISTS, NCHKS, NTOT, IERR)
```

## 4. FORTRAN IV (1962) — `fah_f62.f`
Adds explicit `INTEGER`/`LOGICAL`, `BLOCK DATA FAH4BD`, and `EXTERNAL` callbacks for streaming HTTP (`FH4STR`), WebSockets (`FH4WSS`), and SSE (`FH4SSE`):
```fortran
      EXTERNAL ONCHUK, ONPROG
      CALL FH4CFG(5000, .FALSE., .TRUE., 2, ICFG, IERR)
      CALL FH4STR(ICLI, IHURL, NURL, ONCHUK, 1, ONPROG, 1, ISTS, IERR)
```

## 5. FORTRAN 66 (ANSI X3.9-1966) — `fah_f66.f`
Strict standard-portable Hollerith HTTP (`FH6REQ`), `EXTERNAL` chunked upload (`FH6UPL`), and error classification (`FH6ERR`):
```fortran
      CALL FH6ERR(IERR, ICAT, LRETRY, IMSG, 128, NLEN, IRC)
```

## 6. FORTRAN 77 (ANSI X3.9-1978) — `fah_f77.f` & `fah_f77_strict.f`
Native `CHARACTER*(*)` strings, `INCLUDE 'fah77.inc'`, Multipart (`FH7RPT`), OAuth2 (`FH7OA2`), Multi-Requests (`FH7MUL`), and Modalities (`FH7MOD`):
```fortran
      INCLUDE 'fah77.inc'
      CHARACTER*256 OUTB
      CALL FH7INI(IERR)
      CALL FH7CLC(0, 1, ICLI, IERR)
      CALL FHSGET(ICLI, 'http://example.com/', OUTB, ISTS, NLEN, IERR)
      CALL FH7CLF(ICLI, IERR)
      CALL FH7CLN(IERR)
```

## 7. Fortran 90 (ISO/IEC 1539:1991) — `fah_f90_mod`
Encapsulated private handle derived types (`fah90_client_t`, `fah90_error_t`), `OPTIONAL` parameters, `OPERATOR(==)`/`(/=)`, and concurrent `fah90_send_multi_two`:
```fortran
USE fah_f90_mod
TYPE(fah90_client_t) :: cli
TYPE(fah90_error_t) :: err
CALL fah90_client_create(cli, err, timeout_ms=3000, follow_redirects=.TRUE.)
IF (err == FAH90_OK) CALL fah90_client_destroy(cli, err)
```

## 8. Fortran 95 (ISO/IEC 1539-1:1997) — `fah_f95_mod`
Default-initialized `fah95_error_t`, `PURE` observers, and array-capable `ELEMENTAL` predicates (`fah95_status_is_2xx`, `fah95_err_is_ok`):
```fortran
USE fah_f95_mod
LOGICAL :: ok(3)
ok = fah95_status_is_2xx((/ 200, 204, 503 /))
```

## 9. Fortran 2003 (ISO/IEC 1539-1:2004) — `fah_f2003_mod`
Object-Oriented `fah_client`, `fah_request`, `fah_response`, `fah_error` with deferred-length `CHARACTER(LEN=:), ALLOCATABLE` and automatic `FINAL` RAII destructors:
```fortran
USE fah_f2003_mod
TYPE(fah_client) :: cli
TYPE(fah_request) :: req
TYPE(fah_response) :: res
TYPE(fah_error) :: err
CHARACTER(LEN=:), ALLOCATABLE :: body
err = cli%open()
err = req%init()
err = req%set_target(0, 'http://example.com/')
err = cli%send(req, res)
err = res%body_string(body)
```

## 10. Fortran 2008 (ISO/IEC 1539-1:2010) — `fah_f2008_mod`
`SUBMODULE` separation and zero-copy `INTEGER(INT8), POINTER, CONTIGUOUS :: view(:)` into C response memory:
```fortran
USE fah_f2008_mod
INTEGER(INT8), POINTER, CONTIGUOUS :: bytes(:)
err = fah08_response_body_view(res, bytes)
```

## 11. Fortran 2018 (ISO/IEC 1539-1:2018) — `fah_f2018_mod`
Assumed-type, assumed-rank `TYPE(*), DIMENSION(..)` request payloads (scalars, arrays, and non-contiguous strided slices) + `CFI_allocate` response byte arrays:
```fortran
USE fah_f2018_mod
INTEGER(C_INT8_T) :: matrix(4, 4)
INTEGER(C_INT8_T), ALLOCATABLE :: resp_bytes(:)
err = fah18_req_set_body_any(req, matrix(1:4:2, 1:4:2))
err = fah18_res_body_bytes(res, resp_bytes)
```

## 12. Fortran 2023 (ISO/IEC 1539-1:2023) — `fah_f2023_mod` & `fortran_abstract_http`
Intrinsic `F_C_STRING` (with optional `ASIS=.TRUE.`) and zero-copy deferred-length `CHARACTER(LEN=:), POINTER` views:
```fortran
USE fortran_abstract_http
CHARACTER(LEN=:), POINTER :: str_view
err = fah23_post(cli, 'http://example.com/echo', 'text/plain', 'Hello 2023', res)
err = fah23_response_strpointer(res, str_view)
```
