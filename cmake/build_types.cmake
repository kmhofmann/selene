# Add "Sanitizers" build type.
# See also https://gitlab.kitware.com/cmake/community/wikis/FAQ#how-can-i-extend-the-build-modes-with-a-custom-made-one-

set(SELENE_CMAKE_SANITIZER_FLAGS
        "-fsanitize=address -fsanitize=undefined -fno-omit-frame-pointer -fno-optimize-sibling-calls -g -O1")

set(CMAKE_CXX_FLAGS_SANITIZERS ${SELENE_CMAKE_SANITIZER_FLAGS} CACHE STRING
        "Flags used by the C++ compiler during sanitizer-enabled builds." FORCE)
set(CMAKE_C_FLAGS_SANITIZERS ${SELENE_CMAKE_SANITIZER_FLAGS} CACHE STRING
        "Flags used by the C compiler during sanitizer-enabled builds." FORCE)
set(CMAKE_EXE_LINKER_FLAGS_SANITIZERS ${SELENE_CMAKE_SANITIZER_FLAGS} CACHE STRING
        "Flags used for linking binaries during sanitizer-enabled builds." FORCE)
set(CMAKE_SHARED_LINKER_FLAGS_SANITIZERS ${SELENE_CMAKE_SANITIZER_FLAGS} CACHE STRING
        "Flags used by the shared libraries linker during sanitizer-enabled builds." FORCE)

mark_as_advanced(CMAKE_CXX_FLAGS_SANITIZERS
                 CMAKE_C_FLAGS_SANITIZERS
                 CMAKE_EXE_LINKER_FLAGS_SANITIZERS
                 CMAKE_SHARED_LINKER_FLAGS_SANITIZERS)

# Update the documentation string of CMAKE_BUILD_TYPE for GUIs
set(CMAKE_BUILD_TYPE "${CMAKE_BUILD_TYPE}" CACHE STRING
        "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel Sanitizers." FORCE)
