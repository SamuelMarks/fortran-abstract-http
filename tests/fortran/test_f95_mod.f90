!> @file test_f95_mod.f90
!> @brief Integration test suite for ISO/IEC 1539-1:1997 (Fortran 95) SDK.
PROGRAM test_f95_mod
  USE fah_f90_mod, ONLY: fah90_global_init, fah90_global_cleanup, &
      fah90_error_t
  USE fah_f95_mod
  IMPLICIT NONE

  INTERFACE
    INTEGER FUNCTION ihmkst(out_port)
      INTEGER, INTENT(OUT) :: out_port
    END FUNCTION ihmkst
    INTEGER FUNCTION ihmksp(port)
      INTEGER, INTENT(IN) :: port
    END FUNCTION ihmksp
    SUBROUTINE fhmks7(port, path, out_url, out_len)
      INTEGER, INTENT(IN) :: port
      CHARACTER(LEN=*), INTENT(IN) :: path
      CHARACTER(LEN=*), INTENT(OUT) :: out_url
      INTEGER, INTENT(OUT) :: out_len
    END SUBROUTINE fhmks7
  END INTERFACE

  TYPE(fah95_client_t) :: cli
  TYPE(fah95_error_t) :: errs(2), err
  TYPE(fah90_error_t) :: e90
  LOGICAL :: ok_mask(2), two_mask(3), red_mask(3)
  INTEGER :: codes(2), statuses(3)
  CHARACTER(LEN=128) :: url, body_buf
  INTEGER :: port, ulen, st, blen, dm

  CALL fah90_global_init(e90)
  IF (ihmkst(port) /= 0) STOP 1

  ! Inactive client guard checks
  IF (fah95_client_is_active(cli)) STOP 1
  CALL fah95_get_text(cli, 'http://127.0.0.1/', body_buf, st, blen, err)
  IF (fah95_err_is_ok(err)) STOP 1
  CALL fah95_client_close(cli, errs(2))
  IF (fah95_err_is_ok(errs(2))) STOP 1

  ! ELEMENTAL array operations
  ok_mask = fah95_err_is_ok(errs)
  codes = fah95_err_code(errs)
  IF (.NOT. ok_mask(1) .OR. ok_mask(2) .OR. codes(1) /= 0) STOP 1

  statuses = (/ 200, 302, 404 /)
  two_mask = fah95_status_is_2xx(statuses)
  red_mask = fah95_status_is_redirect(statuses)
  IF (.NOT. two_mask(1) .OR. two_mask(2) .OR. .NOT. red_mask(2)) STOP 1

  CALL fah95_client_open(cli, err, use_raw=.TRUE.)
  IF (.NOT. fah95_err_is_ok(err) .OR. .NOT. fah95_client_is_active(cli)) &
      STOP 1

  CALL fhmks7(port, '/hello', url, ulen)
  CALL fah95_get_text(cli, url, body_buf, st, blen, err)
  IF (.NOT. fah95_err_is_ok(err) .OR. st /= 200 .OR. blen /= 13) STOP 1

  CALL fah95_client_close(cli, err)
  IF (.NOT. fah95_err_is_ok(err) .OR. fah95_client_is_active(cli)) STOP 1

  dm = ihmksp(port)
  CALL fah90_global_cleanup(e90)
END PROGRAM test_f95_mod
