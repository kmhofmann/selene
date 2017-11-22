# Very similar to CMake's FindJPEG.cmake, but attempts to detect libjpeg-turbo extensions.
# Defines:
#   JPEG_FOUND: If false, do not try to use JPEG.
#
#   JPEG_INCLUDE_DIR: where to find jpeglib.h, etc.
#   JPEG_LIBRARIES: the libraries needed to use JPEG.
#
#   JPEG_PARTIAL_DECODING: whether partial decoding is supported
#   JPEG_EXTENDED_COLORSPACES: whether extended color spaces are supported

if (APPLE)
    if (DEFINED ENV{HOMEBREW_PREFIX})
        set(HOMEBREW_PREFIX $ENV{HOMEBREW_PREFIX})
    else()
        set(HOMEBREW_PREFIX "/usr/local")
    endif()
endif()

find_path(JPEG_INCLUDE_DIR jpeglib.h
          HINTS ${HOMEBREW_PREFIX}/opt/jpeg-turbo/include)

set(JPEG_NAMES ${JPEG_NAMES} jpeg libjpeg)
find_library(JPEG_LIBRARY NAMES ${JPEG_NAMES}
             HINTS ${HOMEBREW_PREFIX}/opt/jpeg-turbo/lib)

# handle the QUIETLY and REQUIRED arguments and set JPEG_FOUND to TRUE if
# all listed variables are TRUE
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(JPEG DEFAULT_MSG JPEG_LIBRARY JPEG_INCLUDE_DIR)

if(JPEG_FOUND)
    set(JPEG_LIBRARIES ${JPEG_LIBRARY})
endif()

mark_as_advanced(JPEG_LIBRARY JPEG_INCLUDE_DIR)

# Detect whether partial image decoding is supported.
# Also detect whether the extended colorspaces from libjpeg-turbo are supported.
set(JPEG_PARTIAL_DECODING OFF)
set(JPEG_EXTENDED_COLORSPACES OFF)
if (JPEG_INCLUDE_DIR AND EXISTS "${JPEG_INCLUDE_DIR}/jpeglib.h")
    file(STRINGS "${JPEG_INCLUDE_DIR}/jpeglib.h" jpeg_supports_skip_scanlines REGEX "jpeg_skip_scanlines")
    file(STRINGS "${JPEG_INCLUDE_DIR}/jpeglib.h" jpeg_supports_crop_scanline REGEX "jpeg_crop_scanline")
    file(STRINGS "${JPEG_INCLUDE_DIR}/jpeglib.h" jpeg_supports_jcs_ext REGEX "JCS_EXT_")
    if (jpeg_supports_skip_scanlines AND jpeg_supports_crop_scanline)
        set(JPEG_PARTIAL_DECODING ON)
    endif()
    if (jpeg_supports_jcs_ext)
        set(JPEG_EXTENDED_COLORSPACES ON)
    endif()
    unset(jpeg_supports_skip_scanlines)
    unset(jpeg_supports_crop_scanline)
    unset(jpeg_supports_jcs_ext)
endif()