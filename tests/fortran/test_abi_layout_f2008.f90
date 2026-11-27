!> @file test_abi_layout_f2008.f90
!> @brief Fortran 2008 C_SIZEOF verification of generated BIND(C) types
!> against strict C89 sizeof queries in c_abstract_http_fortran_shim.c.

!> @brief Compare C_SIZEOF of all 8 core BIND(C) structs with C sizeof.
!> @return 0 on exact byte match, non-zero on mismatch.
INTEGER(C_INT) FUNCTION fah_test_f2008_abi_sizes() &
    BIND(C, NAME="fah_test_f2008_abi_sizes")
  USE, INTRINSIC :: ISO_C_BINDING, ONLY: C_INT, C_SIZE_T, C_SIZEOF
  USE, INTRINSIC :: ISO_FORTRAN_ENV, ONLY: INT64
  USE c_abstract_http_bind_mod
  USE fah_f2003_mod, ONLY: fah_error
  USE fah_f2008_mod, ONLY: fah08_verify_c_abi_sizes
  IMPLICIT NONE

  INTERFACE
    FUNCTION fah_abi_sizeof_http_config() &
        BIND(C, NAME="fah_abi_sizeof_http_config") RESULT(sz)
      IMPORT :: C_SIZE_T
      INTEGER(C_SIZE_T) :: sz
    END FUNCTION fah_abi_sizeof_http_config
    FUNCTION fah_abi_sizeof_http_client() &
        BIND(C, NAME="fah_abi_sizeof_http_client") RESULT(sz)
      IMPORT :: C_SIZE_T
      INTEGER(C_SIZE_T) :: sz
    END FUNCTION fah_abi_sizeof_http_client
    FUNCTION fah_abi_sizeof_http_headers() &
        BIND(C, NAME="fah_abi_sizeof_http_headers") RESULT(sz)
      IMPORT :: C_SIZE_T
      INTEGER(C_SIZE_T) :: sz
    END FUNCTION fah_abi_sizeof_http_headers
    FUNCTION fah_abi_sizeof_http_cookie_jar() &
        BIND(C, NAME="fah_abi_sizeof_http_cookie_jar") RESULT(sz)
      IMPORT :: C_SIZE_T
      INTEGER(C_SIZE_T) :: sz
    END FUNCTION fah_abi_sizeof_http_cookie_jar
    FUNCTION fah_abi_sizeof_http_request() &
        BIND(C, NAME="fah_abi_sizeof_http_request") RESULT(sz)
      IMPORT :: C_SIZE_T
      INTEGER(C_SIZE_T) :: sz
    END FUNCTION fah_abi_sizeof_http_request
    FUNCTION fah_abi_sizeof_http_response() &
        BIND(C, NAME="fah_abi_sizeof_http_response") RESULT(sz)
      IMPORT :: C_SIZE_T
      INTEGER(C_SIZE_T) :: sz
    END FUNCTION fah_abi_sizeof_http_response
    FUNCTION fah_abi_sizeof_http_multi_request() &
        BIND(C, NAME="fah_abi_sizeof_http_multi_request") RESULT(sz)
      IMPORT :: C_SIZE_T
      INTEGER(C_SIZE_T) :: sz
    END FUNCTION fah_abi_sizeof_http_multi_request
    FUNCTION fah_abi_sizeof_http_future() &
        BIND(C, NAME="fah_abi_sizeof_http_future") RESULT(sz)
      IMPORT :: C_SIZE_T
      INTEGER(C_SIZE_T) :: sz
    END FUNCTION fah_abi_sizeof_http_future
  END INTERFACE

  TYPE(httpconfig) :: v_cfg
  TYPE(httpclient) :: v_cli
  TYPE(httpheaders) :: v_hdr
  TYPE(httpcookiejar) :: v_jar
  TYPE(httprequest) :: v_req
  TYPE(httpresponse) :: v_res
  TYPE(httpmultirequest) :: v_mul
  TYPE(httpfuture) :: v_fut
  INTEGER(INT64) :: rq_b, rs_b
  TYPE(fah_error) :: err

  fah_test_f2008_abi_sizes = 0_C_INT
  IF (C_SIZEOF(v_cfg) /= fah_abi_sizeof_http_config()) THEN
    fah_test_f2008_abi_sizes = 1_C_INT
    RETURN
  END IF
  IF (C_SIZEOF(v_cli) /= fah_abi_sizeof_http_client()) THEN
    fah_test_f2008_abi_sizes = 2_C_INT
    RETURN
  END IF
  IF (C_SIZEOF(v_hdr) /= fah_abi_sizeof_http_headers()) THEN
    fah_test_f2008_abi_sizes = 3_C_INT
    RETURN
  END IF
  IF (C_SIZEOF(v_jar) /= fah_abi_sizeof_http_cookie_jar()) THEN
    fah_test_f2008_abi_sizes = 4_C_INT
    RETURN
  END IF
  IF (C_SIZEOF(v_req) /= fah_abi_sizeof_http_request()) THEN
    fah_test_f2008_abi_sizes = 5_C_INT
    RETURN
  END IF
  IF (C_SIZEOF(v_res) /= fah_abi_sizeof_http_response()) THEN
    fah_test_f2008_abi_sizes = 6_C_INT
    RETURN
  END IF
  IF (C_SIZEOF(v_mul) /= fah_abi_sizeof_http_multi_request()) THEN
    fah_test_f2008_abi_sizes = 7_C_INT
    RETURN
  END IF
  IF (C_SIZEOF(v_fut) /= fah_abi_sizeof_http_future()) THEN
    fah_test_f2008_abi_sizes = 8_C_INT
    RETURN
  END IF

  err = fah08_verify_c_abi_sizes(rq_b, rs_b)
  IF (.NOT. err%is_ok()) fah_test_f2008_abi_sizes = 9_C_INT
END FUNCTION fah_test_f2008_abi_sizes
