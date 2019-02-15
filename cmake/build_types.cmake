# Add "Sanitizers" build type.
# See also https://gitlab.kitware.com/cmake/community/wikis/FAQ#how-can-i-extend-the-build-modes-with-a-custom-made-one-
# And Professional CMake, 2nd ed., chapter 13.3 (https://crascit.com/professional-cmake/).

# CAVEAT: It is not easily settable (or maybe even deterministic?) which build type gets linked for dependencies,
# when building with a custom CMAKE_BUILD_TYPE, or even one that is not 'Debug' or 'Release'.
#
# For example, vcpkg builds both 'Debug' and 'Release' build types. In dependencies' config files, some properties are
# explicitly set for all built build type versions. In the dependency's <libname>Targets.cmake, dependent files get
# included by a file(GLOB ...) command, one for each build type. These included files hard-code the location of the
# respective shared/static library files. The first one included seems to determine which library type gets linked.
#
# Things are handled *differently* with Find* modules, which can lead to inconsistencies (i.e. some debug and some
# release libraries getting linked).


# Check for allowable build types.

get_property(is_multi_config GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)

if(is_multi_config)
    if(NOT "Sanitizers" IN_LIST CMAKE_CONFIGURATION_TYPES)
        list(APPEND CMAKE_CONFIGURATION_TYPES Sanitizers)
    endif()
else()
    set(allowable_build_types Debug Release RelWithDebInfo MinSizeRel Sanitizers)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "${allowable_build_types}")

    if(NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE Debug CACHE STRING "" FORCE)  # Set to build type 'Debug' if no other type is set.
        message(STATUS "No build type set; defaulting to 'Debug'.")
    elseif(NOT CMAKE_BUILD_TYPE IN_LIST allowable_build_types)
        message(FATAL_ERROR "Invalid build type '${CMAKE_BUILD_TYPE}'")
    endif()
endif()

# Update the documentation string of CMAKE_BUILD_TYPE for GUIs.

set(CMAKE_BUILD_TYPE "${CMAKE_BUILD_TYPE}" CACHE STRING
        "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel Sanitizers." FORCE)

# Modify variables to add Sanitizers build type.

set(CMAKE_C_FLAGS_SANITIZERS             "-g -O1 -fsanitize=address -fsanitize=undefined -fno-omit-frame-pointer -fno-optimize-sibling-calls" CACHE STRING "" FORCE)
set(CMAKE_CXX_FLAGS_SANITIZERS           "-g -O1 -fsanitize=address -fsanitize=undefined -fno-omit-frame-pointer -fno-optimize-sibling-calls" CACHE STRING "" FORCE)
set(CMAKE_EXE_LINKER_FLAGS_SANITIZERS    "-fsanitize=address -fsanitize=undefined" CACHE STRING "" FORCE)
set(CMAKE_SHARED_LINKER_FLAGS_SANITIZERS "-fsanitize=address -fsanitize=undefined" CACHE STRING "" FORCE)
set(CMAKE_STATIC_LINKER_FLAGS_SANITIZERS "" CACHE STRING "" FORCE)
set(CMAKE_MODULE_LINKER_FLAGS_SANITIZERS "" CACHE STRING "" FORCE)

mark_as_advanced(CMAKE_C_FLAGS_SANITIZERS CMAKE_CXX_FLAGS_SANITIZERS CMAKE_EXE_LINKER_FLAGS_SANITIZERS
        CMAKE_SHARED_LINKER_FLAGS_SANITIZERS CMAKE_STATIC_LINKER_FLAGS_SANITIZERS CMAKE_MODULE_LINKER_FLAGS_SANITIZERS)

# Set postfix for some of the build types.

set(CMAKE_DEBUG_POSTFIX _debug)
set(CMAKE_SANITIZERS_POSTFIX _sanitizers)