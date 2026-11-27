!> @file fah_f2023_mod.f90
!> @module fah_f2023_mod
!> @brief Strict ISO/IEC 1539-1:2023 (Fortran 2023) HTTP SDK module
!> leveraging intrinsic F_C_STRING (with optional ASIS) and zero-copy
!> deferred-length CHARACTER(LEN=:) pointer views (-std=f2023).
MODULE fah_f2023_mod
  USE, INTRINSIC :: ISO_C_BINDING, ONLY: C_INT, C_CHAR, C_PTR, &
      C_SIZE_T, C_ASSOCIATED, C_F_POINTER
  USE fah_f2023_fcstr_mod, ONLY: F_C_STRING
  USE fah_f2003_mod
  IMPLICIT NONE
  PRIVATE

  !> @brief Empty static character target for zero-length body views.
  CHARACTER(LEN=0, KIND=C_CHAR), TARGET, SAVE :: k_empty_cstr = ''

  !> @brief Direct BIND(C) entry points fed by Fortran 2023 F_C_STRING.
  INTERFACE
    !> @brief Set HTTP verb and NUL-terminated target URL on req_h.
    !> @param[in] req_h Request handle ID.
    !> @param[in] method HTTP verb enum (0=GET, 1=POST, ...).
    !> @param[in] url NUL-terminated C string produced by F_C_STRING.
    !> @return fah_error_code_t integer.
    FUNCTION c23_req_set_target(req_h, method, url) &
        BIND(C, NAME="fah_request_set_target") RESULT(rc)
      IMPORT :: C_INT, C_CHAR
      INTEGER(C_INT), VALUE :: req_h, method
      CHARACTER(KIND=C_CHAR), DIMENSION(*), INTENT(IN) :: url
      INTEGER(C_INT) :: rc
    END FUNCTION c23_req_set_target

    !> @brief Add HTTP header onto req_h using F_C_STRING buffers.
    !> @param[in] req_h Request handle ID.
    !> @param[in] hkey NUL-terminated header key.
    !> @param[in] hval NUL-terminated header value.
    !> @return fah_error_code_t integer.
    FUNCTION c23_req_add_header(req_h, hkey, hval) &
        BIND(C, NAME="fah_request_add_header") RESULT(rc)
      IMPORT :: C_INT, C_CHAR
      INTEGER(C_INT), VALUE :: req_h
      CHARACTER(KIND=C_CHAR), DIMENSION(*), INTENT(IN) :: hkey, hval
      INTEGER(C_INT) :: rc
    END FUNCTION c23_req_add_header

    !> @brief Set Bearer token authorization onto req_h.
    !> @param[in] req_h Request handle ID.
    !> @param[in] token NUL-terminated Bearer token.
    !> @return fah_error_code_t integer.
    FUNCTION c23_auth_bearer(req_h, token) &
        BIND(C, NAME="fah_auth_bearer") RESULT(rc)
      IMPORT :: C_INT, C_CHAR
      INTEGER(C_INT), VALUE :: req_h
      CHARACTER(KIND=C_CHAR), DIMENSION(*), INTENT(IN) :: token
      INTEGER(C_INT) :: rc
    END FUNCTION c23_auth_bearer
  END INTERFACE

  PUBLIC :: fah23_to_cstring, fah23_response_strpointer
  PUBLIC :: fah23_get, fah23_post

CONTAINS

  !> @brief Convert a Fortran CHARACTER scalar into a NUL-terminated
  !> C_CHAR string via the Fortran 2023 F_C_STRING intrinsic.
  !> @param[in] fstr Input Fortran character string.
  !> @param[in] asis Optional flag: .TRUE. preserves trailing spaces.
  !> @return Allocatable NUL-terminated C_CHAR string.
  PURE FUNCTION fah23_to_cstring(fstr, asis) RESULT(cstr)
    CHARACTER(LEN=*, KIND=C_CHAR), INTENT(IN) :: fstr
    LOGICAL, OPTIONAL, INTENT(IN) :: asis
    CHARACTER(LEN=:, KIND=C_CHAR), ALLOCATABLE :: cstr

    IF (PRESENT(asis)) THEN
      cstr = F_C_STRING(fstr, ASIS=asis)
    ELSE
      cstr = F_C_STRING(fstr)
    END IF
  END FUNCTION fah23_to_cstring

  !> @brief Bind a zero-copy deferred-length CHARACTER(LEN=:) pointer
  !> directly to res's C payload buffer via Fortran 2023 C_F_STRPOINTER.
  !> @param[in] res Active fah_response object.
  !> @param[out] str_ptr Zero-copy scalar string pointer view.
  !> @return fah_error status (NULLIFY(str_ptr) on handle error).
  FUNCTION fah23_response_strpointer(res, str_ptr) RESULT(err)
    CLASS(fah_response), INTENT(IN) :: res
    CHARACTER(LEN=:, KIND=C_CHAR), POINTER, INTENT(OUT) :: str_ptr
    TYPE(fah_error) :: err
    TYPE(C_PTR) :: raw_ptr
    INTEGER(C_SIZE_T) :: blen
    INTEGER(C_INT) :: rc

    rc = c_fah_response_body_ptr(res%handle_id(), raw_ptr, blen)
    IF (rc /= FAH_OK) THEN
      NULLIFY(str_ptr)
      err = fah03_from_code(rc, 'fah23_response_strpointer')
      RETURN
    END IF
    IF (blen == 0_C_SIZE_T .OR. .NOT. C_ASSOCIATED(raw_ptr)) THEN
      str_ptr => k_empty_cstr
    ELSE
      BLOCK
        CHARACTER(LEN=blen, KIND=C_CHAR), POINTER :: fixed_ptr
        CALL C_F_POINTER(raw_ptr, fixed_ptr)
        str_ptr => fixed_ptr
      END BLOCK
    END IF
    err = fah03_from_code(FAH_OK, 'fah23_response_strpointer')
  END FUNCTION fah23_response_strpointer

  !> @brief Execute an ergonomic one-shot HTTP GET request using Fortran
  !> 2023 F_C_STRING string marshalling.
  !> @param[in] cli Open fah_client instance.
  !> @param[in] url Target URL (trailing blanks trimmed via F_C_STRING).
  !> @param[inout] res Output fah_response instance.
  !> @param[in] bearer_token Optional exact Bearer token (ASIS=.TRUE.).
  !> @return fah_error status.
  FUNCTION fah23_get(cli, url, res, bearer_token) RESULT(err)
    CLASS(fah_client), INTENT(IN) :: cli
    CHARACTER(LEN=*, KIND=C_CHAR), INTENT(IN) :: url
    CLASS(fah_response), INTENT(INOUT) :: res
    CHARACTER(LEN=*, KIND=C_CHAR), OPTIONAL, INTENT(IN) :: bearer_token
    TYPE(fah_error) :: err
    TYPE(fah_request) :: req
    INTEGER(C_INT) :: rc

    err = req%init()
    IF (.NOT. err%is_ok()) RETURN
    rc = c23_req_set_target(req%handle_id(), 0_C_INT, F_C_STRING(url))
    err = fah03_from_code(rc, 'fah23_get%set_target')
    IF (err%is_ok() .AND. PRESENT(bearer_token)) THEN
      rc = c23_auth_bearer(req%handle_id(), &
          F_C_STRING(bearer_token, ASIS=.TRUE.))
      err = fah03_from_code(rc, 'fah23_get%bearer')
    END IF
    IF (err%is_ok()) err = cli%send(req, res)
  END FUNCTION fah23_get

  !> @brief Execute an ergonomic one-shot HTTP POST request using
  !> Fortran 2023 F_C_STRING marshalling.
  !> @param[in] cli Open fah_client instance.
  !> @param[in] url Target URL (trailing blanks trimmed via F_C_STRING).
  !> @param[in] content_type Content-Type header value.
  !> @param[in] body Exact request body payload string.
  !> @param[inout] res Output fah_response instance.
  !> @return fah_error status.
  FUNCTION fah23_post(cli, url, content_type, body, res) RESULT(err)
    CLASS(fah_client), INTENT(IN) :: cli
    CHARACTER(LEN=*, KIND=C_CHAR), INTENT(IN) :: url
    CHARACTER(LEN=*, KIND=C_CHAR), INTENT(IN) :: content_type
    CHARACTER(LEN=*, KIND=C_CHAR), INTENT(IN) :: body
    CLASS(fah_response), INTENT(INOUT) :: res
    TYPE(fah_error) :: err
    TYPE(fah_request) :: req
    INTEGER(C_INT) :: rc

    err = req%init()
    IF (.NOT. err%is_ok()) RETURN
    rc = c23_req_set_target(req%handle_id(), 1_C_INT, F_C_STRING(url))
    err = fah03_from_code(rc, 'fah23_post%set_target')
    IF (err%is_ok()) THEN
      rc = c23_req_add_header(req%handle_id(), &
          F_C_STRING('Content-Type'), F_C_STRING(content_type))
      err = fah03_from_code(rc, 'fah23_post%add_header')
    END IF
    IF (err%is_ok()) err = req%set_body_text(body)
    IF (err%is_ok()) err = cli%send(req, res)
  END FUNCTION fah23_post

END MODULE fah_f2023_mod
