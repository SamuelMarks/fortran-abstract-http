!> @file fah_f90_mod.f90
!> @module fah_f90_mod
!> @brief Strict ISO/IEC 1539:1991 (Fortran 90) HTTP SDK module with
!> encapsulated private-handle derived types, OPTIONAL arguments, and
!> GENERIC equality operators on fah90_error_t (zero ISO_C_BINDING and
!> zero Fortran 95 extensions; all identifiers <= 31 characters).
MODULE fah_f90_mod
  IMPLICIT NONE
  PRIVATE

  !> @brief Success status code (FAH_OK).
  INTEGER, PUBLIC, PARAMETER :: FAH90_OK = 0
  !> @brief Invalid argument error code.
  INTEGER, PUBLIC, PARAMETER :: FAH90_ERR_INVAL = -1
  !> @brief Out of memory error code.
  INTEGER, PUBLIC, PARAMETER :: FAH90_ERR_NOMEM = -2
  !> @brief Network I/O error code.
  INTEGER, PUBLIC, PARAMETER :: FAH90_ERR_IO = -3
  !> @brief Null handle error code.
  INTEGER, PUBLIC, PARAMETER :: FAH90_ERR_NULL_HDL = -2001
  !> @brief Buffer truncated error code.
  INTEGER, PUBLIC, PARAMETER :: FAH90_ERR_BUF_SMALL = -2005

  !> @brief HTTP GET method constant.
  INTEGER, PUBLIC, PARAMETER :: FAH90_HTTP_GET = 0
  !> @brief HTTP POST method constant.
  INTEGER, PUBLIC, PARAMETER :: FAH90_HTTP_POST = 1
  !> @brief HTTP PUT method constant.
  INTEGER, PUBLIC, PARAMETER :: FAH90_HTTP_PUT = 2
  !> @brief HTTP DELETE method constant.
  INTEGER, PUBLIC, PARAMETER :: FAH90_HTTP_DELETE = 3

  !> @brief Unified structured error value for Fortran 90 callers.
  TYPE, PUBLIC :: fah90_error_t
    !> @brief Numeric fah_error_code_t value.
    INTEGER :: code
    !> @brief Semantic category (0..8).
    INTEGER :: category
    !> @brief .TRUE. if transient and retryable.
    LOGICAL :: retryable
    !> @brief Formatted human-readable diagnostic line.
    CHARACTER(LEN=160) :: message
  END TYPE fah90_error_t

  !> @brief Opaque Fortran 90 configuration handle wrapper.
  TYPE, PUBLIC :: fah90_config_t
    PRIVATE
    !> @brief Positive 31-bit integer registry handle.
    INTEGER :: id
  END TYPE fah90_config_t

  !> @brief Opaque Fortran 90 HTTP client handle wrapper.
  TYPE, PUBLIC :: fah90_client_t
    PRIVATE
    !> @brief Positive 31-bit integer registry handle.
    INTEGER :: id
  END TYPE fah90_client_t

  !> @brief Opaque Fortran 90 HTTP request handle wrapper.
  TYPE, PUBLIC :: fah90_request_t
    PRIVATE
    !> @brief Positive 31-bit integer registry handle.
    INTEGER :: id
  END TYPE fah90_request_t

  !> @brief Opaque Fortran 90 HTTP response handle wrapper.
  TYPE, PUBLIC :: fah90_response_t
    PRIVATE
    !> @brief Positive 31-bit integer registry handle.
    INTEGER :: id
  END TYPE fah90_response_t

  !> @brief Opaque Fortran 90 multi-request batch handle wrapper.
  TYPE, PUBLIC :: fah90_multi_t
    PRIVATE
    !> @brief Positive 31-bit integer registry handle.
    INTEGER :: id
  END TYPE fah90_multi_t

  !> @brief Opaque Fortran 90 deferred future handle wrapper.
  TYPE, PUBLIC :: fah90_future_t
    PRIVATE
    !> @brief Positive 31-bit integer registry handle.
    INTEGER :: id
  END TYPE fah90_future_t

  !> @brief Generic equality operator comparing fah90_error_t with code.
  INTERFACE OPERATOR(==)
    MODULE PROCEDURE fah90_err_eq_err
    MODULE PROCEDURE fah90_err_eq_code
  END INTERFACE

  !> @brief Generic inequality operator comparing fah90_error_t with code.
  INTERFACE OPERATOR(/=)
    MODULE PROCEDURE fah90_err_ne_err
    MODULE PROCEDURE fah90_err_ne_code
  END INTERFACE

  !> @brief Explicit Fortran 90 external interfaces to C89 entry shims.
  INTERFACE
    !> @brief Initialize global transport.
    !> @param[in] dummy Unused integer.
    !> @return Error code.
    INTEGER FUNCTION ihgini(dummy)
      INTEGER, INTENT(IN) :: dummy
    END FUNCTION ihgini

    !> @brief Clean up global transport.
    !> @param[in] dummy Unused integer.
    !> @return Error code.
    INTEGER FUNCTION ihgcln(dummy)
      INTEGER, INTENT(IN) :: dummy
    END FUNCTION ihgcln

    !> @brief Create config handle.
    !> @param[out] out_cfg Config handle.
    !> @return Error code.
    INTEGER FUNCTION ihcfcr(out_cfg)
      INTEGER, INTENT(OUT) :: out_cfg
    END FUNCTION ihcfcr

    !> @brief Destroy config handle.
    !> @param[in] cfg_h Config handle.
    !> @return Error code.
    INTEGER FUNCTION ihcffr(cfg_h)
      INTEGER, INTENT(IN) :: cfg_h
    END FUNCTION ihcffr

    !> @brief Set basic config options.
    !> @param[in] cfg_h Config handle.
    !> @param[in] timeout_ms Timeout in ms.
    !> @param[in] verify_peer Verify TLS peer flag.
    !> @param[in] follow_redirs Follow 3xx redirects flag.
    !> @param[in] retry_cnt Retry count.
    !> @return Error code.
    INTEGER FUNCTION ihcfst(cfg_h, timeout_ms, verify_peer, &
        follow_redirs, retry_cnt)
      INTEGER, INTENT(IN) :: cfg_h, timeout_ms, verify_peer
      INTEGER, INTENT(IN) :: follow_redirs, retry_cnt
    END FUNCTION ihcfst

    !> @brief Create client handle.
    !> @param[in] cfg_h Optional config handle.
    !> @param[in] use_raw Raw socket flag.
    !> @param[out] out_cli Client handle.
    !> @return Error code.
    INTEGER FUNCTION ihclcr(cfg_h, use_raw, out_cli)
      INTEGER, INTENT(IN) :: cfg_h, use_raw
      INTEGER, INTENT(OUT) :: out_cli
    END FUNCTION ihclcr

    !> @brief Destroy client handle.
    !> @param[in] cli_h Client handle.
    !> @return Error code.
    INTEGER FUNCTION ihclfr(cli_h)
      INTEGER, INTENT(IN) :: cli_h
    END FUNCTION ihclfr

    !> @brief Create request handle.
    !> @param[out] out_req Request handle.
    !> @return Error code.
    INTEGER FUNCTION ihrqcr(out_req)
      INTEGER, INTENT(OUT) :: out_req
    END FUNCTION ihrqcr

    !> @brief Destroy request handle.
    !> @param[in] req_h Request handle.
    !> @return Error code.
    INTEGER FUNCTION ihrqfr(req_h)
      INTEGER, INTENT(IN) :: req_h
    END FUNCTION ihrqfr

    !> @brief Send synchronous HTTP request.
    !> @param[in] cli_h Client handle.
    !> @param[in] req_h Request handle.
    !> @param[out] out_res Response handle.
    !> @return Error code.
    INTEGER FUNCTION ihsend(cli_h, req_h, out_res)
      INTEGER, INTENT(IN) :: cli_h, req_h
      INTEGER, INTENT(OUT) :: out_res
    END FUNCTION ihsend

    !> @brief Read response status and body length.
    !> @param[in] res_h Response handle.
    !> @param[out] out_status HTTP status code.
    !> @param[out] out_len Body byte length.
    !> @return Error code.
    INTEGER FUNCTION ihrsin(res_h, out_status, out_len)
      INTEGER, INTENT(IN) :: res_h
      INTEGER, INTENT(OUT) :: out_status, out_len
    END FUNCTION ihrsin

    !> @brief Destroy response handle.
    !> @param[in] res_h Response handle.
    !> @return Error code.
    INTEGER FUNCTION ihrsfr(res_h)
      INTEGER, INTENT(IN) :: res_h
    END FUNCTION ihrsfr

    !> @brief Request string mutator entry point.
    !> @param[in] req_h Request handle.
    !> @param[in] op Operation code.
    !> @param[in] iarg Integer selector.
    !> @param[in] s1 String 1.
    !> @param[in] s2 String 2.
    !> @param[in] s3 String 3.
    !> @param[in] s4 String 4.
    !> @param[out] out_err Error code.
    SUBROUTINE fh7req(req_h, op, iarg, s1, s2, s3, s4, out_err)
      INTEGER, INTENT(IN) :: req_h, op, iarg
      CHARACTER(LEN=*), INTENT(IN) :: s1, s2, s3, s4
      INTEGER, INTENT(OUT) :: out_err
    END SUBROUTINE fh7req

    !> @brief Response string reader entry point.
    !> @param[in] res_h Response handle.
    !> @param[in] op Operation code.
    !> @param[in] idx Header index.
    !> @param[inout] s1 Key/path string.
    !> @param[out] s2 Output buffer.
    !> @param[out] out_n1 Output length 1.
    !> @param[out] out_n2 Output length 2.
    !> @param[out] out_err Error code.
    SUBROUTINE fh7res(res_h, op, idx, s1, s2, out_n1, out_n2, out_err)
      INTEGER, INTENT(IN) :: res_h, op, idx
      CHARACTER(LEN=*), INTENT(INOUT) :: s1
      CHARACTER(LEN=*), INTENT(OUT) :: s2
      INTEGER, INTENT(OUT) :: out_n1, out_n2, out_err
    END SUBROUTINE fh7res

    !> @brief Multi-request and future operations entry point.
    !> @param[in] cli_h Client handle.
    !> @param[inout] io_h Multi or future handle.
    !> @param[in] op Operation code.
    !> @param[in] arg Auxiliary integer argument.
    !> @param[out] fut_arr Future handle array.
    !> @param[in] cap Array capacity.
    !> @param[out] out_n1 Result 1.
    !> @param[out] out_n2 Result 2.
    !> @param[out] out_err Error code.
    SUBROUTINE fh7mul(cli_h, io_h, op, arg, fut_arr, cap, out_n1, &
        out_n2, out_err)
      INTEGER, INTENT(IN) :: cli_h, op, arg, cap
      INTEGER, INTENT(INOUT) :: io_h
      INTEGER, DIMENSION(*), INTENT(OUT) :: fut_arr
      INTEGER, INTENT(OUT) :: out_n1, out_n2, out_err
    END SUBROUTINE fh7mul

    !> @brief Error formatting entry point.
    !> @param[in] code Error code.
    !> @param[in] op_name Operation name.
    !> @param[out] out_msg Formatted diagnostic text.
    !> @param[out] out_cat Error category.
    !> @param[out] out_retry Retryable flag.
    !> @param[out] out_len Formatted length.
    !> @param[out] out_err Formatter status.
    SUBROUTINE fh7err(code, op_name, out_msg, out_cat, out_retry, &
        out_len, out_err)
      INTEGER, INTENT(IN) :: code
      CHARACTER(LEN=*), INTENT(IN) :: op_name
      CHARACTER(LEN=*), INTENT(OUT) :: out_msg
      INTEGER, INTENT(OUT) :: out_cat, out_retry, out_len, out_err
    END SUBROUTINE fh7err
  END INTERFACE

  PUBLIC :: OPERATOR(==), OPERATOR(/=)
  PUBLIC :: fah90_make_error, fah90_global_init, fah90_global_cleanup
  PUBLIC :: fah90_client_create, fah90_client_destroy, fah90_client_raw_id
  PUBLIC :: fah90_request_create, fah90_request_set_target
  PUBLIC :: fah90_request_add_header, fah90_request_set_body
  PUBLIC :: fah90_request_destroy
  PUBLIC :: fah90_send, fah90_response_status, fah90_response_body
  PUBLIC :: fah90_response_destroy
  PUBLIC :: fah90_send_multi_two

CONTAINS

  !> @brief Construct a structured fah90_error_t from raw code and op.
  !> @param[in] code Numeric fah_error_code_t.
  !> @param[in] op_name Originating procedure name.
  !> @param[out] err Populated fah90_error_t record.
  SUBROUTINE fah90_make_error(code, op_name, err)
    INTEGER, INTENT(IN) :: code
    CHARACTER(LEN=*), INTENT(IN) :: op_name
    TYPE(fah90_error_t), INTENT(OUT) :: err
    INTEGER :: irty, ilen, f_rc
    err%code = code
    IF (code == 0) THEN
      err%category = 0
      err%retryable = .FALSE.
      err%message = '[FAH_OK] Operation succeeded'
      RETURN
    END IF
    CALL fh7err(code, op_name, err%message, err%category, irty, ilen, &
        f_rc)
    err%retryable = (irty /= 0)
  END SUBROUTINE fah90_make_error

  !> @brief Compare two fah90_error_t values by error code.
  !> @param[in] lhs Left operand.
  !> @param[in] rhs Right operand.
  !> @return .TRUE. when codes match.
  LOGICAL FUNCTION fah90_err_eq_err(lhs, rhs)
    TYPE(fah90_error_t), INTENT(IN) :: lhs, rhs
    fah90_err_eq_err = (lhs%code == rhs%code)
  END FUNCTION fah90_err_eq_err

  !> @brief Compare fah90_error_t with an integer code.
  !> @param[in] lhs Error value.
  !> @param[in] rhs Integer code.
  !> @return .TRUE. when lhs%code == rhs.
  LOGICAL FUNCTION fah90_err_eq_code(lhs, rhs)
    TYPE(fah90_error_t), INTENT(IN) :: lhs
    INTEGER, INTENT(IN) :: rhs
    fah90_err_eq_code = (lhs%code == rhs)
  END FUNCTION fah90_err_eq_code

  !> @brief Inequality check for two fah90_error_t values.
  !> @param[in] lhs Left operand.
  !> @param[in] rhs Right operand.
  !> @return .TRUE. when codes differ.
  LOGICAL FUNCTION fah90_err_ne_err(lhs, rhs)
    TYPE(fah90_error_t), INTENT(IN) :: lhs, rhs
    fah90_err_ne_err = (lhs%code /= rhs%code)
  END FUNCTION fah90_err_ne_err

  !> @brief Inequality check between fah90_error_t and integer code.
  !> @param[in] lhs Error value.
  !> @param[in] rhs Integer code.
  !> @return .TRUE. when lhs%code /= rhs.
  LOGICAL FUNCTION fah90_err_ne_code(lhs, rhs)
    TYPE(fah90_error_t), INTENT(IN) :: lhs
    INTEGER, INTENT(IN) :: rhs
    fah90_err_ne_code = (lhs%code /= rhs)
  END FUNCTION fah90_err_ne_code

  !> @brief Initialize global HTTP transport (Fortran 90).
  !> @param[out] err Structured error status.
  SUBROUTINE fah90_global_init(err)
    TYPE(fah90_error_t), INTENT(OUT) :: err
    INTEGER :: rc
    rc = ihgini(0)
    CALL fah90_make_error(rc, 'fah90_global_init', err)
  END SUBROUTINE fah90_global_init

  !> @brief Clean up global HTTP transport (Fortran 90).
  !> @param[out] err Structured error status.
  SUBROUTINE fah90_global_cleanup(err)
    TYPE(fah90_error_t), INTENT(OUT) :: err
    INTEGER :: rc
    rc = ihgcln(0)
    CALL fah90_make_error(rc, 'fah90_global_cleanup', err)
  END SUBROUTINE fah90_global_cleanup

  !> @brief Create a Fortran 90 HTTP client with OPTIONAL settings.
  !> @param[out] cli Initialized client handle wrapper.
  !> @param[out] err Structured error status.
  !> @param[in] timeout_ms Optional timeout in milliseconds.
  !> @param[in] verify_peer Optional TLS peer verification flag.
  !> @param[in] follow_redirects Optional 3xx redirect following flag.
  !> @param[in] use_raw Optional raw-socket transport flag.
  SUBROUTINE fah90_client_create(cli, err, timeout_ms, verify_peer, &
      follow_redirects, use_raw)
    TYPE(fah90_client_t), INTENT(OUT) :: cli
    TYPE(fah90_error_t), INTENT(OUT) :: err
    INTEGER, OPTIONAL, INTENT(IN) :: timeout_ms
    LOGICAL, OPTIONAL, INTENT(IN) :: verify_peer, follow_redirects
    LOGICAL, OPTIONAL, INTENT(IN) :: use_raw
    INTEGER :: cfg_id, tmo, ivp, ifol, iraw, rc, dm

    cli%id = 0
    tmo = 5000
    IF (PRESENT(timeout_ms)) tmo = timeout_ms
    ivp = 0
    IF (PRESENT(verify_peer)) THEN
      IF (verify_peer) ivp = 1
    END IF
    ifol = 1
    IF (PRESENT(follow_redirects)) THEN
      IF (.NOT. follow_redirects) ifol = 0
    END IF
    iraw = 1
    IF (PRESENT(use_raw)) THEN
      IF (.NOT. use_raw) iraw = 0
    END IF

    rc = ihcfcr(cfg_id)
    IF (rc == 0) THEN
      rc = ihcfst(cfg_id, tmo, ivp, ifol, 1)
      IF (rc == 0) THEN
        rc = ihclcr(cfg_id, iraw, cli%id)
      END IF
      dm = ihcffr(cfg_id)
    END IF
    CALL fah90_make_error(rc, 'fah90_client_create', err)
  END SUBROUTINE fah90_client_create

  !> @brief Inspect raw integer handle ID of a client (0 if closed).
  !> @param[in] cli Client handle wrapper.
  !> @return Integer handle ID.
  INTEGER FUNCTION fah90_client_raw_id(cli)
    TYPE(fah90_client_t), INTENT(IN) :: cli
    fah90_client_raw_id = cli%id
  END FUNCTION fah90_client_raw_id

  !> @brief Destroy a Fortran 90 HTTP client handle.
  !> @param[inout] cli Client handle wrapper (reset to 0).
  !> @param[out] err Structured error status.
  SUBROUTINE fah90_client_destroy(cli, err)
    TYPE(fah90_client_t), INTENT(INOUT) :: cli
    TYPE(fah90_error_t), INTENT(OUT) :: err
    INTEGER :: rc
    rc = ihclfr(cli%id)
    IF (rc == 0) cli%id = 0
    CALL fah90_make_error(rc, 'fah90_client_destroy', err)
  END SUBROUTINE fah90_client_destroy

  !> @brief Create a Fortran 90 HTTP request handle.
  !> @param[out] req Request handle wrapper.
  !> @param[out] err Structured error status.
  SUBROUTINE fah90_request_create(req, err)
    TYPE(fah90_request_t), INTENT(OUT) :: req
    TYPE(fah90_error_t), INTENT(OUT) :: err
    INTEGER :: rc
    req%id = 0
    rc = ihrqcr(req%id)
    CALL fah90_make_error(rc, 'fah90_request_create', err)
  END SUBROUTINE fah90_request_create

  !> @brief Configure method and URL on a Fortran 90 request.
  !> @param[inout] req Request handle wrapper.
  !> @param[in] method HTTP verb constant.
  !> @param[in] url Destination URL.
  !> @param[out] err Structured error status.
  SUBROUTINE fah90_request_set_target(req, method, url, err)
    TYPE(fah90_request_t), INTENT(INOUT) :: req
    INTEGER, INTENT(IN) :: method
    CHARACTER(LEN=*), INTENT(IN) :: url
    TYPE(fah90_error_t), INTENT(OUT) :: err
    INTEGER :: rc
    CALL fh7req(req%id, 1, method, url, ' ', ' ', ' ', rc)
    CALL fah90_make_error(rc, 'fah90_request_set_target', err)
  END SUBROUTINE fah90_request_set_target

  !> @brief Add an HTTP header onto a Fortran 90 request.
  !> @param[inout] req Request handle wrapper.
  !> @param[in] hkey Header name.
  !> @param[in] hval Header value.
  !> @param[out] err Structured error status.
  SUBROUTINE fah90_request_add_header(req, hkey, hval, err)
    TYPE(fah90_request_t), INTENT(INOUT) :: req
    CHARACTER(LEN=*), INTENT(IN) :: hkey, hval
    TYPE(fah90_error_t), INTENT(OUT) :: err
    INTEGER :: rc
    CALL fh7req(req%id, 2, 0, hkey, hval, ' ', ' ', rc)
    CALL fah90_make_error(rc, 'fah90_request_add_header', err)
  END SUBROUTINE fah90_request_add_header

  !> @brief Set an explicit-length binary/character body on req.
  !> @param[inout] req Request handle wrapper.
  !> @param[in] body Raw payload string/buffer.
  !> @param[in] exact_len Optional exact byte length (defaults to LEN_TRIM).
  !> @param[out] err Structured error status.
  SUBROUTINE fah90_request_set_body(req, body, err, exact_len)
    TYPE(fah90_request_t), INTENT(INOUT) :: req
    CHARACTER(LEN=*), INTENT(IN) :: body
    TYPE(fah90_error_t), INTENT(OUT) :: err
    INTEGER, OPTIONAL, INTENT(IN) :: exact_len
    INTEGER :: nbytes, rc
    IF (PRESENT(exact_len)) THEN
      nbytes = exact_len
    ELSE
      nbytes = LEN_TRIM(body)
    END IF
    CALL fh7req(req%id, 4, nbytes, body, ' ', ' ', ' ', rc)
    CALL fah90_make_error(rc, 'fah90_request_set_body', err)
  END SUBROUTINE fah90_request_set_body

  !> @brief Destroy a Fortran 90 HTTP request handle.
  !> @param[inout] req Request handle wrapper.
  !> @param[out] err Structured error status.
  SUBROUTINE fah90_request_destroy(req, err)
    TYPE(fah90_request_t), INTENT(INOUT) :: req
    TYPE(fah90_error_t), INTENT(OUT) :: err
    INTEGER :: rc
    rc = ihrqfr(req%id)
    IF (rc == 0) req%id = 0
    CALL fah90_make_error(rc, 'fah90_request_destroy', err)
  END SUBROUTINE fah90_request_destroy

  !> @brief Send an HTTP request synchronously in Fortran 90.
  !> @param[in] cli Client handle wrapper.
  !> @param[in] req Request handle wrapper.
  !> @param[out] res Response handle wrapper.
  !> @param[out] err Structured error status.
  SUBROUTINE fah90_send(cli, req, res, err)
    TYPE(fah90_client_t), INTENT(IN) :: cli
    TYPE(fah90_request_t), INTENT(IN) :: req
    TYPE(fah90_response_t), INTENT(OUT) :: res
    TYPE(fah90_error_t), INTENT(OUT) :: err
    INTEGER :: rc
    res%id = 0
    rc = ihsend(cli%id, req%id, res%id)
    CALL fah90_make_error(rc, 'fah90_send', err)
  END SUBROUTINE fah90_send

  !> @brief Query HTTP status code and payload byte length on res.
  !> @param[in] res Response handle wrapper.
  !> @param[out] status_code HTTP status code.
  !> @param[out] body_len Body byte length.
  !> @param[out] err Structured error status.
  SUBROUTINE fah90_response_status(res, status_code, body_len, err)
    TYPE(fah90_response_t), INTENT(IN) :: res
    INTEGER, INTENT(OUT) :: status_code, body_len
    TYPE(fah90_error_t), INTENT(OUT) :: err
    INTEGER :: rc
    status_code = 0
    body_len = 0
    rc = ihrsin(res%id, status_code, body_len)
    CALL fah90_make_error(rc, 'fah90_response_status', err)
  END SUBROUTINE fah90_response_status

  !> @brief Copy response body into caller CHARACTER buffer.
  !> @param[in] res Response handle wrapper.
  !> @param[out] out_buf Destination buffer.
  !> @param[out] actual_len Actual payload byte count.
  !> @param[out] err Structured error status.
  SUBROUTINE fah90_response_body(res, out_buf, actual_len, err)
    TYPE(fah90_response_t), INTENT(IN) :: res
    CHARACTER(LEN=*), INTENT(OUT) :: out_buf
    INTEGER, INTENT(OUT) :: actual_len
    TYPE(fah90_error_t), INTENT(OUT) :: err
    INTEGER :: rc, dummy_n
    CHARACTER(LEN=8) :: scratch_k
    scratch_k = ' '
    CALL fh7res(res%id, 1, 0, scratch_k, out_buf, actual_len, &
        dummy_n, rc)
    CALL fah90_make_error(rc, 'fah90_response_body', err)
  END SUBROUTINE fah90_response_body

  !> @brief Destroy a Fortran 90 HTTP response handle.
  !> @param[inout] res Response handle wrapper.
  !> @param[out] err Structured error status.
  SUBROUTINE fah90_response_destroy(res, err)
    TYPE(fah90_response_t), INTENT(INOUT) :: res
    TYPE(fah90_error_t), INTENT(OUT) :: err
    INTEGER :: rc
    rc = ihrsfr(res%id)
    IF (rc == 0) res%id = 0
    CALL fah90_make_error(rc, 'fah90_response_destroy', err)
  END SUBROUTINE fah90_response_destroy

  !> @brief Execute two requests concurrently via http_client_send_multi.
  !> @param[in] cli Client handle wrapper.
  !> @param[in] req1 First request handle wrapper.
  !> @param[in] req2 Second request handle wrapper.
  !> @param[out] res1 Completed response 1.
  !> @param[out] res2 Completed response 2.
  !> @param[out] err Structured error status.
  SUBROUTINE fah90_send_multi_two(cli, req1, req2, res1, res2, err)
    TYPE(fah90_client_t), INTENT(IN) :: cli
    TYPE(fah90_request_t), INTENT(IN) :: req1, req2
    TYPE(fah90_response_t), INTENT(OUT) :: res1, res2
    TYPE(fah90_error_t), INTENT(OUT) :: err
    INTEGER :: mh, futs(2), cnt, xerr, n2, rc, dm

    res1%id = 0
    res2%id = 0
    mh = 0
    CALL fh7mul(cli%id, mh, 1, 0, futs, 2, cnt, n2, rc)
    IF (rc == 0) CALL fh7mul(cli%id, mh, 2, req1%id, futs, 2, cnt, &
        n2, rc)
    IF (rc == 0) CALL fh7mul(cli%id, mh, 2, req2%id, futs, 2, cnt, &
        n2, rc)
    IF (rc == 0) CALL fh7mul(cli%id, mh, 3, 0, futs, 2, cnt, n2, rc)
    IF (rc == 0) THEN
      CALL fh7mul(cli%id, futs(1), 5, 0, futs, 2, xerr, n2, rc)
      IF (rc == 0 .AND. xerr == 0) THEN
        CALL fh7mul(cli%id, futs(1), 8, 0, futs, 2, res1%id, n2, rc)
      ELSE IF (rc == 0) THEN
        rc = xerr
      END IF
      IF (rc == 0) THEN
        CALL fh7mul(cli%id, futs(2), 5, 0, futs, 2, xerr, n2, rc)
        IF (rc == 0 .AND. xerr == 0) THEN
          CALL fh7mul(cli%id, futs(2), 8, 0, futs, 2, res2%id, n2, rc)
        ELSE IF (rc == 0) THEN
          rc = xerr
        END IF
      END IF
      CALL fh7mul(cli%id, futs(1), 9, 0, futs, 2, cnt, n2, dm)
      CALL fh7mul(cli%id, futs(2), 9, 0, futs, 2, cnt, n2, dm)
    END IF
    IF (mh /= 0) CALL fh7mul(cli%id, mh, 4, 0, futs, 2, cnt, n2, dm)
    CALL fah90_make_error(rc, 'fah90_send_multi_two', err)
  END SUBROUTINE fah90_send_multi_two

END MODULE fah_f90_mod
