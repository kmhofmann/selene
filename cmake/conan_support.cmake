# Conan support
# (in anticipation of having https://github.com/conan-io/conan/issues/2463 resolved in a release)
if (EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    set(SELENE_CONAN_VERSION_REQUIRED 1.3.0)

    # Find Conan binary
    find_program(CONAN_BIN conan)
    if(NOT CONAN_BIN)
        message(FATAL_ERROR "Conan executable not found!")
    endif()

    # Check Conan version
    execute_process(COMMAND ${CONAN_BIN} --version OUTPUT_VARIABLE CONAN_VERSION_OUTPUT)
    string(REGEX MATCH ".*Conan version ([0-9]+\.[0-9]+\.[0-9]+)" FOO "${CONAN_VERSION_OUTPUT}")
    if(${CMAKE_MATCH_1} VERSION_LESS ${SELENE_CONAN_VERSION_REQUIRED})
        message(FATAL_ERROR "Please upgrade Conan to a version >= ${SELENE_CONAN_VERSION_REQUIRED}.")
    endif()

    set(SELENE_HAVE_CONAN TRUE)

    include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    conan_basic_setup(TARGETS KEEP_RPATHS)

    if (APPLE)
        set(CMAKE_INSTALL_RPATH "@executable_path/../lib")
    else()
        set(CMAKE_INSTALL_RPATH "$ORIGIN/../lib")
    endif()
endif()