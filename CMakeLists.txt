cmake_minimum_required(VERSION 3.8)
project(selene CXX)

option(SELENE_ENABLE_TESTS "Build selene tests" ON)
option(SELENE_USE_LIBCXX "Use libc++" OFF)
option(SELENE_USE_CXXABI "Use libc++abi" OFF)

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

get_directory_property(has_parent PARENT_DIRECTORY)

#------------------------------------------------------------------------------

# Set compiler flags (GCC or Clang)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Wno-attributes -Wno-clobbered")
elseif ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

    if (SELENE_USE_LIBCXX)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++")
    endif()

    if (SELENE_USE_CXXABI)
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lc++abi")
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
