# Centralized dependency handling

# Conan support

if (EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    set(SELENE_HAVE_CONAN TRUE)
    include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    conan_basic_setup(TARGETS KEEP_RPATHS)

    if (APPLE)
        set(CMAKE_INSTALL_RPATH "@executable_path/../lib")
    else()
        set(CMAKE_INSTALL_RPATH "$ORIGIN/../lib")
    endif()
endif()

# libjpeg-turbo (or libjpeg)

find_package(JPEG)

if (JPEG_FOUND)
    message("Building with libjpeg support.")
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
        message("libjpeg-turbo supports partial decoding.")
    endif()
    if (JPEG_EXTENDED_COLORSPACES)
        message("libjpeg-turbo supports extended color spaces.")
    endif()
else()
    message(WARNING "Building without libjpeg support.")
endif()

# libpng

find_package(PNG)

if (PNG_FOUND)
    message("Building with libpng support.")
else()
    message(WARNING "Building without libpng support.")
endif()

# OpenCV

find_package(OpenCV)

if (OPENCV_CORE_FOUND)
    message("Building with OpenCV support.")
else()
    message(WARNING "Building without OpenCV support.")
endif()

# Boost

if (SELENE_BUILD_TESTS OR SELENE_BUILD_EXAMPLES OR SELENE_BUILD_BENCHMARKS)
    if (SELENE_HAVE_CONAN)
        set(SELENE_BOOST_TARGET_NAME CONAN_PKG::boost_filesystem)  # HACK
    else()
        find_package(Boost REQUIRED filesystem system)
        set(SELENE_BOOST_TARGET_NAME Boost::filesystem)  # HACK
    endif()
endif()

# Google Benchmark

if (SELENE_BUILD_BENCHMARKS)
    find_package(benchmark REQUIRED)
endif()

# Threads

if (SELENE_BUILD_TESTS)
    find_package(Threads REQUIRED)
endif()