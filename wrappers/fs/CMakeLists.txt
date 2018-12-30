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

# Try to detect if we can compile a simple program that includes the filesystem library

check_cxx_source_compiles("
#include <filesystem>
int main() {}"
        SELENE_CAN_COMPILE_STD_FILESYSTEM)

check_cxx_source_compiles("
#include <experimental/filesystem>
int main() {}"
        SELENE_CAN_COMPILE_STD_EXPERIMENTAL_FILESYSTEM)

# Try to detect if we're using libstdc++ or libc++

if (NOT MSVC)
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
endif()

# Now put everything together, using Boost.Filesystem as a fallback

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
        if (SELENE_CAN_COMPILE_STD_FILESYSTEM)
            target_link_libraries(selene_wrapper_fs INTERFACE stdc++fs)
        elseif(SELENE_CAN_COMPILE_STD_EXPERIMENTAL_FILESYSTEM)
            target_link_libraries(selene_wrapper_fs INTERFACE stdc++experimental)
            target_compile_definitions(selene_wrapper_fs INTERFACE SELENE_WITH_EXPERIMENTAL_STD_FILESYSTEM)
        endif()
    elseif(SELENE_USING_LIBCXX)
        if (SELENE_CAN_COMPILE_STD_FILESYSTEM)
            target_link_libraries(selene_wrapper_fs INTERFACE c++fs)
        elseif(SELENE_CAN_COMPILE_STD_EXPERIMENTAL_FILESYSTEM)
            target_link_libraries(selene_wrapper_fs INTERFACE c++experimental)
            target_compile_definitions(selene_wrapper_fs INTERFACE SELENE_WITH_EXPERIMENTAL_STD_FILESYSTEM)
        endif()
    endif()
endif()
