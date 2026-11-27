!> @file fortran_abstract_http.f90
!> @module fortran_abstract_http
!> @brief Top-level umbrella SDK module re-exporting Fortran 2003 OOP,
!> Fortran 2008 CONTIGUOUS/SUBMODULE views, Fortran 2018 CFI_cdesc_t
!> descriptors, and Fortran 2023 F_C_STRING / C_F_STRPOINTER APIs.
MODULE fortran_abstract_http
  USE fah_f2003_mod
  USE fah_f2008_mod
  USE fah_f2018_mod
  USE fah_f2023_mod
  IMPLICIT NONE
  PUBLIC

  !> @brief Major semantic version of fortran-abstract-http.
  INTEGER, PARAMETER :: FAH_SDK_VERSION_MAJOR = 0
  !> @brief Minor semantic version of fortran-abstract-http.
  INTEGER, PARAMETER :: FAH_SDK_VERSION_MINOR = 1
  !> @brief Patch semantic version of fortran-abstract-http.
  INTEGER, PARAMETER :: FAH_SDK_VERSION_PATCH = 0

CONTAINS

  !> @brief Return the semantic version string of fortran-abstract-http.
  !> @return Version string literal '0.1.0'.
  PURE FUNCTION fah_sdk_version() RESULT(ver)
    CHARACTER(LEN=5) :: ver
    ver = '0.1.0'
  END FUNCTION fah_sdk_version

END MODULE fortran_abstract_http
