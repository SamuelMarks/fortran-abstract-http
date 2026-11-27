!> @file fah_f95_mod.f90
!> @module fah_f95_mod
!> @brief Strict ISO/IEC 1539-1:1997 (Fortran 95) HTTP SDK module adding
!> default component initialization, PURE predicates, and ELEMENTAL
!> array-capable status/error classifiers (-std=f95 compliant).
MODULE fah_f95_mod
  USE fah_f90_mod
  IMPLICIT NONE
  PRIVATE

  !> @brief Default-initialized Fortran 95 structured error record.
  TYPE, PUBLIC :: fah95_error_t
    !> @brief Numeric fah_error_code_t (defaults to FAH90_OK = 0).
    INTEGER :: code = FAH90_OK
    !> @brief Semantic category (defaults to 0).
    INTEGER :: category = 0
    !> @brief Retryability flag (defaults to .FALSE.).
    LOGICAL :: retryable = .FALSE.
    !> @brief Formatted diagnostic line.
    CHARACTER(LEN=160) :: message = ' '
  END TYPE fah95_error_t

  !> @brief Default-initialized Fortran 95 HTTP client handle wrapper.
  TYPE, PUBLIC :: fah95_client_t
    PRIVATE
    !> @brief Underlying Fortran 90 client handle.
    TYPE(fah90_client_t) :: inner
    !> @brief .TRUE. when client has been initialized and not destroyed.
    LOGICAL :: active = .FALSE.
  END TYPE fah95_client_t

  PUBLIC :: fah95_err_is_ok, fah95_err_code, fah95_status_is_2xx
  PUBLIC :: fah95_status_is_redirect, fah95_client_is_active
  PUBLIC :: fah95_client_open, fah95_client_close, fah95_get_text

CONTAINS

  !> @brief ELEMENTAL check whether a fah95_error_t indicates success.
  !> @param[in] err Scalar or array of fah95_error_t values.
  !> @return .TRUE. for each element where code == 0.
  ELEMENTAL LOGICAL FUNCTION fah95_err_is_ok(err)
    TYPE(fah95_error_t), INTENT(IN) :: err
    fah95_err_is_ok = (err%code == FAH90_OK)
  END FUNCTION fah95_err_is_ok

  !> @brief ELEMENTAL extractor returning integer code of fah95_error_t.
  !> @param[in] err Scalar or array of fah95_error_t values.
  !> @return Integer error code.
  ELEMENTAL INTEGER FUNCTION fah95_err_code(err)
    TYPE(fah95_error_t), INTENT(IN) :: err
    fah95_err_code = err%code
  END FUNCTION fah95_err_code

  !> @brief ELEMENTAL classifier returning .TRUE. for 200..299 statuses.
  !> @param[in] status_code Scalar or array of HTTP status codes.
  !> @return .TRUE. if 2xx success status.
  ELEMENTAL LOGICAL FUNCTION fah95_status_is_2xx(status_code)
    INTEGER, INTENT(IN) :: status_code
    fah95_status_is_2xx = (status_code >= 200 .AND. status_code < 300)
  END FUNCTION fah95_status_is_2xx

  !> @brief ELEMENTAL classifier returning .TRUE. for 300..399 statuses.
  !> @param[in] status_code Scalar or array of HTTP status codes.
  !> @return .TRUE. if 3xx redirect status.
  ELEMENTAL LOGICAL FUNCTION fah95_status_is_redirect(status_code)
    INTEGER, INTENT(IN) :: status_code
    fah95_status_is_redirect = &
        (status_code >= 300 .AND. status_code < 400)
  END FUNCTION fah95_status_is_redirect

  !> @brief PURE observer checking whether a fah95_client_t is active.
  !> @param[in] cli Client handle wrapper.
  !> @return .TRUE. when active.
  PURE LOGICAL FUNCTION fah95_client_is_active(cli)
    TYPE(fah95_client_t), INTENT(IN) :: cli
    fah95_client_is_active = cli%active
  END FUNCTION fah95_client_is_active

  !> @brief Convert a fah90_error_t into a default-initialized fah95_error_t.
  !> @param[in] src Source Fortran 90 error.
  !> @param[out] dst Destination Fortran 95 error.
  SUBROUTINE fah95_from_f90_err(src, dst)
    TYPE(fah90_error_t), INTENT(IN) :: src
    TYPE(fah95_error_t), INTENT(OUT) :: dst
    dst%code = src%code
    dst%category = src%category
    dst%retryable = src%retryable
    dst%message = src%message
  END SUBROUTINE fah95_from_f90_err

  !> @brief Open a Fortran 95 HTTP client handle.
  !> @param[out] cli Client handle wrapper.
  !> @param[out] err Default-initialized structured error.
  !> @param[in] use_raw Optional raw-socket transport flag.
  SUBROUTINE fah95_client_open(cli, err, use_raw)
    TYPE(fah95_client_t), INTENT(OUT) :: cli
    TYPE(fah95_error_t), INTENT(OUT) :: err
    LOGICAL, OPTIONAL, INTENT(IN) :: use_raw
    TYPE(fah90_error_t) :: e90
    CALL fah90_client_create(cli%inner, e90, use_raw=use_raw)
    CALL fah95_from_f90_err(e90, err)
    cli%active = (err%code == FAH90_OK)
  END SUBROUTINE fah95_client_open

  !> @brief Close a Fortran 95 HTTP client handle.
  !> @param[inout] cli Client handle wrapper.
  !> @param[out] err Structured error.
  SUBROUTINE fah95_client_close(cli, err)
    TYPE(fah95_client_t), INTENT(INOUT) :: cli
    TYPE(fah95_error_t), INTENT(OUT) :: err
    TYPE(fah90_error_t) :: e90
    IF (.NOT. cli%active) THEN
      err%code = FAH90_ERR_NULL_HDL
      err%category = 7
      err%message = '[FAH_ERR_NULL_HANDLE] Client is not active'
      RETURN
    END IF
    CALL fah90_client_destroy(cli%inner, e90)
    CALL fah95_from_f90_err(e90, err)
    IF (err%code == FAH90_OK) cli%active = .FALSE.
  END SUBROUTINE fah95_client_close

  !> @brief Execute an HTTP GET and return status + body in Fortran 95.
  !> @param[in] cli Active Fortran 95 client.
  !> @param[in] url Destination URL.
  !> @param[out] out_body Space-padded response buffer.
  !> @param[out] status_code HTTP status code.
  !> @param[out] body_len Actual response byte length.
  !> @param[out] err Structured error.
  SUBROUTINE fah95_get_text(cli, url, out_body, status_code, body_len, &
      err)
    TYPE(fah95_client_t), INTENT(IN) :: cli
    CHARACTER(LEN=*), INTENT(IN) :: url
    CHARACTER(LEN=*), INTENT(OUT) :: out_body
    INTEGER, INTENT(OUT) :: status_code, body_len
    TYPE(fah95_error_t), INTENT(OUT) :: err
    TYPE(fah90_request_t) :: req
    TYPE(fah90_response_t) :: res
    TYPE(fah90_error_t) :: e90, e_tmp

    status_code = 0
    body_len = 0
    out_body = ' '
    IF (.NOT. cli%active) THEN
      err%code = FAH90_ERR_NULL_HDL
      err%category = 7
      err%message = '[FAH_ERR_NULL_HANDLE] Client is not active'
      RETURN
    END IF

    CALL fah90_request_create(req, e90)
    IF (e90 == FAH90_OK) THEN
      CALL fah90_request_set_target(req, FAH90_HTTP_GET, url, e90)
      IF (e90 == FAH90_OK) THEN
        CALL fah90_send(cli%inner, req, res, e90)
        IF (e90 == FAH90_OK) THEN
          CALL fah90_response_status(res, status_code, body_len, e90)
          IF (e90 == FAH90_OK) THEN
            CALL fah90_response_body(res, out_body, body_len, e90)
          END IF
          CALL fah90_response_destroy(res, e_tmp)
        END IF
      END IF
      CALL fah90_request_destroy(req, e_tmp)
    END IF
    CALL fah95_from_f90_err(e90, err)
  END SUBROUTINE fah95_get_text

END MODULE fah_f95_mod
