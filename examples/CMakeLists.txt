if(NOT IS_DIRECTORY ${SELENE_DIR})
    message(FATAL_ERROR "Please build using the outermost CMakeLists.txt file.")
endif()

#------------------------------------------------------------------------------

add_executable(image_access
        ${CMAKE_CURRENT_LIST_DIR}/image_access.cpp
        ${CMAKE_CURRENT_LIST_DIR}/Utils.hpp)
target_compile_options(image_access PRIVATE ${SELENE_COMPILER_OPTIONS})
target_compile_definitions(image_access PRIVATE ${SELENE_COMPILER_DEFINITIONS})
target_include_directories(image_access PRIVATE ${SELENE_DIR}/examples ${Boost_INCLUDE_DIR})
target_link_libraries(image_access selene ${SELENE_BOOST_TARGET_NAME})

add_executable(image_clone_view
        ${CMAKE_CURRENT_LIST_DIR}/image_clone_view.cpp)
target_compile_options(image_clone_view PRIVATE ${SELENE_COMPILER_OPTIONS})
target_compile_definitions(image_clone_view PRIVATE ${SELENE_COMPILER_DEFINITIONS})
target_include_directories(image_clone_view PRIVATE ${SELENE_DIR}/examples ${Boost_INCLUDE_DIR})
target_link_libraries(image_clone_view selene ${SELENE_BOOST_TARGET_NAME})

add_executable(image_conversion
        ${CMAKE_CURRENT_LIST_DIR}/image_conversion.cpp)
target_compile_options(image_conversion PRIVATE ${SELENE_COMPILER_OPTIONS})
target_compile_definitions(image_conversion PRIVATE ${SELENE_COMPILER_DEFINITIONS})
target_include_directories(image_conversion PRIVATE ${SELENE_DIR}/examples ${Boost_INCLUDE_DIR})
target_link_libraries(image_conversion selene ${SELENE_BOOST_TARGET_NAME})

add_executable(image_io
        ${CMAKE_CURRENT_LIST_DIR}/image_io.cpp)
target_compile_options(image_io PRIVATE ${SELENE_COMPILER_OPTIONS})
target_compile_definitions(image_io PRIVATE ${SELENE_COMPILER_DEFINITIONS})
target_include_directories(image_io PRIVATE ${SELENE_DIR}/examples ${Boost_INCLUDE_DIR})
target_link_libraries(image_io selene ${SELENE_BOOST_TARGET_NAME})

add_executable(image_resample
        ${CMAKE_CURRENT_LIST_DIR}/image_resample.cpp)
target_compile_options(image_resample PRIVATE ${SELENE_COMPILER_OPTIONS})
target_compile_definitions(image_resample PRIVATE ${SELENE_COMPILER_DEFINITIONS})
target_include_directories(image_resample PRIVATE ${SELENE_DIR}/examples ${Boost_INCLUDE_DIR})
target_link_libraries(image_resample selene ${SELENE_BOOST_TARGET_NAME})

add_executable(image_rotate
        ${CMAKE_CURRENT_LIST_DIR}/image_rotate.cpp)
target_compile_options(image_rotate PRIVATE ${SELENE_COMPILER_OPTIONS})
target_compile_definitions(image_rotate PRIVATE ${SELENE_COMPILER_DEFINITIONS})
target_include_directories(image_rotate PRIVATE ${SELENE_DIR}/examples ${Boost_INCLUDE_DIR})
target_link_libraries(image_rotate selene ${SELENE_BOOST_TARGET_NAME})

add_executable(image_transform
        ${CMAKE_CURRENT_LIST_DIR}/image_transform.cpp)
target_compile_options(image_transform PRIVATE ${SELENE_COMPILER_OPTIONS})
target_compile_definitions(image_transform PRIVATE ${SELENE_COMPILER_DEFINITIONS})
target_include_directories(image_transform PRIVATE ${SELENE_DIR}/examples ${Boost_INCLUDE_DIR})
target_link_libraries(image_transform selene ${SELENE_BOOST_TARGET_NAME})

add_executable(readme_example
        ${CMAKE_CURRENT_LIST_DIR}/readme_example.cpp)
target_compile_options(readme_example PRIVATE ${SELENE_COMPILER_OPTIONS})
target_compile_definitions(readme_example PRIVATE ${SELENE_COMPILER_DEFINITIONS})
target_include_directories(readme_example PRIVATE ${SELENE_DIR}/examples ${Boost_INCLUDE_DIR})
target_link_libraries(readme_example selene ${SELENE_BOOST_TARGET_NAME})