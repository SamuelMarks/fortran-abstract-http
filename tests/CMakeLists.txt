# Loopback test server helper library (strict C89)
add_library(fah_loopback_server STATIC c/mock_loopback_server.c)
target_include_directories(fah_loopback_server
  PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/c"
    "${CMAKE_SOURCE_DIR}/include"
    "${CMAKE_BINARY_DIR}/include"
)
target_compile_definitions(fah_loopback_server PUBLIC FAH_HAVE_CMAKE_MANGLE=1)
if(APPLE)
  target_compile_definitions(fah_loopback_server PRIVATE _DARWIN_C_SOURCE)
elseif(UNIX)
  target_compile_definitions(fah_loopback_server PRIVATE _POSIX_C_SOURCE=200809L)
endif()
target_compile_options(fah_loopback_server PRIVATE ${FAH_C89_WARNING_FLAGS})
target_link_libraries(fah_loopback_server PUBLIC fah_c_bridge)

# 1. C89 Bridge, Handle Registry, Hollerith, F77 Strings, CFI & OOM Exhaustive Unit Suite
add_executable(test_fah_c_bridge c/test_fah_c_bridge.c)
target_include_directories(test_fah_c_bridge
  PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/c"
    "${CMAKE_SOURCE_DIR}/include"
    "${CMAKE_SOURCE_DIR}/generated"
    "${CMAKE_BINARY_DIR}/include"
)
target_compile_definitions(test_fah_c_bridge PRIVATE FAH_TEST_OOM=1 FAH_HAVE_CMAKE_MANGLE=1)
target_compile_options(test_fah_c_bridge PRIVATE ${FAH_C89_WARNING_FLAGS})
target_link_directories(test_fah_c_bridge PRIVATE ${CMAKE_Fortran_IMPLICIT_LINK_DIRECTORIES})
target_link_libraries(test_fah_c_bridge PRIVATE
  fah_c_bridge
  fah_loopback_server
  ${CMAKE_Fortran_IMPLICIT_LINK_LIBRARIES}
)
add_test(NAME test_fah_c_bridge COMMAND test_fah_c_bridge)

# 2. ABI Struct Layout & ISO_Fortran_binding.h Verification Suite
execute_process(
  COMMAND "${CMAKE_Fortran_COMPILER}" -print-file-name=include
  OUTPUT_VARIABLE _FAH_FC_INC_DIR
  OUTPUT_STRIP_TRAILING_WHITESPACE
  ERROR_QUIET
)
add_executable(test_abi_layout c/test_abi_layout.c fortran/test_abi_layout_f2008.f90)
target_include_directories(test_abi_layout
  PRIVATE
    "${CMAKE_SOURCE_DIR}/include"
    "${CMAKE_SOURCE_DIR}/generated"
    "${CMAKE_BINARY_DIR}/include"
    "${CMAKE_Fortran_MODULE_DIRECTORY}"
)
if(_FAH_FC_INC_DIR AND EXISTS "${_FAH_FC_INC_DIR}/ISO_Fortran_binding.h")
  target_include_directories(test_abi_layout SYSTEM PRIVATE "${_FAH_FC_INC_DIR}")
endif()
# Note: test_abi_layout.c checks our C89 CFI_cdesc_t shim against compiler <ISO_Fortran_binding.h>
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
  set_source_files_properties(c/test_abi_layout.c PROPERTIES COMPILE_OPTIONS "-std=c99;-Wall;-Wextra;-Werror")
endif()
if(FAH_HAVE_FSTD_F2008)
  set_source_files_properties(fortran/test_abi_layout_f2008.f90 PROPERTIES COMPILE_OPTIONS "-std=f2008;-Wall;-Wextra;-Werror")
endif()
target_link_libraries(test_abi_layout PRIVATE fah_c_bridge fah_bind_raw fah_f2008)
add_test(NAME test_abi_layout COMMAND test_abi_layout)

# Helper to register strict per-standard Fortran integration test executables
function(fah_add_fortran_test TEST_NAME STD_FLAG FORM_FLAGS SRC_FILE)
  add_executable(${TEST_NAME} ${SRC_FILE})
  target_include_directories(${TEST_NAME}
    PRIVATE
      "${CMAKE_SOURCE_DIR}/include"
      "${CMAKE_SOURCE_DIR}/generated"
      "${CMAKE_Fortran_MODULE_DIRECTORY}"
  )
  target_compile_options(${TEST_NAME}
    PRIVATE
      ${FAH_FORTRAN_BASE_WARNINGS}
      ${FORM_FLAGS}
      ${STD_FLAG}
      ${FAH_COVERAGE_F_FLAGS}
  )
  target_link_options(${TEST_NAME} PRIVATE ${FAH_COVERAGE_LINK_FLAGS})
  target_link_libraries(${TEST_NAME} PRIVATE fortran-abstract-http fah_loopback_server ${ARGN})
  add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
endfunction()

set(_T_FIXED -ffixed-form -ffixed-line-length-72 -Wno-conversion)
set(_T_FREE -ffree-form -ffree-line-length-132 -pedantic ${FAH_FORTRAN_MODERN_WARNINGS})

set(_T_LEG "")
if(FAH_HAVE_FSTD_LEGACY)
  set(_T_LEG -std=legacy)
endif()
set(_T_95 "")
if(FAH_HAVE_FSTD_F95)
  set(_T_95 -std=f95)
endif()
set(_T_03 "")
if(FAH_HAVE_FSTD_F2003)
  set(_T_03 -std=f2003)
endif()
set(_T_08 "")
if(FAH_HAVE_FSTD_F2008)
  set(_T_08 -std=f2008)
endif()
set(_T_18 "")
if(FAH_HAVE_FSTD_F2018)
  set(_T_18 -std=f2018)
endif()
set(_T_23 "")
if(FAH_HAVE_FSTD_F2023)
  set(_T_23 -std=f2023)
elseif(FAH_HAVE_FSTD_F2018)
  set(_T_23 -std=f2018)
endif()

# 12 Individual Dialect Suites (FORTRAN I through Fortran 2023)
fah_add_fortran_test(test_f57_fortran_i   "${_T_LEG}" "${_T_FIXED}" fortran/test_f57_fortran_i.f)
fah_add_fortran_test(test_f58_fortran_ii  "${_T_LEG}" "${_T_FIXED}" fortran/test_f58_fortran_ii.f)
fah_add_fortran_test(test_f60_fortran_iii "${_T_LEG}" "${_T_FIXED}" fortran/test_f60_fortran_iii.f)
fah_add_fortran_test(test_f62_fortran_iv  "${_T_LEG}" "${_T_FIXED}" fortran/test_f62_fortran_iv.f)
fah_add_fortran_test(test_f66_ansi        "${_T_LEG}" "${_T_FIXED}" fortran/test_f66_ansi.f)
fah_add_fortran_test(test_f77_ansi        "${_T_LEG}" "${_T_FIXED}" fortran/test_f77_ansi.f)
fah_add_fortran_test(test_f90_mod         "${_T_95}"  "${_T_FREE}"  fortran/test_f90_mod.f90)
fah_add_fortran_test(test_f95_mod         "${_T_95}"  "${_T_FREE}"  fortran/test_f95_mod.f90)
fah_add_fortran_test(test_f2003_oop       "${_T_03}"  "${_T_FREE}"  fortran/test_f2003_oop.f90)
fah_add_fortran_test(test_f2008_submod    "${_T_08}"  "${_T_FREE}"  fortran/test_f2008_submod.f90)
fah_add_fortran_test(test_f2018_cfi       "${_T_18}"  "${_T_FREE}"  fortran/test_f2018_cfi.f90)
fah_add_fortran_test(test_f2023_mod       "${_T_23}"  "${_T_FREE}"  fortran/test_f2023_mod.f90)
