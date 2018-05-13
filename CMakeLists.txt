cmake_minimum_required(VERSION 3.5)
project(selene VERSION 0.1.0 LANGUAGES CXX)

option(SELENE_BUILD_TESTS "Build Selene tests" OFF)
option(SELENE_BUILD_EXAMPLES "Build Selene examples" OFF)
option(SELENE_BUILD_BENCHMARKS "Build Selene benchmarks" OFF)
option(SELENE_BUILD_ALL "Build Selene tests, examples, and benchmarks (even with other SELENE_BUILD_* variables OFF)" OFF)

if (SELENE_BUILD_ALL)
    set(SELENE_BUILD_TESTS ON)
    set(SELENE_BUILD_EXAMPLES ON)
    set(SELENE_BUILD_BENCHMARKS ON)
endif()

# https://cmake.org/cmake/help/latest/prop_tgt/CXX_STANDARD.html
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Generate compilation database (e.g. for cquery or YouCompleteMe)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(SELENE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

include(cmake/get_external.cmake)
include(cmake/conan_support.cmake)
include(cmake/find_dependencies.cmake)
include(cmake/compiler_options.cmake)

add_subdirectory(selene)

if (SELENE_BUILD_TESTS)
    add_subdirectory(test)
endif()

if (SELENE_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if (SELENE_BUILD_BENCHMARKS)
    add_subdirectory(benchmark)
endif()
