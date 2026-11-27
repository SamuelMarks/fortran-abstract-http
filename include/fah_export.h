/**
 * @file fah_export.h
 * @brief Symbol visibility, nodiscard attributes, and Fortran ABI mangling
 * macros for fortran-abstract-http.
 */

#ifndef FAH_EXPORT_H
#define FAH_EXPORT_H

/* clang-format off */
#include <stddef.h>
#if defined(FAH_HAVE_CMAKE_MANGLE) || defined(fah_c_bridge_EXPORTS)
#include "fah_fc_mangle.h"
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifndef FAH_API
#if defined(_WIN32) || defined(__CYGWIN__)
#if defined(FAH_BUILD_SHARED)
#if defined(fah_c_bridge_EXPORTS)
/** @brief Public shared-library export attribute */
#define FAH_API __declspec(dllexport)
#else
/** @brief Public shared-library import attribute */
#define FAH_API __declspec(dllimport)
#endif
#else
/** @brief Static-library symbol visibility macro */
#define FAH_API
#endif
#elif defined(__GNUC__) && (__GNUC__ >= 4)
/** @brief ELF/Mach-O default symbol visibility attribute */
#define FAH_API __attribute__((visibility("default")))
#else
/** @brief Fallback public symbol visibility macro */
#define FAH_API
#endif
#endif /* !FAH_API */

#ifndef FAH_NO_DISCARD
#if defined(__cplusplus) && (__cplusplus >= 201703L)
/** @brief Warn when function return value is discarded */
#define FAH_NO_DISCARD [[nodiscard]]
#elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 202311L)
/** @brief Warn when function return value is discarded */
#define FAH_NO_DISCARD [[nodiscard]]
#elif defined(__GNUC__) || defined(__clang__)
/** @brief Warn when function return value is discarded */
#define FAH_NO_DISCARD __attribute__((warn_unused_result))
#elif defined(_MSC_VER) && (_MSC_VER >= 1700)
/** @brief Warn when function return value is discarded */
#define FAH_NO_DISCARD _Check_return_
#else
/** @brief Fallback nodiscard attribute */
#define FAH_NO_DISCARD
#endif
#endif /* !FAH_NO_DISCARD */

#ifndef FAH_FC_GLOBAL
/**
 * @brief Maps an underscore-free Fortran external symbol `(low, UP)` to the
 * active compiler's C linker name (defaults to trailing underscore `low##_`).
 * @param low Lowercase identifier token.
 * @param UP Uppercase identifier token.
 */
#define FAH_FC_GLOBAL(low, UP) low##_
#endif

#ifndef FAH_FC_GLOBAL_
/**
 * @brief Maps an underscore-containing Fortran external symbol `(low, UP)` to
 * the active compiler's C linker name.
 * @param low Lowercase identifier token.
 * @param UP Uppercase identifier token.
 */
#define FAH_FC_GLOBAL_(low, UP) low##_
#endif

/**
 * @brief Hidden trailing character-length integer type passed by Fortran
 * 77/90/95 compilers when calling external procedures with `CHARACTER*(*)`
 * arguments.
 *
 * Uses `size_t` for modern `gfortran >= 8` and `flang`, or 32-bit `int` when
 * `FAH_LEGACY_INT_CHARLEN` is defined (`f2c` / historical Unix `f77`).
 */
#if defined(FAH_LEGACY_INT_CHARLEN)
typedef int fah_charlen_t;
#else
typedef size_t fah_charlen_t;
#endif

/**
 * @brief Explicitly consume a `warn_unused_result` return code during
 * best-effort error-unwind cleanup without introducing extra branch-coverage
 * edges.
 * @param expr Fallible cleanup expression whose integer status is acknowledged.
 */
#define FAH_IGNORE_RC(expr)                                                    \
  do {                                                                         \
    int fah_ign_rc_ = (int)(expr);                                             \
    (void)fah_ign_rc_;                                                         \
  } while (0)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* FAH_EXPORT_H */
