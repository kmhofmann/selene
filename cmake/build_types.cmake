# Set postfix for some of the build types.

set(CMAKE_DEBUG_POSTFIX _debug)

# Modify flags variables if project should be built with sanitizers.

if(SELENE_ENABLE_SANITIZERS AND ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang"))
    message(STATUS "Building with AddressSanitizer and UndefinedBehaviorSanitizer enabled:")

    if(NOT CMAKE_BUILD_TYPE)  # This is the recommended setting for sanitizers.
        string(APPEND CMAKE_CXX_FLAGS " -O1 -g -fsanitize=address -fsanitize=undefined -fno-omit-frame-pointer -fno-optimize-sibling-calls")
        string(APPEND CMAKE_EXE_LINKER_FLAGS " -fsanitize=address -fsanitize=undefined")
        string(APPEND CMAKE_SHARED_LINKER_FLAGS " -fsanitize=address -fsanitize=undefined")

        message(STATUS "  CMAKE_CXX_FLAGS:           ${CMAKE_CXX_FLAGS}")
        message(STATUS "  CMAKE_EXE_LINKER_FLAGS:    ${CMAKE_EXE_LINKER_FLAGS}")
        message(STATUS "  CMAKE_SHARED_LINKER_FLAGS: ${CMAKE_SHARED_LINKER_FLAGS}")
    else()
        message(WARNING "CMAKE_BUILD_TYPE was set to ${CMAKE_BUILD_TYPE}.\
            It is recommended to leave CMAKE_BUILD_TYPE unset when enabling sanitizers.")

        string(TOUPPER ${CMAKE_BUILD_TYPE} build_type)
        string(APPEND CMAKE_CXX_FLAGS_${build_type} " -fsanitize=address -fsanitize=undefined -fno-omit-frame-pointer -fno-optimize-sibling-calls")
        string(APPEND CMAKE_EXE_LINKER_FLAGS_${build_type} " -fsanitize=address -fsanitize=undefined")
        string(APPEND CMAKE_SHARED_LINKER_FLAGS_${build_type} " -fsanitize=address -fsanitize=undefined")

        message(STATUS "  CMAKE_CXX_FLAGS_${build_type}:           ${CMAKE_CXX_FLAGS_${build_type}}")
        message(STATUS "  CMAKE_EXE_LINKER_FLAGS_${build_type}:    ${CMAKE_EXE_LINKER_FLAGS_${build_type}}")
        message(STATUS "  CMAKE_SHARED_LINKER_FLAGS_${build_type}: ${CMAKE_SHARED_LINKER_FLAGS_${build_type}}")
    endif()
endif()