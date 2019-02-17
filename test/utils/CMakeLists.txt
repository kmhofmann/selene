if(NOT IS_DIRECTORY ${SELENE_DIR})
    message(FATAL_ERROR "Please build using the outermost CMakeLists.txt file.")
endif()

#------------------------------------------------------------------------------

add_library(selene_test_utils "")

target_sources(selene_test_utils PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/Utils.hpp
        ${CMAKE_CURRENT_LIST_DIR}/Utils.cpp
        )

target_compile_options(selene_test_utils PRIVATE ${SELENE_COMPILE_OPTIONS})
target_compile_definitions(selene_test_utils PRIVATE ${SELENE_COMPILE_DEFINITIONS})
target_link_libraries(selene_test_utils selene_base selene_wrapper_fs)
