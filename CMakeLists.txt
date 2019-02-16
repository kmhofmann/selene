cmake_minimum_required(VERSION 3.12)
project(selene VERSION 0.3.1 LANGUAGES CXX)

# User-settable options
option(SELENE_BUILD_TESTS "Build Selene tests" OFF)
option(SELENE_BUILD_EXAMPLES "Build Selene examples" OFF)
option(SELENE_BUILD_BENCHMARKS "Build Selene benchmarks" OFF)
option(SELENE_BUILD_ALL "Build Selene tests, examples, and benchmarks (even with other SELENE_BUILD_* variables OFF)" OFF)
if (SELENE_BUILD_ALL)
    set(SELENE_BUILD_TESTS ON)
    set(SELENE_BUILD_EXAMPLES ON)
    set(SELENE_BUILD_BENCHMARKS ON)
endif()

option(SELENE_USE_LIBJPEG "Enable looking for libjpeg" ON)
option(SELENE_USE_LIBPNG "Enable looking for libpng" ON)
option(SELENE_USE_LIBTIFF "Enable looking for libtiff" ON)
option(SELENE_USE_OPENCV "Enable looking for OpenCV" ON)

option(SELENE_USE_DEFAULT_SINGLE_PRECISION "Use single precision for floating point calculations by default (instead of double precision)" OFF)
option(SELENE_WARNINGS_AS_ERRORS "Treat warnings as errors (mainly intended for CI)" OFF)

# https://cmake.org/cmake/help/latest/prop_tgt/CXX_STANDARD.html
set(CMAKE_CXX_STANDARD 17 CACHE STRING "The C++ standard whose features are requested to build this target.")
set(CMAKE_CXX_STANDARD_REQUIRED ON CACHE BOOL "Boolean describing whether the value of CXX_STANDARD is a requirement.")
set(CMAKE_CXX_EXTENSIONS OFF CACHE BOOL "Boolean specifying whether compiler specific extensions are requested.")

# Generate compilation database (e.g. for cquery or YouCompleteMe)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(SELENE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

# Conan support: the cmake_paths generator requires Conan >= 1.4.1
if (EXISTS ${CMAKE_BINARY_DIR}/conan_paths.cmake)
    include(${CMAKE_BINARY_DIR}/conan_paths.cmake)
endif()

include(cmake/build_types.cmake)
include(cmake/get_external.cmake)
include(cmake/find_dependencies.cmake)
include(cmake/compiler_options.cmake)

add_subdirectory(selene)

if (SELENE_BUILD_TESTS OR SELENE_BUILD_EXAMPLES OR SELENE_BUILD_BENCHMARKS)
    add_subdirectory(wrappers/fs)
endif()

if (SELENE_BUILD_TESTS)
    add_subdirectory(test)
else()
    message(STATUS "Not building unit tests; enable with SELENE_BUILD_TESTS=ON if desired.")
endif()

if (SELENE_BUILD_EXAMPLES)
    add_subdirectory(examples)
else()
    message(STATUS "Not building examples; enable with SELENE_BUILD_EXAMPLES=ON if desired.")
endif()

if (SELENE_BUILD_BENCHMARKS)
    add_subdirectory(benchmark)
else()
    message(STATUS "Not building benchmarks; enable with SELENE_BUILD_BENCHMARKS=ON if desired.")
endif()
