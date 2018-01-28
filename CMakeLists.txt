cmake_minimum_required(VERSION 3.5)
project(selene CXX)

option(SELENE_BUILD_TESTS "Build selene tests" OFF)
option(SELENE_BUILD_EXAMPLES "Build selene examples" OFF)
option(SELENE_BUILD_BENCHMARKS "Build selene benchmarks" OFF)

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
    set(SELENE_COMPILER_OPTIONS -Wall -Wextra -Wpedantic)
    if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        set(SELENE_IMG_COMPILER_OPTIONS -Wno-clobbered)
    endif()
	set(SELENE_COMPILER_DEFINITIONS "")
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
        set(SELENE_COMPILER_OPTIONS /MP /permissive- /wd4324 /wd4611)
        if (NOT "${CMAKE_GENERATOR}" MATCHES "NMake")
            set(SELENE_COMPILER_OPTIONS ${SELENE_COMPILER_OPTIONS} /W4)
        endif()
        set(SELENE_COMPILER_DEFINITIONS _CRT_SECURE_NO_WARNINGS _SCL_SECURE_NO_WARNINGS BOOST_CONFIG_SUPPRESS_OUTDATED_MESSAGE)
endif()

# Add the selene library target
add_subdirectory(src)

# Optionally add the tests target
if (SELENE_BUILD_TESTS)
    add_subdirectory(test)
endif()

if (SELENE_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if (SELENE_BUILD_BENCHMARKS)
    add_subdirectory(benchmark)
endif()