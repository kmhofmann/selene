# Centralized dependency handling

set(selene_fp_failure_msg_user " (requested by user)")
set(selene_fp_failure_msg_nf " (*** NOT FOUND ***)")

# libjpeg-turbo (or libjpeg)

set(selene_fp_failure_msg ${selene_fp_failure_msg_user})
if (SELENE_WITH_LIBJPEG)
    find_package(JPEG QUIET)
    set(selene_fp_failure_msg ${selene_fp_failure_msg_nf})
endif()

if (JPEG_FOUND)
    message(STATUS "Building with libjpeg support (${JPEG_LIBRARY}; version ${JPEG_VERSION}).")
    # Detect some features provided by libjpeg-turbo: partial image decoding and extended colorspaces
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
    if (JPEG_PARTIAL_DECODING)
        message(STATUS "  libjpeg-turbo supports partial decoding.")
    endif()
    if (JPEG_EXTENDED_COLORSPACES)
        message(STATUS "  libjpeg-turbo supports extended color spaces.")
    endif()
else()
    message(STATUS "Building WITHOUT libjpeg support${selene_fp_failure_msg}.")
endif()

# libpng

set(selene_fp_failure_msg ${selene_fp_failure_msg_user})
if (SELENE_WITH_LIBPNG)
    find_package(PNG QUIET)
    set(selene_fp_failure_msg ${selene_fp_failure_msg_nf})
endif()

if (PNG_FOUND)
    message(STATUS "Building with libpng support (${PNG_LIBRARY}; version ${PNG_VERSION_STRING}).")
else()
    message(STATUS "Building WITHOUT libpng support${selene_fp_failure_msg}.")
endif()

# libtiff

set(selene_fp_failure_msg ${selene_fp_failure_msg_user})
if (SELENE_WITH_LIBTIFF)
    find_package(TIFF QUIET)
    set(selene_fp_failure_msg ${selene_fp_failure_msg_nf})
endif()

if (TIFF_FOUND)
    message(STATUS "Building with libtiff support (${TIFF_LIBRARY}; version ${TIFF_VERSION_STRING}).")

    # Detect some features provided only by recent libtiff versions (v4.0.10 and up)
    set(TIFF_SUPPORT_ZSTD_AND_WEBP OFF)
    if (TIFF_INCLUDE_DIR AND EXISTS "${TIFF_INCLUDE_DIR}/tiff.h")
        file(STRINGS "${TIFF_INCLUDE_DIR}/tiff.h" tiff_supports_zstd_compression REGEX "COMPRESSION_ZSTD")
        file(STRINGS "${TIFF_INCLUDE_DIR}/tiff.h" tiff_supports_webp_compression REGEX "COMPRESSION_WEBP")
        if (tiff_supports_zstd_compression AND tiff_supports_webp_compression)
            set(TIFF_SUPPORT_ZSTD_AND_WEBP ON)
        endif()
        unset(tiff_supports_zstd_compression)
        unset(tiff_supports_webp_compression)
    endif()
    if (TIFF_SUPPORT_ZSTD_AND_WEBP)
        message(STATUS "  libtiff supports Zstd and WebP (de)compression, if respective libraries are present.")
    endif()
else()
    message(STATUS "Building WITHOUT libtiff support${selene_fp_failure_msg}.")
endif()

# OpenCV

set(selene_fp_failure_msg ${selene_fp_failure_msg_user})
if (SELENE_WITH_OPENCV)
    find_package(OpenCV QUIET)
    set(selene_fp_failure_msg ${selene_fp_failure_msg_nf})
endif()

if (OPENCV_CORE_FOUND)
    message(STATUS "Building with OpenCV support (${OpenCV_INSTALL_PATH}; version ${OpenCV_VERSION}).")
else()
    message(STATUS "Building WITHOUT OpenCV support${selene_fp_failure_msg}.")
endif()

