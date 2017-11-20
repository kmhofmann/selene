cmake_minimum_required(VERSION 3.8)
project(selene CXX)

option(SELENE_ENABLE_TESTS "Build selene tests" ON)
option(SELENE_SEARCH_LIBJPEG_TURBO "Enable searching for libjpeg-turbo" ON)

# https://cmake.org/cmake/help/latest/prop_tgt/CXX_STANDARD.html
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(external/external.cmake)

# Generate compilation database (e.g. for YouCompleteMe)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

set(SELENE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

get_directory_property(has_parent PARENT_DIRECTORY)

#------------------------------------------------------------------------------

# Set compiler flags (GCC or Clang)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-attributes")
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

find_package(Boost REQUIRED filesystem system)

if (SELENE_SEARCH_LIBJPEG_TURBO)
    find_package(JPEGTurbo)
    if (JPEGTurbo_FOUND)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DSELENE_WITH_LIBJPEG_TURBO")
        if (has_parent)
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DSELENE_WITH_LIBJPEG_TURBO" PARENT_SCOPE)
        endif()
        message("Building with libjpeg-turbo support.")
        if (JPEGTurbo_SUPPORTS_PARTIAL)
            message("This libjpeg-turbo version supports partial image decoding.")
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DSELENE_LIBJPEG_TURBO_PARTIAL_DECODING")
        else()
            message("This libjpeg-turbo version does not support partial image decoding.")
        endif()
    else()
        message(WARNING "Building without libjpeg-turbo support.")
    endif()
endif()

if (NOT JPEGTurbo_FOUND)
    find_package(JPEG)
    if (JPEG_FOUND)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DSELENE_WITH_LIBJPEG")
        if (has_parent)
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DSELENE_WITH_LIBJPEG" PARENT_SCOPE)
        endif()
        message("Building with libjpeg support.")
    else()
        message(WARNING "Building without libjpeg support.")
    endif()
endif()

find_package(PNG 1.6)
if (PNG_FOUND)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DSELENE_WITH_LIBPNG")
    if (has_parent)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DSELENE_WITH_LIBPNG" PARENT_SCOPE)
    endif()
    message("Building with libpng support.")
else()
    message(WARNING "Building without libpng support.")
endif()

#------------------------------------------------------------------------------

# Add the selene library target
add_subdirectory(src)

# Optionally add the tests target
if (SELENE_ENABLE_TESTS)
    add_subdirectory(test)
endif(SELENE_ENABLE_TESTS)
