if(NOT IS_DIRECTORY ${SELENE_DIR})
    message(FATAL_ERROR "Please build using the outermost CMakeLists.txt file.")
endif()

#------------------------------------------------------------------------------

add_executable(image_access "")
target_sources(image_access PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/image_access.cpp
        ${CMAKE_CURRENT_LIST_DIR}/Utils.hpp)
target_compile_options(image_access PRIVATE ${SELENE_COMPILER_OPTIONS})
target_compile_definitions(image_access PRIVATE ${SELENE_COMPILER_DEFINITIONS})
target_include_directories(image_access PRIVATE ${SELENE_DIR}/examples)
target_link_libraries(image_access selene Boost::filesystem)

add_executable(image_clone_view "")
target_sources(image_clone_view PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/image_clone_view.cpp)
target_compile_options(image_clone_view PRIVATE ${SELENE_COMPILER_OPTIONS})
target_compile_definitions(image_clone_view PRIVATE ${SELENE_COMPILER_DEFINITIONS})
target_include_directories(image_clone_view PRIVATE ${SELENE_DIR}/examples)
target_link_libraries(image_clone_view selene Boost::filesystem)

add_executable(image_conversion "")
target_sources(image_conversion PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/image_conversion.cpp)
target_compile_options(image_conversion PRIVATE ${SELENE_COMPILER_OPTIONS})
target_compile_definitions(image_conversion PRIVATE ${SELENE_COMPILER_DEFINITIONS})
target_include_directories(image_conversion PRIVATE ${SELENE_DIR}/examples)
target_link_libraries(image_conversion selene Boost::filesystem)

add_executable(image_convolution "")
target_sources(image_convolution PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/image_convolution.cpp)
target_compile_options(image_convolution PRIVATE ${SELENE_COMPILER_OPTIONS})
target_compile_definitions(image_convolution PRIVATE ${SELENE_COMPILER_DEFINITIONS})
target_include_directories(image_convolution PRIVATE ${SELENE_DIR}/examples)
target_link_libraries(image_convolution selene Boost::filesystem)

add_executable(image_io "")
target_sources(image_io PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/image_io.cpp)
target_compile_options(image_io PRIVATE ${SELENE_COMPILER_OPTIONS})
target_compile_definitions(image_io PRIVATE ${SELENE_COMPILER_DEFINITIONS})
target_include_directories(image_io PRIVATE ${SELENE_DIR}/examples)
target_link_libraries(image_io selene Boost::filesystem)

add_executable(image_resample "")
target_sources(image_resample PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/image_resample.cpp)
target_compile_options(image_resample PRIVATE ${SELENE_COMPILER_OPTIONS})
target_compile_definitions(image_resample PRIVATE ${SELENE_COMPILER_DEFINITIONS})
target_include_directories(image_resample PRIVATE ${SELENE_DIR}/examples)
target_link_libraries(image_resample selene Boost::filesystem)

add_executable(image_rotate "")
target_sources(image_rotate PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/image_rotate.cpp)
target_compile_options(image_rotate PRIVATE ${SELENE_COMPILER_OPTIONS})
target_compile_definitions(image_rotate PRIVATE ${SELENE_COMPILER_DEFINITIONS})
target_include_directories(image_rotate PRIVATE ${SELENE_DIR}/examples)
target_link_libraries(image_rotate selene Boost::filesystem)

add_executable(image_transform "")
target_sources(image_transform PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/image_transform.cpp)
target_compile_options(image_transform PRIVATE ${SELENE_COMPILER_OPTIONS})
target_compile_definitions(image_transform PRIVATE ${SELENE_COMPILER_DEFINITIONS})
target_include_directories(image_transform PRIVATE ${SELENE_DIR}/examples)
target_link_libraries(image_transform selene Boost::filesystem)

add_executable(readme_example "")
target_sources(readme_example PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/readme_example.cpp)
target_compile_options(readme_example PRIVATE ${SELENE_COMPILER_OPTIONS})
target_compile_definitions(readme_example PRIVATE ${SELENE_COMPILER_DEFINITIONS})
target_include_directories(readme_example PRIVATE ${SELENE_DIR}/examples)
target_link_libraries(readme_example selene Boost::filesystem)
