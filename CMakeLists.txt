cmake_minimum_required(VERSION 3.5)
project(selene VERSION 0.1.0 LANGUAGES CXX)

option(SELENE_BUILD_TESTS "Build Selene tests" OFF)
option(SELENE_BUILD_EXAMPLES "Build Selene examples" OFF)
option(SELENE_BUILD_BENCHMARKS "Build Selene benchmarks" OFF)

# https://cmake.org/cmake/help/latest/prop_tgt/CXX_STANDARD.html
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(GNUInstallDirs)
include(external/external.cmake)

# Generate compilation database (e.g. for cquery or YouCompleteMe)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(SELENE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
    set(SELENE_COMPILER_OPTIONS -Wall -Wextra -Wpedantic)
    if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        set(SELENE_IMG_COMPILER_OPTIONS -Wno-clobbered)
        set(SELENE_TEST_COMPILER_OPTIONS -Wno-unused-but-set-variable)
    endif()
	set(SELENE_COMPILER_DEFINITIONS "")
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    set(SELENE_COMPILER_OPTIONS /MP /permissive- /wd4324 /wd4611)
    if (NOT "${CMAKE_GENERATOR}" MATCHES "NMake")
        set(SELENE_COMPILER_OPTIONS ${SELENE_COMPILER_OPTIONS} /W4)
    endif()
    set(SELENE_COMPILER_DEFINITIONS _CRT_SECURE_NO_WARNINGS _SCL_SECURE_NO_WARNINGS BOOST_CONFIG_SUPPRESS_OUTDATED_MESSAGE)
endif()

add_subdirectory(src)

if (SELENE_BUILD_TESTS)
    add_subdirectory(test)
endif()

if (SELENE_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if (SELENE_BUILD_BENCHMARKS)
    add_subdirectory(benchmark)
endif()
