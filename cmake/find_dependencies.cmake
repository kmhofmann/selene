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

# OpenCV

if (NOT SELENE_NO_OPENCV)
    find_package(OpenCV)
endif()

if (OPENCV_CORE_FOUND)
    message(STATUS "Building with OpenCV support.")
else()
    message(STATUS "Building without OpenCV support.")
endif()

# Boost

if (SELENE_BUILD_TESTS OR SELENE_BUILD_EXAMPLES OR SELENE_BUILD_BENCHMARKS)
    find_package(Boost REQUIRED filesystem system)
endif()

# Google Benchmark

if (SELENE_BUILD_BENCHMARKS)
    find_package(benchmark REQUIRED)
endif()

