cmake_minimum_required(VERSION 3.18...3.31)

if(CMAKE_HOST_APPLE
   AND (NOT CMAKE_SYSTEM_NAME OR CMAKE_SYSTEM_NAME STREQUAL "Darwin")
   AND NOT CMAKE_TOOLCHAIN_FILE)
  if(NOT CMAKE_OSX_SYSROOT)
    execute_process(
      COMMAND xcrun --show-sdk-path
      OUTPUT_VARIABLE _FAH_MACOS_SDK
      OUTPUT_STRIP_TRAILING_WHITESPACE
      ERROR_QUIET
    )
    if(_FAH_MACOS_SDK AND EXISTS "${_FAH_MACOS_SDK}")
      set(CMAKE_OSX_SYSROOT "${_FAH_MACOS_SDK}" CACHE PATH "macOS SDK sysroot")
    endif()
  endif()
endif()

project(fortran-abstract-http VERSION 0.1.0 LANGUAGES C Fortran)

include(CTest)
include(GNUInstallDirs)
include(FortranCInterface)
include(CheckFortranCompilerFlag)
include(CheckFortranSourceCompiles)

option(FAH_BUILD_TESTING "Build fortran-abstract-http test suite" ${BUILD_TESTING})
option(FAH_ENABLE_COVERAGE "Enable gcov/lcov line, function, and branch coverage" OFF)
option(FAH_USE_ASAN "Enable AddressSanitizer and UndefinedBehaviorSanitizer" OFF)
option(FAH_LEGACY_INT_CHARLEN "Use 32-bit int hidden character lengths (f2c / GCC <= 7 ABI)" OFF)

set(C_ABSTRACT_HTTP_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../c-abstract-http"
    CACHE PATH "Path to c-abstract-http source repository")

# Forward transport & feature defaults cleanly into c-abstract-http without building its internal tests
set(C_ABSTRACT_HTTP_BUILD_TESTING OFF CACHE BOOL "Disable upstream c-abstract-http tests" FORCE)
option(C_ABSTRACT_HTTP_ENABLE_WEBSOCKETS "Enable RFC 6455 WebSockets" ON)
option(C_ABSTRACT_HTTP_ENABLE_SSE "Enable W3C Server-Sent Events" ON)

if(NOT TARGET c-abstract-http)
  if(EXISTS "${C_ABSTRACT_HTTP_SOURCE_DIR}/CMakeLists.txt")
    add_subdirectory("${C_ABSTRACT_HTTP_SOURCE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}/c-abstract-http" EXCLUDE_FROM_ALL)
  else()
    include(FetchContent)
    FetchContent_Declare(
      c_abstract_http
      GIT_REPOSITORY https://github.com/SamuelMarks/c-abstract-http.git
      GIT_TAG master
      GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(c_abstract_http)
  endif()
endif()

# Generate compiler-detected Fortran <-> C name-mangling header
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/include")
FortranCInterface_HEADER(
  "${CMAKE_CURRENT_BINARY_DIR}/include/fah_fc_mangle.h"
  MACRO_NAMESPACE "FAH_FC_"
  SYMBOL_NAMESPACE "fah_fc_"
)

# Strict C89 and Fortran compiler flag profiles
set(FAH_C89_WARNING_FLAGS "")
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
  list(APPEND FAH_C89_WARNING_FLAGS
    -std=c89
    -pedantic
    -Wall
    -Wextra
    -Werror
    -Wstrict-prototypes
    -Wmissing-prototypes
    -Wold-style-definition
    -Wdeclaration-after-statement
    -Wconversion
    -Wsign-conversion
    -Wshadow
    -Wcast-qual
    -Wformat=2
    -Wundef
  )
elseif(MSVC)
  list(APPEND FAH_C89_WARNING_FLAGS /W4 /WX)
endif()

set(FAH_FORTRAN_BASE_WARNINGS "")
set(FAH_FORTRAN_MODERN_WARNINGS "")
if(CMAKE_Fortran_COMPILER_ID MATCHES "GNU")
  list(APPEND FAH_FORTRAN_BASE_WARNINGS
    -Wall
    -Wextra
    -Werror
    -Wno-unused-dummy-argument
    -Wno-unused-parameter
    -Wconversion
    -Wunderflow
    -Wcharacter-truncation
    -Wline-truncation
  )
  list(APPEND FAH_FORTRAN_MODERN_WARNINGS
    -fimplicit-none
    -Wimplicit-interface
    -Wimplicit-procedure
    -Wsurprising
  )
endif()

check_fortran_compiler_flag("-std=legacy" FAH_HAVE_FSTD_LEGACY)
check_fortran_compiler_flag("-std=f95" FAH_HAVE_FSTD_F95)
check_fortran_compiler_flag("-std=f2003" FAH_HAVE_FSTD_F2003)
check_fortran_compiler_flag("-std=f2008" FAH_HAVE_FSTD_F2008)
check_fortran_compiler_flag("-std=f2018" FAH_HAVE_FSTD_F2018)
check_fortran_compiler_flag("-std=f2023" FAH_HAVE_FSTD_F2023)
check_fortran_source_compiles(
  "program p\n  use, intrinsic :: iso_c_binding, only: f_c_string\n  character(len=:), allocatable :: s\n  s = f_c_string('a')\nend program p"
  FAH_HAVE_INTRINSIC_F_C_STRING
  SRC_EXT f90
)

set(FAH_COVERAGE_C_FLAGS "")
set(FAH_COVERAGE_F_FLAGS "")
set(FAH_COVERAGE_LINK_FLAGS "")
if(FAH_ENABLE_COVERAGE)
  set(FAH_COVERAGE_C_FLAGS -O0 -g --coverage)
  set(FAH_COVERAGE_F_FLAGS -O0 -g -fprofile-arcs -ftest-coverage)
  set(FAH_COVERAGE_LINK_FLAGS --coverage)
endif()

if(FAH_USE_ASAN)
  list(APPEND FAH_COVERAGE_C_FLAGS -fsanitize=address,undefined -fno-omit-frame-pointer)
  list(APPEND FAH_COVERAGE_F_FLAGS -fsanitize=address,undefined -fno-omit-frame-pointer)
  list(APPEND FAH_COVERAGE_LINK_FLAGS -fsanitize=address,undefined)
endif()

set(CMAKE_Fortran_MODULE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/mod")

add_subdirectory(src)

if(FAH_BUILD_TESTING)
  add_subdirectory(tests)
endif()
