set(FAH_C_SOURCES
  "${CMAKE_SOURCE_DIR}/generated/c_abstract_http_fortran_shim.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/ffi/fah_alloc.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/ffi/fah_error.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/ffi/fah_handle_registry.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/ffi/fah_hollerith.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/ffi/fah_f77_strings.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/ffi/fah_cfi_c89.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/ffi/fah_callbacks.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/ffi/fah_c_bridge.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/ffi/fah_fortran_entries.c"
)

add_library(fah_c_bridge STATIC ${FAH_C_SOURCES})
target_include_directories(fah_c_bridge
  PUBLIC
    "${CMAKE_SOURCE_DIR}/include"
    "${CMAKE_SOURCE_DIR}/generated"
    "${CMAKE_BINARY_DIR}/include"
)
target_compile_options(fah_c_bridge PRIVATE ${FAH_C89_WARNING_FLAGS} ${FAH_COVERAGE_C_FLAGS})
target_link_options(fah_c_bridge PUBLIC ${FAH_COVERAGE_LINK_FLAGS})
target_link_libraries(fah_c_bridge PUBLIC c-abstract-http)
target_compile_definitions(fah_c_bridge PUBLIC FAH_HAVE_CMAKE_MANGLE=1)
if(FAH_BUILD_TESTING)
  target_compile_definitions(fah_c_bridge PUBLIC FAH_TEST_OOM=1)
endif()
if(C_ABSTRACT_HTTP_ENABLE_WEBSOCKETS)
  target_compile_definitions(fah_c_bridge PUBLIC C_ABSTRACT_HTTP_ENABLE_WEBSOCKETS=1)
endif()
if(C_ABSTRACT_HTTP_ENABLE_SSE)
  target_compile_definitions(fah_c_bridge PUBLIC C_ABSTRACT_HTTP_ENABLE_SSE=1)
endif()
if(FAH_LEGACY_INT_CHARLEN)
  target_compile_definitions(fah_c_bridge PUBLIC FAH_LEGACY_INT_CHARLEN=1)
endif()

# Helper function to create strict per-standard Fortran dialect libraries
function(fah_add_fortran_layer TARGET_NAME STD_FLAG FORM_FLAGS)
  add_library(${TARGET_NAME} STATIC ${ARGN})
  target_include_directories(${TARGET_NAME}
    PUBLIC
      "${CMAKE_SOURCE_DIR}/include"
      "${CMAKE_SOURCE_DIR}/generated"
      "${CMAKE_Fortran_MODULE_DIRECTORY}"
  )
  target_compile_options(${TARGET_NAME}
    PRIVATE
      ${FAH_FORTRAN_BASE_WARNINGS}
      ${FORM_FLAGS}
      ${STD_FLAG}
      ${FAH_COVERAGE_F_FLAGS}
  )
  target_link_options(${TARGET_NAME} PUBLIC ${FAH_COVERAGE_LINK_FLAGS})
  target_link_libraries(${TARGET_NAME} PUBLIC fah_c_bridge)
endfunction()

set(_FAH_FIXED_FLAGS -ffixed-form -ffixed-line-length-72)
set(_FAH_FREE_FLAGS -ffree-form -ffree-line-length-132 -pedantic ${FAH_FORTRAN_MODERN_WARNINGS})

set(_STD_LEG "")
if(FAH_HAVE_FSTD_LEGACY)
  set(_STD_LEG -std=legacy)
endif()
set(_STD_95 "")
if(FAH_HAVE_FSTD_F95)
  set(_STD_95 -std=f95)
endif()
set(_STD_03 "")
if(FAH_HAVE_FSTD_F2003)
  set(_STD_03 -std=f2003)
endif()
set(_STD_08 "")
if(FAH_HAVE_FSTD_F2008)
  set(_STD_08 -std=f2008)
endif()
set(_STD_18 "")
if(FAH_HAVE_FSTD_F2018)
  set(_STD_18 -std=f2018)
endif()
set(_STD_23 "")
if(FAH_HAVE_FSTD_F2023)
  set(_STD_23 -std=f2023)
elseif(FAH_HAVE_FSTD_F2018)
  set(_STD_23 -std=f2018)
endif()

# Tier A & B: Fixed-form historical & classical FORTRAN (1957 - 1978)
fah_add_fortran_layer(fah_f57 "${_STD_LEG}" "${_FAH_FIXED_FLAGS}" legacy/fah_f57.f)
fah_add_fortran_layer(fah_f58 "${_STD_LEG}" "${_FAH_FIXED_FLAGS}" legacy/fah_f58.f)
fah_add_fortran_layer(fah_f60 "${_STD_LEG}" "${_FAH_FIXED_FLAGS}" legacy/fah_f60.f)
fah_add_fortran_layer(fah_f62 "${_STD_LEG}" "${_FAH_FIXED_FLAGS}" legacy/fah_f62.f)
fah_add_fortran_layer(fah_f66 "${_STD_LEG}" "${_FAH_FIXED_FLAGS}" legacy/fah_f66.f)
fah_add_fortran_layer(fah_f77 "${_STD_LEG}" "${_FAH_FIXED_FLAGS}" legacy/fah_f77.f legacy/fah_f77_strict.f)

# Tier C: Modular free-form pre-ISO_C_BINDING (Fortran 90 & Fortran 95)
fah_add_fortran_layer(fah_f90 "${_STD_95}" "${_FAH_FREE_FLAGS}" f90/fah_f90_mod.f90)
fah_add_fortran_layer(fah_f95 "${_STD_95}" "${_FAH_FREE_FLAGS}" f95/fah_f95_mod.f90)
target_link_libraries(fah_f95 PUBLIC fah_f90)

# Tier D: ISO_C_BINDING, OOP, SUBMODULE, CFI_cdesc_t & F_C_STRING (Fortran 2003 - 2023)
add_library(fah_bind_raw STATIC "${CMAKE_SOURCE_DIR}/generated/c_abstract_http_bind_mod.f90")
target_include_directories(fah_bind_raw
  PUBLIC
    "${CMAKE_SOURCE_DIR}/include"
    "${CMAKE_SOURCE_DIR}/generated"
    "${CMAKE_Fortran_MODULE_DIRECTORY}"
)
target_compile_options(fah_bind_raw
  PRIVATE
    ${FAH_FORTRAN_BASE_WARNINGS}
    ${_FAH_FREE_FLAGS}
    ${_STD_03}
)
target_link_libraries(fah_bind_raw PUBLIC fah_c_bridge)
fah_add_fortran_layer(fah_f2003 "${_STD_03}" "${_FAH_FREE_FLAGS}" f2003/fah_f2003_mod.f90)
target_link_libraries(fah_f2003 PUBLIC fah_bind_raw)

fah_add_fortran_layer(fah_f2008 "${_STD_08}" "${_FAH_FREE_FLAGS}"
  f2008/fah_f2008_mod.f90
  f2008/fah_f2008_impl_smod.f90
)
target_link_libraries(fah_f2008 PUBLIC fah_f2003)

fah_add_fortran_layer(fah_f2018 "${_STD_18}" "${_FAH_FREE_FLAGS}" f2018/fah_f2018_mod.f90)
target_link_libraries(fah_f2018 PUBLIC fah_f2008)

if(FAH_HAVE_INTRINSIC_F_C_STRING)
  set(_FAH_FCSTR_SRC f2023/fah_f2023_fcstr_mod.f90)
else()
  set(_FAH_FCSTR_SRC "${CMAKE_SOURCE_DIR}/cmake/fah_f2023_fcstr_polyfill.f90")
endif()

fah_add_fortran_layer(fah_f2023 "${_STD_23}" "${_FAH_FREE_FLAGS}"
  "${_FAH_FCSTR_SRC}"
  f2023/fah_f2023_mod.f90
  fortran_abstract_http.f90
)
target_link_libraries(fah_f2023 PUBLIC fah_f2018)

# Aggregate library target exporting all 12 dialect layers
add_library(fortran-abstract-http INTERFACE)
target_link_libraries(fortran-abstract-http INTERFACE
  fah_c_bridge
  fah_f57
  fah_f58
  fah_f60
  fah_f62
  fah_f66
  fah_f77
  fah_f90
  fah_f95
  fah_bind_raw
  fah_f2003
  fah_f2008
  fah_f2018
  fah_f2023
)
add_library(fortran-abstract-http::fortran-abstract-http ALIAS fortran-abstract-http)
