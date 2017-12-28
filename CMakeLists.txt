cmake_minimum_required(VERSION 3.5)
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
	set(SELENE_COMPILER_EXTRA_DEFINITIONS "")
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
	set(SELENE_COMPILER_WARNING_OPTIONS /MP /permissive- /W4 /wd4611)
	set(SELENE_COMPILER_EXTRA_DEFINITIONS _CRT_SECURE_NO_WARNINGS)
endif()

# Add the selene library target
add_subdirectory(src)

# Optionally add the tests target
if (SELENE_ENABLE_TESTS)
    add_subdirectory(test)
endif()
