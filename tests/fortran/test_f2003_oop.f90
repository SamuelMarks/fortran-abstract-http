!> @file test_f2003_oop.f90
!> @brief Integration test suite for ISO/IEC 1539-1:2004 (Fortran 2003)
!> OOP classes, deferred-length strings, and RAII FINAL destructors.
PROGRAM test_f2003_oop
  USE, INTRINSIC :: ISO_C_BINDING, ONLY: C_INT
  USE fah_f2003_mod
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

  INTEGER(C_INT) :: port, dm
  TYPE(fah_error) :: err, default_err
  CHARACTER(LEN=:), ALLOCATABLE :: disp

  err = fah03_global_init()
  IF (.NOT. err%is_ok()) STOP 1
  disp = default_err%display()
  IF (disp /= 'OK') STOP 1

  IF (fah_mock_server_start(port) /= 0_C_INT) STOP 1
  CALL run_raii_scope(port)
  dm = fah_mock_server_stop(port)

  err = fah03_global_cleanup()
  IF (.NOT. err%is_ok()) STOP 1

CONTAINS

  SUBROUTINE run_raii_scope(srv_port)
    INTEGER(C_INT), INTENT(IN) :: srv_port
    TYPE(fah_client) :: cli, cli_raii
    TYPE(fah_request) :: req, req_raii
    TYPE(fah_response) :: res, res_raii, res_empty
    TYPE(fah_error) :: e
    CHARACTER(LEN=128) :: u_echo, u_tok, u_base
    CHARACTER(LEN=:), ALLOCATABLE :: bstr, hstr, edisp
    INTEGER :: ulen
    INTEGER(C_INT) :: st

    ! Open & re-open client + RAII objects left open for FINAL on return
    e = cli%open(use_raw=.FALSE.)
    e = cli%open()
    IF (.NOT. e%is_ok() .OR. cli%handle_id() <= 0_C_INT) STOP 1
    e = cli_raii%open()
    e = req_raii%init()

    CALL fhmks7(INT(srv_port), '', u_base, ulen)
    e = cli%set_base_url(u_base(1:ulen))
    IF (.NOT. e%is_ok()) STOP 1

    ! Request init + re-init + Bearer + POST
    e = req%init()
    e = req%init()
    CALL fhmks7(INT(srv_port), '/echo', u_echo, ulen)
    e = req%set_target(1_C_INT, u_echo)
    e = req%add_header('X-Custom', 'oop2003')
    e = req%set_bearer('tok2003')
    e = req%set_body_text('Hello F2003 OOP!')
    IF (.NOT. e%is_ok() .OR. req%handle_id() <= 0_C_INT) STOP 1

    e = cli%send(req, res)
    IF (.NOT. e%is_ok() .OR. res%handle_id() <= 0_C_INT) STOP 1
    e = res%status(st)
    e = res%body_string(bstr)
    e = res%header('X-Echo-Custom', hstr)
    IF (st /= 200_C_INT .OR. bstr /= 'Hello F2003 OOP!' .OR. &
        hstr /= 'oop2003') STOP 1

    ! Overwrite res via second send + OAuth2 client_creds (with & without scope)
    CALL fhmks7(INT(srv_port), '/oauth/token', u_tok, ulen)
    e = req%set_oauth2_client_creds(u_tok, 'cid', 'sec')
    e = req%set_oauth2_client_creds(u_tok, 'cid', 'sec', scope='api')
    e = cli%send(req, res)
    IF (.NOT. e%is_ok()) STOP 1

    ! Adopt handle into res_raii so FINAL frees res_raii on scope exit
    CALL res_raii%adopt_handle(res%handle_id())
    CALL res_raii%adopt_handle(0_C_INT)
    e = cli%send(req, res_raii)

    ! Error branches on uninitialized response
    e = res_empty%body_string(bstr)
    IF (e%is_ok()) STOP 1
    edisp = e%display()
    IF (LEN(edisp) <= 2) STOP 1
    e = res_empty%header('Missing', hstr)
    IF (e%is_ok()) STOP 1

    ! Explicit free / close
    e = req%free()
    e = cli%close()
    IF (.NOT. e%is_ok()) STOP 1

    ! Polymorphic CLASS(...) ALLOCATE(SOURCE=...) & DEALLOCATE vtable dispatch
    CALL test_polymorphic_clones(e, cli, req, res_empty)
  END SUBROUTINE run_raii_scope

  SUBROUTINE test_polymorphic_clones(e_src, c_src, q_src, s_src)
    TYPE(fah_error), INTENT(IN) :: e_src
    TYPE(fah_client), INTENT(IN) :: c_src
    TYPE(fah_request), INTENT(IN) :: q_src
    TYPE(fah_response), INTENT(IN) :: s_src
    CLASS(fah_error), ALLOCATABLE :: pe
    CLASS(fah_client), ALLOCATABLE :: pc
    CLASS(fah_request), ALLOCATABLE :: pq
    CLASS(fah_response), ALLOCATABLE :: ps

    ALLOCATE(pe, SOURCE=e_src)
    IF (.NOT. pe%is_ok()) STOP 1
    DEALLOCATE(pe)

    ALLOCATE(pc, SOURCE=c_src)
    DEALLOCATE(pc)

    ALLOCATE(pq, SOURCE=q_src)
    DEALLOCATE(pq)

    ALLOCATE(ps, SOURCE=s_src)
    DEALLOCATE(ps)
  END SUBROUTINE test_polymorphic_clones

END PROGRAM test_f2003_oop
