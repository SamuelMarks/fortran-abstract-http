!> @file test_f2023_mod.f90
!> @brief Integration test suite for ISO/IEC 1539-1:2023 (Fortran 2023)
!> F_C_STRING, deferred-length pointer views, and umbrella SDK module.
PROGRAM test_f2023_mod
  USE, INTRINSIC :: ISO_C_BINDING, ONLY: C_INT, C_CHAR
  USE fortran_abstract_http
  IMPLICIT NONE

  INTERFACE
    INTEGER(C_INT) FUNCTION fah_mock_server_start(out_port) &
        BIND(C, NAME="fah_mock_server_start")
      IMPORT :: C_INT
      INTEGER(C_INT), INTENT(OUT) :: out_port
    END FUNCTION fah_mock_server_start
    INTEGER(C_INT) FUNCTION fah_mock_server_stop(port) &
        BIND(C, NAME="fah_mock_server_stop")
      IMPORT :: C_INT
      INTEGER(C_INT), VALUE :: port
    END FUNCTION fah_mock_server_stop
    SUBROUTINE fhmks7(port, path, out_url, out_len)
      INTEGER, INTENT(IN) :: port
      CHARACTER(LEN=*), INTENT(IN) :: path
      CHARACTER(LEN=*), INTENT(OUT) :: out_url
      INTEGER, INTENT(OUT) :: out_len
    END SUBROUTINE fhmks7
  END INTERFACE

  TYPE(fah_client) :: cli, closed_cli
  TYPE(fah_response) :: res, bad_res
  TYPE(fah_error) :: err
  CHARACTER(LEN=:, KIND=C_CHAR), ALLOCATABLE :: cs1, cs2
  CHARACTER(LEN=:, KIND=C_CHAR), POINTER :: sview
  CHARACTER(LEN=128) :: u_hello, u_echo, u_empty
  CHARACTER(LEN=:), ALLOCATABLE :: echo_auth
  INTEGER :: ulen
  INTEGER(C_INT) :: port, dm

  IF (fah_sdk_version() /= '0.1.0') STOP 1
  cs1 = fah23_to_cstring('abc  ')
  cs2 = fah23_to_cstring('abc  ', asis=.TRUE.)
  IF (LEN(cs1) /= 4 .OR. LEN(cs2) /= 6) STOP 1

  err = fah03_global_init()
  IF (fah_mock_server_start(port) /= 0_C_INT) STOP 1
  err = cli%open()

  ! 1. fah23_get with & without bearer_token + zero-copy strpointer
  CALL fhmks7(INT(port), '/hello', u_hello, ulen)
  err = fah23_get(cli, u_hello, res)
  IF (.NOT. err%is_ok()) STOP 1
  err = fah23_response_strpointer(res, sview)
  IF (.NOT. err%is_ok() .OR. sview /= 'Hello FORTRAN') STOP 1

  CALL fhmks7(INT(port), '/echo', u_echo, ulen)
  err = fah23_get(cli, u_echo, res, bearer_token='f2023_tok')
  err = res%header('X-Echo-Auth', echo_auth)
  IF (.NOT. err%is_ok() .OR. echo_auth /= 'Bearer f2023_tok') STOP 1

  ! 2. fah23_post
  err = fah23_post(cli, u_echo, 'text/plain  ', 'Fortran 2023!', res)
  err = fah23_response_strpointer(res, sview)
  IF (.NOT. err%is_ok() .OR. sview /= 'Fortran 2023!') STOP 1

  ! 3. Empty 204 body view + uninitialized response error path
  CALL fhmks7(INT(port), '/empty', u_empty, ulen)
  err = fah23_get(cli, u_empty, res)
  err = fah23_response_strpointer(res, sview)
  IF (.NOT. err%is_ok() .OR. LEN(sview) /= 0) STOP 1

  err = fah23_response_strpointer(bad_res, sview)
  IF (err%is_ok() .OR. ASSOCIATED(sview)) STOP 1

  err = fah23_get(closed_cli, u_hello, res)
  IF (err%is_ok()) STOP 1

  err = res%free()
  err = cli%close()
  dm = fah_mock_server_stop(port)
  err = fah03_global_cleanup()
END PROGRAM test_f2023_mod
