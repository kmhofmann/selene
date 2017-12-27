cmake_minimum_required(VERSION 3.8)
project(selene CXX)

option(SELENE_ENABLE_TESTS "Build selene tests" OFF)

# https://cmake.org/cmake/help/latest/prop_tgt/CXX_STANDARD.html
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(GNUInstallDirs)
include(external/external.cmake)

# Generate compilation database (e.g. for YouCompleteMe)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Ensure that we find the provided FindJPEGTurbo.cmake file
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

set(SELENE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
    set(SELENE_COMPILER_WARNING_OPTIONS -Wall -Wextra -Wpedantic)
    if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        set(SELENE_IMG_COMPILER_WARNING_OPTIONS -Wno-clobbered)
    endif()
endif()

#------------------------------------------------------------------------------

if (SELENE_ENABLE_TESTS)
    find_package(Boost REQUIRED filesystem system)
endif()

find_package(JPEGTurbo)  # Use the custom JPEGTurbo module
if (JPEG_FOUND)
    message("Building with libjpeg support.")
    if (JPEG_PARTIAL_DECODING)
        message("libjpeg-turbo supports partial decoding.")
    endif()
    if (JPEG_EXTENDED_COLORSPACES)
        message("libjpeg-turbo supports extended color spaces.")
    endif()
else()
    message(WARNING "Building without libjpeg support.")
endif()

find_package(PNG 1.6)
if (PNG_FOUND)
    message("Building with libpng support.")
else()
    message(WARNING "Building without libpng support.")
endif()

find_package(OpenCV)
if (OPENCV_CORE_FOUND)
    message("Building with OpenCV support.")
else()
    message(WARNING "Building without OpenCV support.")
endif()

#------------------------------------------------------------------------------

# Add the selene library target
add_subdirectory(src)

# Optionally add the tests target
if (SELENE_ENABLE_TESTS)
    add_subdirectory(test)
endif()
