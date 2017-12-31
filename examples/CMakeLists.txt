if(NOT IS_DIRECTORY ${SELENE_DIR})
    message(FATAL_ERROR "Please build using the outermost CMakeLists.txt file.")
endif()

#------------------------------------------------------------------------------

find_package(Boost REQUIRED filesystem system)

add_executable(image_conversion
        ${CMAKE_CURRENT_LIST_DIR}/image_conversion.cpp
        ${CMAKE_CURRENT_LIST_DIR}/Utils.hpp)
target_compile_options(image_conversion PRIVATE ${SELENE_COMPILER_WARNING_OPTIONS})
target_compile_definitions(image_conversion PRIVATE ${SELENE_COMPILER_EXTRA_DEFINITIONS})
target_include_directories(image_conversion PRIVATE ${SELENE_DIR}/examples ${Boost_INCLUDE_DIR})
target_link_libraries(image_conversion selene ${Boost_LIBRARIES})
