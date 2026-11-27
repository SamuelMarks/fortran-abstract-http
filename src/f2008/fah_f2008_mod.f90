!> @file fah_f2008_mod.f90
!> @module fah_f2008_mod
!> @brief Strict ISO/IEC 1539-1:2010 (Fortran 2008) HTTP interface
!> module declaring SUBMODULE procedures, CONTIGUOUS zero-copy byte
!> pointer views, ISO_FORTRAN_ENV kinds, and C_SIZEOF ABI verification.
MODULE fah_f2008_mod
  USE, INTRINSIC :: ISO_FORTRAN_ENV, ONLY: INT8, INT32, INT64
  USE, INTRINSIC :: ISO_C_BINDING
  USE fah_f2003_mod
  IMPLICIT NONE
  PRIVATE

  PUBLIC :: fah08_response_body_view, fah08_verify_c_abi_sizes

  !> @brief Separate module procedure interfaces implemented in
  !> SUBMODULE fah_f2008_impl_smod.
  INTERFACE
    !> @brief Obtain a zero-copy CONTIGUOUS INT8 pointer slice into a
    !> fah_response payload without heap allocation.
    !> @param[in] res Active fah_response instance.
    !> @param[out] view Contiguous byte pointer slice (empty if 0 len).
    !> @return fah_error status.
    MODULE FUNCTION fah08_response_body_view(res, view) RESULT(err)
      CLASS(fah_response), INTENT(IN) :: res
      INTEGER(INT8), POINTER, CONTIGUOUS, INTENT(OUT) :: view(:)
      TYPE(fah_error) :: err
    END FUNCTION fah08_response_body_view

    !> @brief Verify Fortran 2008 C_SIZEOF layout invariants on BIND(C)
    !> types exported by generated/c_abstract_http_bind_mod.f90.
    !> @param[out] req_bytes Byte size of generated httprequest type.
    !> @param[out] res_bytes Byte size of generated httpresponse type.
    !> @return fah_error status (FAH_OK if valid).
    MODULE FUNCTION fah08_verify_c_abi_sizes(req_bytes, res_bytes) &
        RESULT(err)
      INTEGER(INT64), INTENT(OUT) :: req_bytes, res_bytes
      TYPE(fah_error) :: err
    END FUNCTION fah08_verify_c_abi_sizes
  END INTERFACE

END MODULE fah_f2008_mod
