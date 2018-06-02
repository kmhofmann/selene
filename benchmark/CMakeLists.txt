if(NOT IS_DIRECTORY ${SELENE_DIR})
    message(FATAL_ERROR "Please build using the outermost CMakeLists.txt file.")
endif()

#------------------------------------------------------------------------------

add_executable(benchmark_image_access
        ${CMAKE_CURRENT_LIST_DIR}/image_access.cpp)
target_compile_options(benchmark_image_access PRIVATE ${SELENE_COMPILER_OPTIONS})
target_compile_definitions(benchmark_image_access PRIVATE ${SELENE_COMPILER_DEFINITIONS})
target_include_directories(benchmark_image_access PRIVATE ${SELENE_DIR}/examples ${Boost_INCLUDE_DIR})
target_link_libraries(benchmark_image_access selene Boost::filesystem benchmark::benchmark)
