!> @file test_f2018_cfi.f90
!> @brief Integration test suite for ISO/IEC 1539-1:2018 (Fortran 2018 /
!> TS 29113) TYPE(*), DIMENSION(..), and ALLOCATABLE CFI_cdesc_t APIs.
PROGRAM test_f2018_cfi
  USE, INTRINSIC :: ISO_C_BINDING, ONLY: C_INT, C_INT8_T
  USE fah_f2003_mod
  USE fah_f2018_mod
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
  TYPE(fah_response) :: res
  TYPE(fah_error) :: err
  INTEGER(C_INT8_T) :: scalar_b
  INTEGER(C_INT8_T) :: mat(2, 4)
  INTEGER(C_INT8_T), ALLOCATABLE :: out_bytes(:)
  CHARACTER(LEN=128) :: url
  INTEGER :: ulen
  INTEGER(C_INT) :: port, dm

  err = fah03_global_init()
  IF (fah_mock_server_start(port) /= 0_C_INT) STOP 1
  err = cli%open()
  err = req%init()
  CALL fhmks7(INT(port), '/echo', url, ulen)
  err = req%set_target(1_C_INT, url)

  ! 1. Rank-0 scalar via TYPE(*), DIMENSION(..)
  scalar_b = 65_C_INT8_T
  err = fah18_req_set_body_any(req, scalar_b)
  err = cli%send(req, res)
  err = fah18_res_body_bytes(res, out_bytes)
  IF (.NOT. err%is_ok() .OR. SIZE(out_bytes) /= 1 .OR. &
      out_bytes(1) /= 65_C_INT8_T) STOP 1

  ! 2. Rank-2 non-contiguous strided slice mat(1:2, 1:4:2) -> 4 bytes
  mat = RESHAPE([10_C_INT8_T, 20_C_INT8_T, 99_C_INT8_T, 99_C_INT8_T, &
      30_C_INT8_T, 40_C_INT8_T, 99_C_INT8_T, 99_C_INT8_T], [2, 4])
  err = fah18_req_set_body_any(req, mat(1:2, 1:4:2))
  err = cli%send(req, res)
  err = fah18_res_body_bytes(res, out_bytes)
  IF (.NOT. err%is_ok() .OR. SIZE(out_bytes) /= 4) STOP 1
  IF (out_bytes(1) /= 10_C_INT8_T .OR. out_bytes(4) /= 40_C_INT8_T) &
      STOP 1

  err = res%free()
  err = req%free()
  err = cli%close()
  dm = fah_mock_server_stop(port)
  err = fah03_global_cleanup()
END PROGRAM test_f2018_cfi
