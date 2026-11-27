!> @file fah_f2023_fcstr_mod.f90
!> @module fah_f2023_fcstr_mod
!> @brief Re-exports Fortran 2023 intrinsic ISO_C_BINDING :: F_C_STRING.
MODULE fah_f2023_fcstr_mod
  USE, INTRINSIC :: ISO_C_BINDING, ONLY: F_C_STRING
  IMPLICIT NONE
  PRIVATE
  PUBLIC :: F_C_STRING
END MODULE fah_f2023_fcstr_mod
