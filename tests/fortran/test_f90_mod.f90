!> @file test_f90_mod.f90
!> @brief Integration test suite for ISO/IEC 1539:1991 (Fortran 90) SDK.
PROGRAM test_f90_mod
  USE fah_f90_mod
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

  TYPE(fah90_client_t) :: cli, cli2
  TYPE(fah90_request_t) :: req1, req2
  TYPE(fah90_response_t) :: res1, res2
  TYPE(fah90_error_t) :: err, err2
  CHARACTER(LEN=128) :: url1, url2, body_out
  INTEGER :: port, ulen, st, blen, dm

  CALL fah90_global_init(err)
  IF (err /= FAH90_OK) STOP 1
  CALL fah90_make_error(FAH90_ERR_INVAL, 'test', err2)
  IF (err == err2 .OR. .NOT. (err /= err2)) STOP 1

  IF (ihmkst(port) /= 0) STOP 1
  CALL fah90_client_create(cli, err, timeout_ms=4000, &
      verify_peer=.TRUE., follow_redirects=.FALSE., use_raw=.TRUE.)
  IF (.NOT. (err == FAH90_OK)) STOP 1
  IF (fah90_client_raw_id(cli) <= 0) STOP 1

  CALL fah90_client_create(cli2, err, use_raw=.FALSE.)
  CALL fah90_client_destroy(cli2, err)

  CALL fhmks7(port, '/echo', url1, ulen)
  CALL fah90_request_create(req1, err)
  CALL fah90_request_set_target(req1, FAH90_HTTP_POST, url1, err)
  CALL fah90_request_add_header(req1, 'X-Custom', 'f90', err)
  CALL fah90_request_set_body(req1, 'F90-BODY  ', err)
  CALL fah90_send(cli, req1, res1, err)
  IF (err /= FAH90_OK) STOP 1
  CALL fah90_response_status(res1, st, blen, err)
  CALL fah90_response_body(res1, body_out, blen, err)
  IF (st /= 200 .OR. body_out(1:8) /= 'F90-BODY') STOP 1
  CALL fah90_response_destroy(res1, err)

  CALL fah90_request_set_body(req1, 'EXACT', err, exact_len=5)
  CALL fhmks7(port, '/hello', url2, ulen)
  CALL fah90_request_create(req2, err)
  CALL fah90_request_set_target(req2, FAH90_HTTP_GET, url2, err)

  CALL fah90_send_multi_two(cli, req1, req2, res1, res2, err)
  IF (err /= FAH90_OK) STOP 1
  CALL fah90_response_status(res2, st, blen, err)
  IF (st /= 200 .OR. blen /= 13) STOP 1

  CALL fah90_response_destroy(res1, err)
  CALL fah90_response_destroy(res2, err)

  ! Exercise multi-request transfer error propagation on slot 1 & slot 2
  CALL fah90_request_set_target(req1, FAH90_HTTP_GET, &
      'badscheme://unreachable', err)
  CALL fah90_send_multi_two(cli, req1, req2, res1, res2, err)
  IF (err == FAH90_OK) STOP 1
  CALL fah90_send_multi_two(cli, req2, req1, res1, res2, err)
  IF (err == FAH90_OK) STOP 1
  CALL fah90_response_destroy(res1, err)

  CALL fah90_request_destroy(req1, err)
  CALL fah90_request_destroy(req2, err)
  CALL fah90_client_destroy(cli, err)
  dm = ihmksp(port)
  CALL fah90_global_cleanup(err)
END PROGRAM test_f90_mod
