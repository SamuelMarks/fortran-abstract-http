!> @file fah_f2018_mod.f90
!> @module fah_f2018_mod
!> @brief Strict ISO/IEC 1539-1:2018 (Fortran 2018 / TS 29113) SDK
!> module exposing assumed-type TYPE(*), assumed-rank DIMENSION(..), and
!> direct BIND(C) ALLOCATABLE response descriptors backed by our strict
!> ISO C89 CFI_cdesc_t shim (-std=f2018).
MODULE fah_f2018_mod
  USE, INTRINSIC :: ISO_C_BINDING
  USE fah_f2003_mod
  IMPLICIT NONE
  PRIVATE

  !> @brief TS 29113 / Fortran 2018 BIND(C) interfaces passing compiler
  !> CFI_cdesc_t descriptors directly to fah_c_bridge.
  INTERFACE
    !> @brief Ingest any scalar or rank 1..15 array (contiguous or
    !> strided) as an HTTP request body via CFI_cdesc_t.
    !> @param[in] req_h Request handle ID.
    !> @param[in] payload Assumed-type, assumed-rank Fortran object.
    !> @return fah_error_code_t integer.
    FUNCTION c_fah_req_set_body_cfi(req_h, payload) &
        BIND(C, NAME="fah_request_set_body_cfi") RESULT(rc)
      IMPORT :: C_INT
      INTEGER(C_INT), VALUE :: req_h
      TYPE(*), DIMENSION(..), INTENT(IN) :: payload
      INTEGER(C_INT) :: rc
    END FUNCTION c_fah_req_set_body_cfi

    !> @brief Allocate and populate a Fortran ALLOCATABLE byte array
    !> directly from a C HttpResponse via runtime CFI_allocate.
    !> @param[in] res_h Response handle ID.
    !> @param[out] out_bytes Rank-1 allocatable C_INT8_T buffer.
    !> @return fah_error_code_t integer.
    FUNCTION c_fah_res_body_cfi(res_h, out_bytes) &
        BIND(C, NAME="fah_response_body_cfi") RESULT(rc)
      IMPORT :: C_INT, C_INT8_T
      INTEGER(C_INT), VALUE :: res_h
      INTEGER(C_INT8_T), ALLOCATABLE, INTENT(OUT) :: out_bytes(:)
      INTEGER(C_INT) :: rc
    END FUNCTION c_fah_res_body_cfi
  END INTERFACE

  PUBLIC :: fah18_req_set_body_any, fah18_res_body_bytes

CONTAINS

  !> @brief Set any scalar, contiguous array, or strided array slice as
  !> the raw binary payload of req (Fortran 2018 TYPE(*), DIMENSION(..)).
  !> @param[inout] req Target fah_request object.
  !> @param[in] payload Any scalar or array of rank 0..15.
  !> @return fah_error status.
  FUNCTION fah18_req_set_body_any(req, payload) RESULT(err)
    CLASS(fah_request), INTENT(INOUT) :: req
    TYPE(*), DIMENSION(..), INTENT(IN) :: payload
    TYPE(fah_error) :: err
    INTEGER(C_INT) :: rc
    rc = c_fah_req_set_body_cfi(req%handle_id(), payload)
    err = fah03_from_code(rc, 'fah18_req_set_body_any')
  END FUNCTION fah18_req_set_body_any

  !> @brief Extract response body directly into a caller ALLOCATABLE
  !> INTEGER(C_INT8_T) array via TS 29113 CFI_allocate.
  !> @param[in] res Active fah_response object.
  !> @param[out] bytes_out Allocated byte array (size == body_len).
  !> @return fah_error status.
  FUNCTION fah18_res_body_bytes(res, bytes_out) RESULT(err)
    CLASS(fah_response), INTENT(IN) :: res
    INTEGER(C_INT8_T), ALLOCATABLE, INTENT(OUT) :: bytes_out(:)
    TYPE(fah_error) :: err
    INTEGER(C_INT) :: rc
    rc = c_fah_res_body_cfi(res%handle_id(), bytes_out)
    err = fah03_from_code(rc, 'fah18_res_body_bytes')
  END FUNCTION fah18_res_body_bytes

END MODULE fah_f2018_mod
