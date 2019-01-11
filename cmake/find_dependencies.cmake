# Centralized dependency handling

# libjpeg-turbo (or libjpeg)

if (NOT SELENE_NO_LIBJPEG)
    find_package(JPEG)
endif()

if (JPEG_FOUND)
    message(STATUS "Building with libjpeg support.")
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
        message(STATUS "libjpeg-turbo supports partial decoding.")
    endif()
    if (JPEG_EXTENDED_COLORSPACES)
        message(STATUS "libjpeg-turbo supports extended color spaces.")
    endif()
else()
    message(STATUS "Building without libjpeg support.")
endif()

# libpng

if (NOT SELENE_NO_LIBPNG)
    find_package(PNG)
endif()

if (PNG_FOUND)
    message(STATUS "Building with libpng support.")
else()
    message(STATUS "Building without libpng support.")
endif()

# libtiff

if (NOT SELENE_NO_LIBTIFF)
    find_package(TIFF)
endif()

if (TIFF_FOUND)
    message(STATUS "Building with libtiff support.")
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
        message(STATUS "libtiff supports Zstd and WebP (de)compression, with respective libraries present.")
    endif()
else()
    message(STATUS "Building without libtiff support.")
endif()

# OpenCV

if (NOT SELENE_NO_OPENCV)
    find_package(OpenCV)
endif()

if (OPENCV_CORE_FOUND)
    message(STATUS "Building with OpenCV support.")
else()
    message(STATUS "Building without OpenCV support.")
endif()

