!> @file fah_f2008_impl_smod.f90
!> @brief Fortran 2008 SUBMODULE implementation of fah_f2008_mod using
!> BLOCK constructs, C_SIZEOF, and CONTIGUOUS pointer association.
SUBMODULE (fah_f2008_mod) fah_f2008_impl_smod
  USE c_abstract_http_bind_mod
  IMPLICIT NONE

  !> @brief Empty static target backing zero-length CONTIGUOUS slices.
  INTEGER(INT8), TARGET, SAVE :: k_empty_bytes(1) = (/ 0_INT8 /)

CONTAINS

  !> @brief Associate a CONTIGUOUS INT8 pointer slice with res's body.
  !> @param[in] res Active fah_response instance.
  !> @param[out] view Contiguous byte pointer slice.
  !> @return fah_error status.
  MODULE PROCEDURE fah08_response_body_view
    BLOCK
      TYPE(C_PTR) :: raw_ptr
      INTEGER(C_SIZE_T) :: blen
      INTEGER(C_INT) :: rc

      rc = c_fah_response_body_ptr(res%handle_id(), raw_ptr, blen)
      IF (rc /= FAH_OK) THEN
        NULLIFY(view)
        err = fah03_from_code(rc, 'fah08_response_body_view')
      ELSE IF (blen == 0_C_SIZE_T .OR. .NOT. C_ASSOCIATED(raw_ptr)) THEN
        view => k_empty_bytes(1:0)
        err = fah03_from_code(FAH_OK, 'fah08_response_body_view')
      ELSE
        CALL C_F_POINTER(raw_ptr, view, [INT(blen, INT64)])
        err = fah03_from_code(FAH_OK, 'fah08_response_body_view')
      END IF
    END BLOCK
  END PROCEDURE fah08_response_body_view

  !> @brief Compute C_SIZEOF on generated BIND(C) request & response.
  !> @param[out] req_bytes Byte size of httprequest.
  !> @param[out] res_bytes Byte size of httpresponse.
  !> @return fah_error status.
  MODULE PROCEDURE fah08_verify_c_abi_sizes
    BLOCK
      TYPE(httprequest) :: sample_req
      TYPE(httpresponse) :: sample_res
      req_bytes = INT(C_SIZEOF(sample_req), INT64)
      res_bytes = INT(C_SIZEOF(sample_res), INT64)
      err = fah03_from_code(FAH_OK, 'fah08_verify_c_abi_sizes')
    END BLOCK
  END PROCEDURE fah08_verify_c_abi_sizes

END SUBMODULE fah_f2008_impl_smod
