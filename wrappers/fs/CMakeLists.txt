if(NOT IS_DIRECTORY ${SELENE_DIR})
    message(FATAL_ERROR "Please build using the outermost CMakeLists.txt file.")
endif()

#------------------------------------------------------------------------------

add_library(selene_wrapper_fs INTERFACE)
add_library(selene::selene_wrapper_fs ALIAS selene_wrapper_fs)

target_sources(selene_wrapper_fs INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/Filesystem.hpp>
        )

target_include_directories(selene_wrapper_fs INTERFACE
        $<BUILD_INTERFACE:${SELENE_DIR}>)
#        $<INSTALL_INTERFACE:include>)

include(CheckCXXSourceCompiles)

# Try to detect if we can compile a simple program that includes the filesystem
# library.

set(SELENE_FILESYSTEM_TEST_PROGRAM "
    #include <filesystem>
    struct X { std::filesystem::path p; };
    int main() {}")

set(SELENE_FILESYSTEM_EXPERIMENTAL_TEST_PROGRAM "
    #include <experimental/filesystem>
    struct X { std::experimental::filesystem::path p; };
    int main() {}")

# Check if we have std::filesystem at our disposal.
check_cxx_source_compiles(
        "${SELENE_FILESYSTEM_TEST_PROGRAM}"
        SELENE_CAN_COMPILE_STD_FILESYSTEM)

# Check if we have std::experimental::filesystem at our disposal.
check_cxx_source_compiles(
        "${SELENE_FILESYSTEM_EXPERIMENTAL_TEST_PROGRAM}"
        SELENE_CAN_COMPILE_STD_EXPERIMENTAL_FILESYSTEM)

if (NOT MSVC)
    # Try to detect if we're using libstdc++ or libc++.

    check_cxx_source_compiles("
            #include <ciso646>
            int test =
            #ifdef __GLIBCXX__
            1;
            #else
            xxx;
            #endif
            int main() { }"
            SELENE_USING_LIBSTDCXX)

    check_cxx_source_compiles("
            #include <ciso646>
            int test =
            #ifdef _LIBCPP_VERSION
            1;
            #else
            xxx;
            #endif
            int main() { }"
            SELENE_USING_LIBCXX)

    # Try to detect whether we require linking to c++fs (prior to Clang 9.0)

    if (SELENE_USING_LIBCXX)
        set(CMAKE_REQUIRED_LIBRARIES "c++fs")
        check_cxx_source_compiles(
                "${SELENE_FILESYSTEM_TEST_PROGRAM}"
                SELENE_CAN_COMPILE_LIBCXX_WITH_CXXFS)

        set(CMAKE_REQUIRED_LIBRARIES "")
        check_cxx_source_compiles(
                "${SELENE_FILESYSTEM_TEST_PROGRAM}"
                SELENE_CAN_COMPILE_LIBCXX_WITHOUT_CXXFS)
    endif()
endif()

# Now put everything together, using Boost.Filesystem as a fallback.

function(use_boost_filesystem)
    message(STATUS "Standard library filesystem support not detected. Trying to use Boost.Filesystem as a fallback.")
    find_package(Boost REQUIRED filesystem system)
    target_compile_definitions(selene_wrapper_fs INTERFACE SELENE_WITH_BOOST_FILESYSTEM)
    target_link_libraries(selene_wrapper_fs INTERFACE Boost::filesystem)
endfunction()

if (NOT (SELENE_CAN_COMPILE_STD_FILESYSTEM OR SELENE_CAN_COMPILE_STD_EXPERIMENTAL_FILESYSTEM))
    use_boost_filesystem()
else()
    if (SELENE_USING_LIBSTDCXX)

        target_link_libraries(selene_wrapper_fs INTERFACE stdc++fs)
        if(SELENE_CAN_COMPILE_STD_EXPERIMENTAL_FILESYSTEM)
            target_compile_definitions(selene_wrapper_fs INTERFACE SELENE_WITH_EXPERIMENTAL_STD_FILESYSTEM)
        endif()

    elseif(SELENE_USING_LIBCXX)

        if (SELENE_CAN_COMPILE_STD_FILESYSTEM)
            if (SELENE_CAN_COMPILE_LIBCXX_WITH_CXXFS)
                target_link_libraries(selene_wrapper_fs INTERFACE c++fs)
            elseif (NOT SELENE_CAN_COMPILE_LIBCXX_WITHOUT_CXXFS)
                message(FATAL_ERROR "Cannot link to libc++ filesystem library (reason unknown).")
            endif()
        elseif(SELENE_CAN_COMPILE_STD_EXPERIMENTAL_FILESYSTEM)
            target_link_libraries(selene_wrapper_fs INTERFACE c++experimental)
            target_compile_definitions(selene_wrapper_fs INTERFACE SELENE_WITH_EXPERIMENTAL_STD_FILESYSTEM)
        endif()

    endif()
endif()
