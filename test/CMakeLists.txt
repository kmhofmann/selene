if(NOT IS_DIRECTORY ${SELENE_DIR})
    message(FATAL_ERROR "Please build using the outermost CMakeLists.txt file.")
endif()

#------------------------------------------------------------------------------

add_executable(selene_tests "")

target_sources(selene_tests
        PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/Catch.cpp
        ${CMAKE_CURRENT_LIST_DIR}/Utils.hpp
        ${CMAKE_CURRENT_LIST_DIR}/base/Utils.cpp
        ${CMAKE_CURRENT_LIST_DIR}/img/Image.cpp
        ${CMAKE_CURRENT_LIST_DIR}/img/ImageDataToImage.cpp
        ${CMAKE_CURRENT_LIST_DIR}/img/ImageToImageData.cpp
        ${CMAKE_CURRENT_LIST_DIR}/img/IO.cpp
        ${CMAKE_CURRENT_LIST_DIR}/img/IO_JPEG.cpp
        ${CMAKE_CURRENT_LIST_DIR}/img/IO_PNG.cpp
        ${CMAKE_CURRENT_LIST_DIR}/img/OpenCV.cpp
        ${CMAKE_CURRENT_LIST_DIR}/img/Pixel.cpp
        ${CMAKE_CURRENT_LIST_DIR}/io/IO.cpp
        ${CMAKE_CURRENT_LIST_DIR}/thread/ThreadPool.cpp
        )

target_include_directories(selene_tests
        PRIVATE
        ${Boost_INCLUDE_DIR}
        ${SELENE_DIR}/test
        ${SELENE_DIR}/external/Catch2/single_include
        )

target_link_libraries(selene_tests
        selene
        ${Boost_LIBRARIES}
        pthread
        )
