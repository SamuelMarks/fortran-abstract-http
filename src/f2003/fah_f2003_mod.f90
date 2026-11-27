!> @file fah_f2003_mod.f90
!> @module fah_f2003_mod
!> @brief Strict ISO/IEC 1539-1:2004 (Fortran 2003) Object-Oriented HTTP
!> SDK using ISO_C_BINDING, deferred-length allocatable strings,
!> type-bound procedures, and RAII FINAL destructors (-std=f2003).
MODULE fah_f2003_mod
  USE, INTRINSIC :: ISO_C_BINDING
  USE c_abstract_http_bind_mod
  IMPLICIT NONE
  PRIVATE

  !> @brief Unified BIND(C) error enumeration for Fortran 2003+ callers.
  ENUM, BIND(C)
    !> @brief Operation succeeded.
    ENUMERATOR :: FAH_OK = 0
    !> @brief Invalid argument.
    ENUMERATOR :: FAH_ERR_INVAL = -1
    !> @brief Out of memory.
    ENUMERATOR :: FAH_ERR_NOMEM = -2
    !> @brief Network I/O error.
    ENUMERATOR :: FAH_ERR_IO = -3
    !> @brief Operation not supported.
    ENUMERATOR :: FAH_ERR_NOTSUP = -4
    !> @brief Timeout expired.
    ENUMERATOR :: FAH_ERR_TIMEOUT = -5
    !> @brief Syscall failure.
    ENUMERATOR :: FAH_ERR_SYSCALL = -6
    !> @brief Null handle supplied.
    ENUMERATOR :: FAH_ERR_NULL_HANDLE = -2001
    !> @brief Handle type tag mismatch.
    ENUMERATOR :: FAH_ERR_BAD_HANDLE_TAG = -2002
    !> @brief Freed/stale handle generation.
    ENUMERATOR :: FAH_ERR_STALE_HANDLE = -2003
    !> @brief Caller buffer truncated.
    ENUMERATOR :: FAH_ERR_BUF_TOO_SMALL = -2005
  END ENUM

  PUBLIC :: FAH_OK, FAH_ERR_INVAL, FAH_ERR_NOMEM, FAH_ERR_IO
  PUBLIC :: FAH_ERR_NOTSUP, FAH_ERR_TIMEOUT, FAH_ERR_SYSCALL
  PUBLIC :: FAH_ERR_NULL_HANDLE, FAH_ERR_BAD_HANDLE_TAG
  PUBLIC :: FAH_ERR_STALE_HANDLE, FAH_ERR_BUF_TOO_SMALL

  !> @brief Direct BIND(C) interfaces to strict C89 fah_c_bridge.
  INTERFACE
    !> @brief Initialize global C transport layer.
    !> @return fah_error_code_t integer.
    FUNCTION c_fah_global_init() BIND(C, NAME="fah_global_init") &
        RESULT(rc)
      IMPORT :: C_INT
      INTEGER(C_INT) :: rc
    END FUNCTION c_fah_global_init

    !> @brief Clean up global C transport layer.
    !> @return fah_error_code_t integer.
    FUNCTION c_fah_global_cleanup() BIND(C, NAME="fah_global_cleanup") &
        RESULT(rc)
      IMPORT :: C_INT
      INTEGER(C_INT) :: rc
    END FUNCTION c_fah_global_cleanup

    !> @brief Create C client handle.
    !> @param[in] cfg_h Optional config handle.
    !> @param[in] use_raw Non-zero for raw sockets.
    !> @param[out] out_cli Client handle.
    !> @return fah_error_code_t integer.
    FUNCTION c_fah_client_create(cfg_h, use_raw, out_cli) &
        BIND(C, NAME="fah_client_create") RESULT(rc)
      IMPORT :: C_INT
      INTEGER(C_INT), VALUE :: cfg_h, use_raw
      INTEGER(C_INT), INTENT(OUT) :: out_cli
      INTEGER(C_INT) :: rc
    END FUNCTION c_fah_client_create

    !> @brief Set base URL on C client handle.
    !> @param[in] cli_h Client handle.
    !> @param[in] base_url NUL-terminated URL.
    !> @return fah_error_code_t integer.
    FUNCTION c_fah_client_set_base_url(cli_h, base_url) &
        BIND(C, NAME="fah_client_set_base_url") RESULT(rc)
      IMPORT :: C_INT, C_CHAR
      INTEGER(C_INT), VALUE :: cli_h
      CHARACTER(KIND=C_CHAR), DIMENSION(*), INTENT(IN) :: base_url
      INTEGER(C_INT) :: rc
    END FUNCTION c_fah_client_set_base_url

    !> @brief Destroy C client handle.
    !> @param[in] cli_h Client handle.
    !> @return fah_error_code_t integer.
    FUNCTION c_fah_client_destroy(cli_h) &
        BIND(C, NAME="fah_client_destroy") RESULT(rc)
      IMPORT :: C_INT
      INTEGER(C_INT), VALUE :: cli_h
      INTEGER(C_INT) :: rc
    END FUNCTION c_fah_client_destroy

    !> @brief Create C request handle.
    !> @param[out] out_req Request handle.
    !> @return fah_error_code_t integer.
    FUNCTION c_fah_request_create(out_req) &
        BIND(C, NAME="fah_request_create") RESULT(rc)
      IMPORT :: C_INT
      INTEGER(C_INT), INTENT(OUT) :: out_req
      INTEGER(C_INT) :: rc
    END FUNCTION c_fah_request_create

    !> @brief Destroy C request handle.
    !> @param[in] req_h Request handle.
    !> @return fah_error_code_t integer.
    FUNCTION c_fah_request_destroy(req_h) &
        BIND(C, NAME="fah_request_destroy") RESULT(rc)
      IMPORT :: C_INT
      INTEGER(C_INT), VALUE :: req_h
      INTEGER(C_INT) :: rc
    END FUNCTION c_fah_request_destroy

    !> @brief Set method and URL on C request handle.
    !> @param[in] req_h Request handle.
    !> @param[in] method HTTP verb enum.
    !> @param[in] url NUL-terminated URL.
    !> @return fah_error_code_t integer.
    FUNCTION c_fah_request_set_target(req_h, method, url) &
        BIND(C, NAME="fah_request_set_target") RESULT(rc)
      IMPORT :: C_INT, C_CHAR
      INTEGER(C_INT), VALUE :: req_h, method
      CHARACTER(KIND=C_CHAR), DIMENSION(*), INTENT(IN) :: url
      INTEGER(C_INT) :: rc
    END FUNCTION c_fah_request_set_target

    !> @brief Add header to C request handle.
    !> @param[in] req_h Request handle.
    !> @param[in] hkey NUL-terminated key.
    !> @param[in] hval NUL-terminated value.
    !> @return fah_error_code_t integer.
    FUNCTION c_fah_request_add_header(req_h, hkey, hval) &
        BIND(C, NAME="fah_request_add_header") RESULT(rc)
      IMPORT :: C_INT, C_CHAR
      INTEGER(C_INT), VALUE :: req_h
      CHARACTER(KIND=C_CHAR), DIMENSION(*), INTENT(IN) :: hkey, hval
      INTEGER(C_INT) :: rc
    END FUNCTION c_fah_request_add_header

    !> @brief Set raw body bytes on C request handle.
    !> @param[in] req_h Request handle.
    !> @param[in] body Pointer to body bytes.
    !> @param[in] body_len Byte length.
    !> @return fah_error_code_t integer.
    FUNCTION c_fah_request_set_body(req_h, body, body_len) &
        BIND(C, NAME="fah_request_set_body") RESULT(rc)
      IMPORT :: C_INT, C_PTR, C_SIZE_T
      INTEGER(C_INT), VALUE :: req_h
      TYPE(C_PTR), VALUE :: body
      INTEGER(C_SIZE_T), VALUE :: body_len
      INTEGER(C_INT) :: rc
    END FUNCTION c_fah_request_set_body

    !> @brief Set Bearer token on C request handle.
    !> @param[in] req_h Request handle.
    !> @param[in] token NUL-terminated token.
    !> @return fah_error_code_t integer.
    FUNCTION c_fah_auth_bearer(req_h, token) &
        BIND(C, NAME="fah_auth_bearer") RESULT(rc)
      IMPORT :: C_INT, C_CHAR
      INTEGER(C_INT), VALUE :: req_h
      CHARACTER(KIND=C_CHAR), DIMENSION(*), INTENT(IN) :: token
      INTEGER(C_INT) :: rc
    END FUNCTION c_fah_auth_bearer

    !> @brief Configure OAuth2 grant on C request handle.
    !> @param[in] req_h Request handle.
    !> @param[in] grant_kind Grant enum (1..9).
    !> @param[in] ep Endpoint URL.
    !> @param[in] p1 Parameter 1.
    !> @param[in] p2 Parameter 2.
    !> @param[in] p3 Parameter 3.
    !> @param[in] p4 Parameter 4.
    !> @param[in] p5 Parameter 5.
    !> @return fah_error_code_t integer.
    FUNCTION c_fah_oauth2_grant(req_h, grant_kind, ep, p1, p2, p3, &
        p4, p5) BIND(C, NAME="fah_oauth2_grant") RESULT(rc)
      IMPORT :: C_INT, C_CHAR
      INTEGER(C_INT), VALUE :: req_h, grant_kind
      CHARACTER(KIND=C_CHAR), DIMENSION(*), INTENT(IN) :: ep, p1, p2
      CHARACTER(KIND=C_CHAR), DIMENSION(*), INTENT(IN) :: p3, p4, p5
      INTEGER(C_INT) :: rc
    END FUNCTION c_fah_oauth2_grant

    !> @brief Execute HTTP send and mint response handle.
    !> @param[in] cli_h Client handle.
    !> @param[in] req_h Request handle.
    !> @param[out] out_res Response handle.
    !> @return fah_error_code_t integer.
    FUNCTION c_fah_send(cli_h, req_h, out_res) &
        BIND(C, NAME="fah_send") RESULT(rc)
      IMPORT :: C_INT
      INTEGER(C_INT), VALUE :: cli_h, req_h
      INTEGER(C_INT), INTENT(OUT) :: out_res
      INTEGER(C_INT) :: rc
    END FUNCTION c_fah_send

    !> @brief Query HTTP status code of response handle.
    !> @param[in] res_h Response handle.
    !> @param[out] out_status HTTP status code.
    !> @return fah_error_code_t integer.
    FUNCTION c_fah_response_status(res_h, out_status) &
        BIND(C, NAME="fah_response_status") RESULT(rc)
      IMPORT :: C_INT
      INTEGER(C_INT), VALUE :: res_h
      INTEGER(C_INT), INTENT(OUT) :: out_status
      INTEGER(C_INT) :: rc
    END FUNCTION c_fah_response_status

    !> @brief Obtain zero-copy C pointer and length of response body.
    !> @param[in] res_h Response handle.
    !> @param[out] out_ptr Raw body pointer.
    !> @param[out] out_len Byte length.
    !> @return fah_error_code_t integer.
    FUNCTION c_fah_response_body_ptr(res_h, out_ptr, out_len) &
        BIND(C, NAME="fah_response_body_ptr") RESULT(rc)
      IMPORT :: C_INT, C_PTR, C_SIZE_T
      INTEGER(C_INT), VALUE :: res_h
      TYPE(C_PTR), INTENT(OUT) :: out_ptr
      INTEGER(C_SIZE_T), INTENT(OUT) :: out_len
      INTEGER(C_INT) :: rc
    END FUNCTION c_fah_response_body_ptr

    !> @brief Look up response header into C buffer.
    !> @param[in] res_h Response handle.
    !> @param[in] hkey NUL-terminated header name.
    !> @param[out] out_buf Destination char array.
    !> @param[in] cap Buffer capacity.
    !> @param[out] out_len Full header value length.
    !> @param[in] space_pad Non-zero to space-pad.
    !> @return fah_error_code_t integer.
    FUNCTION c_fah_response_get_header(res_h, hkey, out_buf, cap, &
        out_len, space_pad) BIND(C, NAME="fah_response_get_header") &
        RESULT(rc)
      IMPORT :: C_INT, C_CHAR, C_SIZE_T
      INTEGER(C_INT), VALUE :: res_h, space_pad
      CHARACTER(KIND=C_CHAR), DIMENSION(*), INTENT(IN) :: hkey
      CHARACTER(KIND=C_CHAR), DIMENSION(*), INTENT(OUT) :: out_buf
      INTEGER(C_SIZE_T), VALUE :: cap
      INTEGER(C_SIZE_T), INTENT(OUT) :: out_len
      INTEGER(C_INT) :: rc
    END FUNCTION c_fah_response_get_header

    !> @brief Destroy C response handle.
    !> @param[in] res_h Response handle.
    !> @return fah_error_code_t integer.
    FUNCTION c_fah_response_destroy(res_h) &
        BIND(C, NAME="fah_response_destroy") RESULT(rc)
      IMPORT :: C_INT
      INTEGER(C_INT), VALUE :: res_h
      INTEGER(C_INT) :: rc
    END FUNCTION c_fah_response_destroy

    !> @brief Classify error code into category.
    !> @param[in] code Error code.
    !> @return Category enum.
    FUNCTION c_fah_error_category_of(code) &
        BIND(C, NAME="fah_error_category_of") RESULT(cat)
      IMPORT :: C_INT
      INTEGER(C_INT), VALUE :: code
      INTEGER(C_INT) :: cat
    END FUNCTION c_fah_error_category_of

    !> @brief Query whether error code is retryable.
    !> @param[in] code Error code.
    !> @return Non-zero if retryable.
    FUNCTION c_fah_error_is_retryable(code) &
        BIND(C, NAME="fah_error_is_retryable") RESULT(r)
      IMPORT :: C_INT
      INTEGER(C_INT), VALUE :: code
      INTEGER(C_INT) :: r
    END FUNCTION c_fah_error_is_retryable

    !> @brief Format structured error diagnostic into C buffer.
    !> @param[in] code Error code.
    !> @param[in] op_name NUL-terminated operation name.
    !> @param[out] out_buf Output char buffer.
    !> @param[in] buf_cap Buffer capacity.
    !> @param[out] out_len Full formatted length.
    !> @return fah_error_code_t integer.
    FUNCTION c_fah_error_format(code, op_name, out_buf, buf_cap, &
        out_len) BIND(C, NAME="fah_error_format") RESULT(rc)
      IMPORT :: C_INT, C_CHAR, C_SIZE_T
      INTEGER(C_INT), VALUE :: code
      CHARACTER(KIND=C_CHAR), DIMENSION(*), INTENT(IN) :: op_name
      CHARACTER(KIND=C_CHAR), DIMENSION(*), INTENT(OUT) :: out_buf
      INTEGER(C_SIZE_T), VALUE :: buf_cap
      INTEGER(C_SIZE_T), INTENT(OUT) :: out_len
      INTEGER(C_INT) :: rc
    END FUNCTION c_fah_error_format
  END INTERFACE

  !> @brief Object-Oriented Fortran 2003 unified error type.
  TYPE, PUBLIC :: fah_error
    !> @brief Numeric fah_error_code_t.
    INTEGER(C_INT) :: code = FAH_OK
    !> @brief Semantic category (0..8).
    INTEGER(C_INT) :: category = 0
    !> @brief .TRUE. if transient/retryable.
    LOGICAL :: retryable = .FALSE.
    !> @brief Exact-length allocatable diagnostic string.
    CHARACTER(LEN=:), ALLOCATABLE :: message
  CONTAINS
    !> @brief Return .TRUE. if code == FAH_OK.
    PROCEDURE :: is_ok => fah03_err_is_ok
    !> @brief Return formatted diagnostic string.
    PROCEDURE :: display => fah03_err_display
  END TYPE fah_error

  !> @brief RAII Fortran 2003 HTTP request class.
  TYPE, PUBLIC :: fah_request
    PRIVATE
    !> @brief Boxed C handle ID (unallocated when closed).
    INTEGER(C_INT), ALLOCATABLE :: h(:)
  CONTAINS
    !> @brief Initialize request handle.
    PROCEDURE :: init => fah03_req_init
    !> @brief Set HTTP verb and URL.
    PROCEDURE :: set_target => fah03_req_set_target
    !> @brief Add HTTP request header.
    PROCEDURE :: add_header => fah03_req_add_header
    !> @brief Set UTF-8/ASCII payload string.
    PROCEDURE :: set_body_text => fah03_req_set_body_text
    !> @brief Set Bearer token authorization.
    PROCEDURE :: set_bearer => fah03_req_set_bearer
    !> @brief Configure OAuth2 Client Credentials grant.
    PROCEDURE :: set_oauth2_client_creds => fah03_req_set_oauth2_cc
    !> @brief Query underlying handle ID.
    PROCEDURE :: handle_id => fah03_req_handle_id
    !> @brief Explicitly release request resources.
    PROCEDURE :: free => fah03_req_free
    !> @brief Automatic RAII destructor on scope exit.
    FINAL :: fah03_req_final
  END TYPE fah_request

  !> @brief RAII Fortran 2003 HTTP response class.
  TYPE, PUBLIC :: fah_response
    PRIVATE
    !> @brief Boxed C handle ID (unallocated when closed).
    INTEGER(C_INT), ALLOCATABLE :: h(:)
  CONTAINS
    !> @brief Read HTTP status code.
    PROCEDURE :: status => fah03_res_status
    !> @brief Extract response body as allocatable CHARACTER(LEN=:).
    PROCEDURE :: body_string => fah03_res_body_string
    !> @brief Look up response header as allocatable CHARACTER(LEN=:).
    PROCEDURE :: header => fah03_res_header
    !> @brief Query underlying handle ID.
    PROCEDURE :: handle_id => fah03_res_handle_id
    !> @brief Bind raw integer handle into wrapper.
    PROCEDURE :: adopt_handle => fah03_res_adopt
    !> @brief Explicitly release response resources.
    PROCEDURE :: free => fah03_res_free
    !> @brief Automatic RAII destructor on scope exit.
    FINAL :: fah03_res_final
  END TYPE fah_response

  !> @brief RAII Fortran 2003 HTTP client class.
  TYPE, PUBLIC :: fah_client
    PRIVATE
    !> @brief Boxed C handle ID (unallocated when closed).
    INTEGER(C_INT), ALLOCATABLE :: h(:)
  CONTAINS
    !> @brief Open HTTP client handle.
    PROCEDURE :: open => fah03_client_open
    !> @brief Set base URL for relative paths.
    PROCEDURE :: set_base_url => fah03_client_set_base_url
    !> @brief Send fah_request and populate fah_response.
    PROCEDURE :: send => fah03_client_send
    !> @brief Query underlying handle ID.
    PROCEDURE :: handle_id => fah03_client_handle_id
    !> @brief Explicitly close client handle.
    PROCEDURE :: close => fah03_client_close
    !> @brief Automatic RAII destructor on scope exit.
    FINAL :: fah03_client_final
  END TYPE fah_client

  PUBLIC :: fah03_from_code, fah03_global_init, fah03_global_cleanup
  PUBLIC :: c_fah_response_body_ptr

CONTAINS

  !> @brief Store or clear an allocatable 1-element C handle box.
  !> @param[inout] box Allocatable 1-element handle box.
  !> @param[in] new_h New positive handle (or <= 0 to deallocate).
  SUBROUTINE fah03_store_h(box, new_h)
    INTEGER(C_INT), ALLOCATABLE, INTENT(INOUT) :: box(:)
    INTEGER(C_INT), INTENT(IN) :: new_h
    IF (new_h > 0_C_INT) THEN
      IF (.NOT. ALLOCATED(box)) ALLOCATE(box(1))
      box(1) = new_h
    ELSE IF (ALLOCATED(box)) THEN
      DEALLOCATE(box)
    END IF
  END SUBROUTINE fah03_store_h

  !> @brief Construct a fah_error from a numeric C status code and op.
  !> @param[in] code Numeric fah_error_code_t.
  !> @param[in] op_name Originating method name.
  !> @return Populated fah_error instance.
  FUNCTION fah03_from_code(code, op_name) RESULT(err)
    INTEGER(C_INT), INTENT(IN) :: code
    CHARACTER(LEN=*), INTENT(IN) :: op_name
    TYPE(fah_error) :: err
    CHARACTER(KIND=C_CHAR) :: cbuf(256)
    INTEGER(C_SIZE_T) :: flen, i, n
    INTEGER(C_INT) :: frc

    err%code = code
    IF (code == FAH_OK) THEN
      err%category = 0
      err%retryable = .FALSE.
      ALLOCATE(CHARACTER(LEN=2) :: err%message)
      err%message = 'OK'
      RETURN
    END IF
    err%category = c_fah_error_category_of(code)
    err%retryable = (c_fah_error_is_retryable(code) /= 0)
    frc = c_fah_error_format(code, TRIM(op_name) // C_NULL_CHAR, cbuf, &
        256_C_SIZE_T, flen)
    n = MIN(flen, 255_C_SIZE_T)
    ALLOCATE(CHARACTER(LEN=INT(n)) :: err%message)
    DO i = 1_C_SIZE_T, n
      err%message(INT(i):INT(i)) = cbuf(i)
    END DO
  END FUNCTION fah03_from_code

  !> @brief Check whether this fah_error represents FAH_OK.
  !> @param[in] self Receiver fah_error.
  !> @return .TRUE. on FAH_OK.
  PURE LOGICAL FUNCTION fah03_err_is_ok(self)
    CLASS(fah_error), INTENT(IN) :: self
    fah03_err_is_ok = (self%code == FAH_OK)
  END FUNCTION fah03_err_is_ok

  !> @brief Return human-readable diagnostic text of self.
  !> @param[in] self Receiver fah_error.
  !> @return Allocatable diagnostic string.
  FUNCTION fah03_err_display(self) RESULT(s)
    CLASS(fah_error), INTENT(IN) :: self
    CHARACTER(LEN=:), ALLOCATABLE :: s
    IF (ALLOCATED(self%message)) THEN
      ALLOCATE(CHARACTER(LEN=LEN(self%message)) :: s)
      s = self%message
    ELSE
      ALLOCATE(CHARACTER(LEN=2) :: s)
      s = 'OK'
    END IF
  END FUNCTION fah03_err_display

  !> @brief Initialize global C transport layer (Fortran 2003).
  !> @return fah_error value.
  FUNCTION fah03_global_init() RESULT(err)
    TYPE(fah_error) :: err
    err = fah03_from_code(c_fah_global_init(), 'fah03_global_init')
  END FUNCTION fah03_global_init

  !> @brief Clean up global C transport layer (Fortran 2003).
  !> @return fah_error value.
  FUNCTION fah03_global_cleanup() RESULT(err)
    TYPE(fah_error) :: err
    err = fah03_from_code(c_fah_global_cleanup(), &
        'fah03_global_cleanup')
  END FUNCTION fah03_global_cleanup

  !> @brief Open a fah_client instance.
  !> @param[inout] self Receiver client object.
  !> @param[in] use_raw Optional raw-socket selector (default .TRUE.).
  !> @return fah_error status.
  FUNCTION fah03_client_open(self, use_raw) RESULT(err)
    CLASS(fah_client), INTENT(INOUT) :: self
    LOGICAL, OPTIONAL, INTENT(IN) :: use_raw
    TYPE(fah_error) :: err
    INTEGER(C_INT) :: iraw, rc, dm, raw_h
    IF (self%handle_id() > 0_C_INT) &
        dm = c_fah_client_destroy(self%handle_id())
    CALL fah03_store_h(self%h, 0_C_INT)
    iraw = 1_C_INT
    IF (PRESENT(use_raw)) THEN
      IF (.NOT. use_raw) iraw = 0_C_INT
    END IF
    rc = c_fah_client_create(0_C_INT, iraw, raw_h)
    IF (rc == FAH_OK) CALL fah03_store_h(self%h, raw_h)
    err = fah03_from_code(rc, 'fah_client%open')
  END FUNCTION fah03_client_open

  !> @brief Set base_url on a fah_client instance.
  !> @param[inout] self Receiver client object.
  !> @param[in] base_url Base URL prefix.
  !> @return fah_error status.
  FUNCTION fah03_client_set_base_url(self, base_url) RESULT(err)
    CLASS(fah_client), INTENT(INOUT) :: self
    CHARACTER(LEN=*), INTENT(IN) :: base_url
    TYPE(fah_error) :: err
    INTEGER(C_INT) :: rc
    rc = c_fah_client_set_base_url(self%handle_id(), &
        TRIM(base_url) // C_NULL_CHAR)
    err = fah03_from_code(rc, 'fah_client%set_base_url')
  END FUNCTION fah03_client_set_base_url

  !> @brief Send a fah_request through self and populate res.
  !> @param[in] self Receiver client object.
  !> @param[in] req Configured request object.
  !> @param[inout] res Output response object.
  !> @return fah_error status.
  FUNCTION fah03_client_send(self, req, res) RESULT(err)
    CLASS(fah_client), INTENT(IN) :: self
    CLASS(fah_request), INTENT(IN) :: req
    CLASS(fah_response), INTENT(INOUT) :: res
    TYPE(fah_error) :: err
    INTEGER(C_INT) :: rc, new_res_h, dm
    IF (res%handle_id() > 0_C_INT) &
        dm = c_fah_response_destroy(res%handle_id())
    CALL fah03_store_h(res%h, 0_C_INT)
    rc = c_fah_send(self%handle_id(), req%handle_id(), new_res_h)
    IF (rc == FAH_OK) CALL fah03_store_h(res%h, new_res_h)
    err = fah03_from_code(rc, 'fah_client%send')
  END FUNCTION fah03_client_send

  !> @brief Return integer handle ID of client.
  !> @param[in] self Receiver client object.
  !> @return Handle integer.
  PURE INTEGER(C_INT) FUNCTION fah03_client_handle_id(self)
    CLASS(fah_client), INTENT(IN) :: self
    IF (ALLOCATED(self%h)) THEN
      fah03_client_handle_id = self%h(1)
    ELSE
      fah03_client_handle_id = 0_C_INT
    END IF
  END FUNCTION fah03_client_handle_id

  !> @brief Explicitly close a fah_client instance.
  !> @param[inout] self Receiver client object.
  !> @return fah_error status.
  FUNCTION fah03_client_close(self) RESULT(err)
    CLASS(fah_client), INTENT(INOUT) :: self
    TYPE(fah_error) :: err
    INTEGER(C_INT) :: rc
    rc = c_fah_client_destroy(self%handle_id())
    IF (rc == FAH_OK) CALL fah03_store_h(self%h, 0_C_INT)
    err = fah03_from_code(rc, 'fah_client%close')
  END FUNCTION fah03_client_close

  !> @brief RAII FINAL destructor releasing client handle if open.
  !> @param[inout] self Finalized client instance.
  SUBROUTINE fah03_client_final(self)
    TYPE(fah_client), INTENT(INOUT) :: self
    INTEGER(C_INT) :: dm
    IF (ALLOCATED(self%h)) THEN
      dm = c_fah_client_destroy(self%h(1))
      DEALLOCATE(self%h)
    END IF
  END SUBROUTINE fah03_client_final

  !> @brief Initialize a fah_request instance.
  !> @param[inout] self Receiver request object.
  !> @return fah_error status.
  FUNCTION fah03_req_init(self) RESULT(err)
    CLASS(fah_request), INTENT(INOUT) :: self
    TYPE(fah_error) :: err
    INTEGER(C_INT) :: rc, dm, raw_h
    IF (self%handle_id() > 0_C_INT) &
        dm = c_fah_request_destroy(self%handle_id())
    CALL fah03_store_h(self%h, 0_C_INT)
    rc = c_fah_request_create(raw_h)
    IF (rc == FAH_OK) CALL fah03_store_h(self%h, raw_h)
    err = fah03_from_code(rc, 'fah_request%init')
  END FUNCTION fah03_req_init

  !> @brief Set HTTP verb and URL on self.
  !> @param[inout] self Receiver request object.
  !> @param[in] method HTTP verb enum (0=GET, 1=POST, ...).
  !> @param[in] url Target URL.
  !> @return fah_error status.
  FUNCTION fah03_req_set_target(self, method, url) RESULT(err)
    CLASS(fah_request), INTENT(INOUT) :: self
    INTEGER(C_INT), INTENT(IN) :: method
    CHARACTER(LEN=*), INTENT(IN) :: url
    TYPE(fah_error) :: err
    INTEGER(C_INT) :: rc
    rc = c_fah_request_set_target(self%handle_id(), method, &
        TRIM(url) // C_NULL_CHAR)
    err = fah03_from_code(rc, 'fah_request%set_target')
  END FUNCTION fah03_req_set_target

  !> @brief Add an HTTP header onto self.
  !> @param[inout] self Receiver request object.
  !> @param[in] hkey Header name.
  !> @param[in] hval Header value.
  !> @return fah_error status.
  FUNCTION fah03_req_add_header(self, hkey, hval) RESULT(err)
    CLASS(fah_request), INTENT(INOUT) :: self
    CHARACTER(LEN=*), INTENT(IN) :: hkey, hval
    TYPE(fah_error) :: err
    INTEGER(C_INT) :: rc
    rc = c_fah_request_add_header(self%handle_id(), &
        TRIM(hkey) // C_NULL_CHAR, TRIM(hval) // C_NULL_CHAR)
    err = fah03_from_code(rc, 'fah_request%add_header')
  END FUNCTION fah03_req_add_header

  !> @brief Set exact CHARACTER string payload on self via C_LOC.
  !> @param[inout] self Receiver request object.
  !> @param[in] body Payload string (exact LEN(body) bytes sent).
  !> @return fah_error status.
  FUNCTION fah03_req_set_body_text(self, body) RESULT(err)
    CLASS(fah_request), INTENT(INOUT) :: self
    CHARACTER(LEN=*), INTENT(IN) :: body
    TYPE(fah_error) :: err
    CHARACTER(KIND=C_CHAR), ALLOCATABLE, TARGET :: cbuf(:)
    INTEGER :: n, i
    INTEGER(C_INT) :: rc
    n = LEN(body)
    ALLOCATE(cbuf(MAX(1, n)))
    DO i = 1, n
      cbuf(i) = body(i:i)
    END DO
    rc = c_fah_request_set_body(self%handle_id(), C_LOC(cbuf(1)), &
        INT(n, C_SIZE_T))
    DEALLOCATE(cbuf)
    err = fah03_from_code(rc, 'fah_request%set_body_text')
  END FUNCTION fah03_req_set_body_text

  !> @brief Set Bearer token authorization on self.
  !> @param[inout] self Receiver request object.
  !> @param[in] token Bearer token string.
  !> @return fah_error status.
  FUNCTION fah03_req_set_bearer(self, token) RESULT(err)
    CLASS(fah_request), INTENT(INOUT) :: self
    CHARACTER(LEN=*), INTENT(IN) :: token
    TYPE(fah_error) :: err
    INTEGER(C_INT) :: rc
    rc = c_fah_auth_bearer(self%handle_id(), &
        TRIM(token) // C_NULL_CHAR)
    err = fah03_from_code(rc, 'fah_request%set_bearer')
  END FUNCTION fah03_req_set_bearer

  !> @brief Configure OAuth 2.0 Client Credentials grant on self.
  !> @param[inout] self Receiver request object.
  !> @param[in] token_url Token endpoint URL.
  !> @param[in] client_id Client ID.
  !> @param[in] client_secret Client secret.
  !> @param[in] scope Optional scope string.
  !> @return fah_error status.
  FUNCTION fah03_req_set_oauth2_cc(self, token_url, client_id, &
      client_secret, scope) RESULT(err)
    CLASS(fah_request), INTENT(INOUT) :: self
    CHARACTER(LEN=*), INTENT(IN) :: token_url, client_id, client_secret
    CHARACTER(LEN=*), OPTIONAL, INTENT(IN) :: scope
    TYPE(fah_error) :: err
    INTEGER(C_INT) :: rc
    IF (PRESENT(scope)) THEN
      rc = c_fah_oauth2_grant(self%handle_id(), 4_C_INT, &
          TRIM(token_url) // C_NULL_CHAR, &
          TRIM(client_id) // C_NULL_CHAR, &
          TRIM(client_secret) // C_NULL_CHAR, &
          TRIM(scope) // C_NULL_CHAR, C_NULL_CHAR, C_NULL_CHAR)
    ELSE
      rc = c_fah_oauth2_grant(self%handle_id(), 4_C_INT, &
          TRIM(token_url) // C_NULL_CHAR, &
          TRIM(client_id) // C_NULL_CHAR, &
          TRIM(client_secret) // C_NULL_CHAR, &
          C_NULL_CHAR, C_NULL_CHAR, C_NULL_CHAR)
    END IF
    err = fah03_from_code(rc, 'fah_request%set_oauth2_client_creds')
  END FUNCTION fah03_req_set_oauth2_cc

  !> @brief Return integer handle ID of request.
  !> @param[in] self Receiver request object.
  !> @return Handle integer.
  PURE INTEGER(C_INT) FUNCTION fah03_req_handle_id(self)
    CLASS(fah_request), INTENT(IN) :: self
    IF (ALLOCATED(self%h)) THEN
      fah03_req_handle_id = self%h(1)
    ELSE
      fah03_req_handle_id = 0_C_INT
    END IF
  END FUNCTION fah03_req_handle_id

  !> @brief Explicitly free a fah_request instance.
  !> @param[inout] self Receiver request object.
  !> @return fah_error status.
  FUNCTION fah03_req_free(self) RESULT(err)
    CLASS(fah_request), INTENT(INOUT) :: self
    TYPE(fah_error) :: err
    INTEGER(C_INT) :: rc
    rc = c_fah_request_destroy(self%handle_id())
    IF (rc == FAH_OK) CALL fah03_store_h(self%h, 0_C_INT)
    err = fah03_from_code(rc, 'fah_request%free')
  END FUNCTION fah03_req_free

  !> @brief RAII FINAL destructor releasing request handle if open.
  !> @param[inout] self Finalized request instance.
  SUBROUTINE fah03_req_final(self)
    TYPE(fah_request), INTENT(INOUT) :: self
    INTEGER(C_INT) :: dm
    IF (ALLOCATED(self%h)) THEN
      dm = c_fah_request_destroy(self%h(1))
      DEALLOCATE(self%h)
    END IF
  END SUBROUTINE fah03_req_final

  !> @brief Query HTTP status code on a fah_response instance.
  !> @param[in] self Receiver response object.
  !> @param[out] status_code HTTP status code.
  !> @return fah_error status.
  FUNCTION fah03_res_status(self, status_code) RESULT(err)
    CLASS(fah_response), INTENT(IN) :: self
    INTEGER(C_INT), INTENT(OUT) :: status_code
    TYPE(fah_error) :: err
    INTEGER(C_INT) :: rc
    status_code = 0_C_INT
    rc = c_fah_response_status(self%handle_id(), status_code)
    err = fah03_from_code(rc, 'fah_response%status')
  END FUNCTION fah03_res_status

  !> @brief Extract response body into exact-length allocatable string.
  !> @param[in] self Receiver response object.
  !> @param[out] body_out Allocatable CHARACTER(LEN=:) string.
  !> @return fah_error status.
  FUNCTION fah03_res_body_string(self, body_out) RESULT(err)
    CLASS(fah_response), INTENT(IN) :: self
    CHARACTER(LEN=:), ALLOCATABLE, INTENT(OUT) :: body_out
    TYPE(fah_error) :: err
    TYPE(C_PTR) :: bptr
    INTEGER(C_SIZE_T) :: blen, i
    CHARACTER(KIND=C_CHAR), POINTER :: ch_view(:)
    INTEGER(C_INT) :: rc

    rc = c_fah_response_body_ptr(self%handle_id(), bptr, blen)
    IF (rc /= FAH_OK) THEN
      ALLOCATE(CHARACTER(LEN=0) :: body_out)
      err = fah03_from_code(rc, 'fah_response%body_string')
      RETURN
    END IF
    ALLOCATE(CHARACTER(LEN=INT(blen)) :: body_out)
    IF (blen > 0_C_SIZE_T .AND. C_ASSOCIATED(bptr)) THEN
      CALL C_F_POINTER(bptr, ch_view, (/ INT(blen) /))
      DO i = 1_C_SIZE_T, blen
        body_out(INT(i):INT(i)) = ch_view(i)
      END DO
    END IF
    err = fah03_from_code(FAH_OK, 'fah_response%body_string')
  END FUNCTION fah03_res_body_string

  !> @brief Look up a response header as allocatable CHARACTER(LEN=:).
  !> @param[in] self Receiver response object.
  !> @param[in] hkey Case-insensitive header name.
  !> @param[out] val_out Allocatable header value.
  !> @return fah_error status.
  FUNCTION fah03_res_header(self, hkey, val_out) RESULT(err)
    CLASS(fah_response), INTENT(IN) :: self
    CHARACTER(LEN=*), INTENT(IN) :: hkey
    CHARACTER(LEN=:), ALLOCATABLE, INTENT(OUT) :: val_out
    TYPE(fah_error) :: err
    CHARACTER(KIND=C_CHAR) :: cbuf(512)
    INTEGER(C_SIZE_T) :: vlen, n, i
    INTEGER(C_INT) :: rc

    rc = c_fah_response_get_header(self%handle_id(), &
        TRIM(hkey) // C_NULL_CHAR, cbuf, 512_C_SIZE_T, vlen, 0_C_INT)
    IF (rc /= FAH_OK .AND. rc /= FAH_ERR_BUF_TOO_SMALL) THEN
      ALLOCATE(CHARACTER(LEN=0) :: val_out)
      err = fah03_from_code(rc, 'fah_response%header')
      RETURN
    END IF
    n = MIN(vlen, 512_C_SIZE_T)
    ALLOCATE(CHARACTER(LEN=INT(n)) :: val_out)
    DO i = 1_C_SIZE_T, n
      val_out(INT(i):INT(i)) = cbuf(i)
    END DO
    err = fah03_from_code(rc, 'fah_response%header')
  END FUNCTION fah03_res_header

  !> @brief Return integer handle ID of response.
  !> @param[in] self Receiver response object.
  !> @return Handle integer.
  PURE INTEGER(C_INT) FUNCTION fah03_res_handle_id(self)
    CLASS(fah_response), INTENT(IN) :: self
    IF (ALLOCATED(self%h)) THEN
      fah03_res_handle_id = self%h(1)
    ELSE
      fah03_res_handle_id = 0_C_INT
    END IF
  END FUNCTION fah03_res_handle_id

  !> @brief Adopt a raw C response handle ID into self.
  !> @param[inout] self Receiver response object.
  !> @param[in] raw_h Raw positive handle ID.
  SUBROUTINE fah03_res_adopt(self, raw_h)
    CLASS(fah_response), INTENT(INOUT) :: self
    INTEGER(C_INT), INTENT(IN) :: raw_h
    INTEGER(C_INT) :: dm
    IF (self%handle_id() > 0_C_INT) &
        dm = c_fah_response_destroy(self%handle_id())
    CALL fah03_store_h(self%h, raw_h)
  END SUBROUTINE fah03_res_adopt

  !> @brief Explicitly free a fah_response instance.
  !> @param[inout] self Receiver response object.
  !> @return fah_error status.
  FUNCTION fah03_res_free(self) RESULT(err)
    CLASS(fah_response), INTENT(INOUT) :: self
    TYPE(fah_error) :: err
    INTEGER(C_INT) :: rc
    rc = c_fah_response_destroy(self%handle_id())
    IF (rc == FAH_OK) CALL fah03_store_h(self%h, 0_C_INT)
    err = fah03_from_code(rc, 'fah_response%free')
  END FUNCTION fah03_res_free

  !> @brief RAII FINAL destructor releasing response handle if open.
  !> @param[inout] self Finalized response instance.
  SUBROUTINE fah03_res_final(self)
    TYPE(fah_response), INTENT(INOUT) :: self
    INTEGER(C_INT) :: dm
    IF (ALLOCATED(self%h)) THEN
      dm = c_fah_response_destroy(self%h(1))
      DEALLOCATE(self%h)
    END IF
  END SUBROUTINE fah03_res_final

END MODULE fah_f2003_mod
