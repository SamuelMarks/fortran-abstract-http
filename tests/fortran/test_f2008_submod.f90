!> @file test_f2008_submod.f90
!> @brief Integration test suite for ISO/IEC 1539-1:2010 (Fortran 2008)
!> SUBMODULE and CONTIGUOUS zero-copy byte pointer views.
PROGRAM test_f2008_submod
  USE, INTRINSIC :: ISO_FORTRAN_ENV, ONLY: INT8, INT64
  USE, INTRINSIC :: ISO_C_BINDING, ONLY: C_INT
  USE fah_f2003_mod
  USE fah_f2008_mod
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

  TYPE(fah_client) :: cli
  TYPE(fah_request) :: req
  TYPE(fah_response) :: res, uninit_res
  TYPE(fah_error) :: err
  INTEGER(INT8), POINTER, CONTIGUOUS :: view(:)
  INTEGER(INT64) :: rq_sz, rs_sz
  CHARACTER(LEN=128) :: url
  INTEGER :: ulen
  INTEGER(C_INT) :: port, dm

  err = fah03_global_init()
  IF (.NOT. err%is_ok()) STOP 1
  err = fah08_verify_c_abi_sizes(rq_sz, rs_sz)
  IF (.NOT. err%is_ok() .OR. rq_sz <= 0_INT64 .OR. rs_sz <= 0_INT64) &
      STOP 1

  IF (fah_mock_server_start(port) /= 0_C_INT) STOP 1
  err = cli%open()
  err = req%init()

  ! 1. Non-empty CONTIGUOUS zero-copy view (/hello -> 13 bytes, 'H' == 72)
  CALL fhmks7(INT(port), '/hello', url, ulen)
  err = req%set_target(0_C_INT, url)
  err = cli%send(req, res)
  err = fah08_response_body_view(res, view)
  IF (.NOT. err%is_ok() .OR. SIZE(view) /= 13 .OR. view(1) /= 72_INT8) &
      STOP 1

  ! 2. Zero-length body (/empty -> 204 No Content -> SIZE(view) == 0)
  CALL fhmks7(INT(port), '/empty', url, ulen)
  err = req%set_target(0_C_INT, url)
  err = cli%send(req, res)
  err = fah08_response_body_view(res, view)
  IF (.NOT. err%is_ok() .OR. SIZE(view) /= 0) STOP 1

  ! 3. Null handle error path
  err = fah08_response_body_view(uninit_res, view)
  IF (err%is_ok() .OR. ASSOCIATED(view)) STOP 1

  err = res%free()
  err = req%free()
  err = cli%close()
  dm = fah_mock_server_stop(port)
  err = fah03_global_cleanup()
END PROGRAM test_f2008_submod
