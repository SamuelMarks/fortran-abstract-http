!> @file fah_f2023_fcstr_polyfill.f90
!> @module fah_f2023_fcstr_mod
!> @brief Standards-exact ISO/IEC 1539-1:2023 F_C_STRING fallback module
!> used when compiling with pre-GCC-15 gfortran toolchains.
MODULE fah_f2023_fcstr_mod
  USE, INTRINSIC :: ISO_C_BINDING, ONLY: C_CHAR, C_NULL_CHAR
  IMPLICIT NONE
  PRIVATE
  PUBLIC :: F_C_STRING

CONTAINS

  !> @brief Convert Fortran CHARACTER scalar into NUL-terminated C_CHAR.
  !> @param[in] fstr Input Fortran character string.
  !> @param[in] asis Optional flag preserving trailing blanks when .TRUE.
  !> @return Allocatable NUL-terminated C_CHAR string.
  PURE FUNCTION F_C_STRING(fstr, asis) RESULT(cstr)
    CHARACTER(LEN=*, KIND=C_CHAR), INTENT(IN) :: fstr
    LOGICAL, OPTIONAL, INTENT(IN) :: asis
    CHARACTER(LEN=:, KIND=C_CHAR), ALLOCATABLE :: cstr
    LOGICAL :: keep_blanks

    keep_blanks = .FALSE.
    IF (PRESENT(asis)) keep_blanks = asis
    IF (keep_blanks) THEN
      cstr = fstr // C_NULL_CHAR
    ELSE
      cstr = TRIM(fstr) // C_NULL_CHAR
    END IF
  END FUNCTION F_C_STRING

END MODULE fah_f2023_fcstr_mod
