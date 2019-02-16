# Centralized dependency handling
# (for the main library)

# Define helper macro that is called below.

macro(find_package_if PKG_NAME CONDITION NAME)
    set(selene_fp_failure_msg " (requested by user)")

    if(${CONDITION})
        find_package(${PKG_NAME} QUIET)
        set(selene_fp_failure_msg " (*** NOT FOUND ***)")
    endif()

    if (NOT ${PKG_NAME}_FOUND)
        message(STATUS "Building WITHOUT ${NAME} support${selene_fp_failure_msg}.")
    endif()
endmacro()

# libjpeg-turbo (or libjpeg)

find_package_if(JPEG SELENE_USE_LIBJPEG "libjpeg")

if(JPEG_FOUND)
    message(STATUS "Building with libjpeg support (${JPEG_LIBRARY}; version ${JPEG_VERSION}).")
    set(SELENE_WITH_LIBJPEG ON)

    # Detect some features provided by libjpeg-turbo: partial image decoding and extended colorspaces
    if(JPEG_INCLUDE_DIR AND EXISTS "${JPEG_INCLUDE_DIR}/jpeglib.h")
        file(STRINGS "${JPEG_INCLUDE_DIR}/jpeglib.h" jpeg_supports_skip_scanlines REGEX "jpeg_skip_scanlines")
        file(STRINGS "${JPEG_INCLUDE_DIR}/jpeglib.h" jpeg_supports_crop_scanline REGEX "jpeg_crop_scanline")
        file(STRINGS "${JPEG_INCLUDE_DIR}/jpeglib.h" jpeg_supports_jcs_ext REGEX "JCS_EXT_")
        if(jpeg_supports_skip_scanlines AND jpeg_supports_crop_scanline)
            set(SELENE_LIBJPEG_PARTIAL_DECODING ON)
        endif()
        if(jpeg_supports_jcs_ext)
            set(SELENE_LIBJPEG_EXTENDED_COLORSPACES ON)
        endif()
        unset(jpeg_supports_skip_scanlines)
        unset(jpeg_supports_crop_scanline)
        unset(jpeg_supports_jcs_ext)
    endif()

    if(SELENE_LIBJPEG_PARTIAL_DECODING)
        message(STATUS "  libjpeg-turbo supports partial decoding.")
    endif()

    if(SELENE_LIBJPEG_EXTENDED_COLORSPACES)
        message(STATUS "  libjpeg-turbo supports extended color spaces.")
    endif()
endif()

# libpng

find_package_if(PNG SELENE_USE_LIBPNG "libpng")

if(PNG_FOUND)
    message(STATUS "Building with libpng support (${PNG_LIBRARY}; version ${PNG_VERSION_STRING}).")
    set(SELENE_WITH_LIBPNG ON)
endif()

# libtiff

find_package_if(TIFF SELENE_USE_LIBTIFF "libtiff")

if(TIFF_FOUND)
    message(STATUS "Building with libtiff support (${TIFF_LIBRARY}; version ${TIFF_VERSION_STRING}).")
    set(SELENE_WITH_LIBTIFF ON)

    # Detect some features provided only by recent libtiff versions (v4.0.10 and up)
    if(TIFF_INCLUDE_DIR AND EXISTS "${TIFF_INCLUDE_DIR}/tiff.h")
        file(STRINGS "${TIFF_INCLUDE_DIR}/tiff.h" tiff_supports_zstd_compression REGEX "COMPRESSION_ZSTD")
        file(STRINGS "${TIFF_INCLUDE_DIR}/tiff.h" tiff_supports_webp_compression REGEX "COMPRESSION_WEBP")
        if(tiff_supports_zstd_compression AND tiff_supports_webp_compression)
            set(SELENE_LIBTIFF_ZSTD_WEBP_SUPPORT ON)
        endif()
        unset(tiff_supports_zstd_compression)
        unset(tiff_supports_webp_compression)
    endif()

    if(SELENE_LIBTIFF_ZSTD_WEBP_SUPPORT)
        message(STATUS "  libtiff supports Zstd and WebP (de)compression, if respective libraries are present.")
    endif()
endif()

# OpenCV

find_package_if(OpenCV SELENE_USE_OPENCV "OpenCV")

if(OPENCV_CORE_FOUND)
    message(STATUS "Building with OpenCV support (${OpenCV_INSTALL_PATH}; version ${OpenCV_VERSION}).")
    set(SELENE_WITH_OPENCV ON)
endif()
