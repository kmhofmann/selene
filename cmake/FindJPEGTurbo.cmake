# Try to find libjpeg. Once done this will define:
#  JPEG_FOUND - System has libjpeg
#  JPEG_INCLUDE_DIRS - The libjpeg include directories
#  JPEG_LIBRARIES - The libraries needed to use libjpeg
#  JPEG_DEFINITIONS - Compiler switches required for using libjpeg

find_package(PkgConfig)
pkg_check_modules(PKG_JPEG QUIET libturbojpeg)
set(JPEGTurbo_DEFINITIONS ${PKG_JPEGTurbo_CFLAGS_OTHER})

if (APPLE)
    if (DEFINED ENV{HOMEBREW_PREFIX})
        set(HOMEBREW_PREFIX $ENV{HOMEBREW_PREFIX})
    else()
        set(HOMEBREW_PREFIX "/usr/local")
    endif()
endif()

find_path(JPEGTurbo_INCLUDE_DIR
        turbojpeg.h
        HINTS
        ${PKG_JPEGTurbo_INCLUDEDIR}
        ${PKG_JPEGTurbo_INCLUDE_DIRS}
        ${HOMEBREW_PREFIX}/opt/jpeg-turbo/include
        PATH_SUFFIXES
        libjpeg)

find_library(JPEGTurbo_LIBRARY
        NAMES
        turbojpeg
        HINTS
        ${PKG_JPEGTurbo_LIBDIR}
        ${PKG_JPEGTurbo_LIBRARY_DIRS}
        ${HOMEBREW_PREFIX}/opt/jpeg-turbo/lib)

# Check if the present version of libjpeg-turbo supports partial image decoding
set(JPEGTurbo_SUPPORTS_PARTIAL OFF)
if (JPEGTurbo_INCLUDE_DIR AND EXISTS "${JPEGTurbo_INCLUDE_DIR}/jpeglib.h")
    file(STRINGS "${JPEGTurbo_INCLUDE_DIR}/jpeglib.h" jpegturbo_supports_skip_scanlines REGEX "jpeg_skip_scanlines")
    if (jpegturbo_supports_skip_scanlines)
        set(JPEGTurbo_SUPPORTS_PARTIAL ON)
    endif()
    unset(jpegturbo_supports_skip_scanlines)
endif()

# Handle the QUIET and REQUIRED arguments and set JPEGTurbo_FOUND to TRUE if all listed variables are TRUE
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(JPEGTurbo DEFAULT_MSG JPEGTurbo_LIBRARY JPEGTurbo_INCLUDE_DIR)

mark_as_advanced(JPEGTurbo_INCLUDE_DIR JPEGTurbo_LIBRARY)

set(JPEGTurbo_LIBRARIES ${JPEGTurbo_LIBRARY})
set(JPEGTurbo_INCLUDE_DIRS ${JPEGTurbo_INCLUDE_DIR})
