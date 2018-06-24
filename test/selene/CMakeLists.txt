if(NOT IS_DIRECTORY ${SELENE_DIR})
    message(FATAL_ERROR "Please build using the outermost CMakeLists.txt file.")
endif()

#------------------------------------------------------------------------------

add_executable(selene_tests
        ${CMAKE_CURRENT_LIST_DIR}/Catch.cpp
        ${CMAKE_CURRENT_LIST_DIR}/Utils.hpp
        ${CMAKE_CURRENT_LIST_DIR}/base/Allocators.cpp
        ${CMAKE_CURRENT_LIST_DIR}/base/Bitcount.cpp
        ${CMAKE_CURRENT_LIST_DIR}/base/Round.cpp
        ${CMAKE_CURRENT_LIST_DIR}/base/Utils.cpp
        ${CMAKE_CURRENT_LIST_DIR}/img/_TestImages.cpp
        ${CMAKE_CURRENT_LIST_DIR}/img/_TestImages.hpp
        ${CMAKE_CURRENT_LIST_DIR}/img/BorderAccessors.cpp
        ${CMAKE_CURRENT_LIST_DIR}/img/Image.cpp
        ${CMAKE_CURRENT_LIST_DIR}/img/ImageAccess.cpp
        ${CMAKE_CURRENT_LIST_DIR}/img/ImageData.cpp
        ${CMAKE_CURRENT_LIST_DIR}/img/ImageDataToImage.cpp
        ${CMAKE_CURRENT_LIST_DIR}/img/ImageToImageData.cpp
        ${CMAKE_CURRENT_LIST_DIR}/img/Interpolators.cpp
        ${CMAKE_CURRENT_LIST_DIR}/img/OpenCV.cpp
        ${CMAKE_CURRENT_LIST_DIR}/img/Pixel.cpp
        ${CMAKE_CURRENT_LIST_DIR}/img/Utilities.cpp
        ${CMAKE_CURRENT_LIST_DIR}/img_io/IO.cpp
        ${CMAKE_CURRENT_LIST_DIR}/img_io/IO_JPEG.cpp
        ${CMAKE_CURRENT_LIST_DIR}/img_io/IO_PNG.cpp
        ${CMAKE_CURRENT_LIST_DIR}/io/IO.cpp
        ${CMAKE_CURRENT_LIST_DIR}/img_ops/Algorithms.cpp
        ${CMAKE_CURRENT_LIST_DIR}/img_ops/ChannelOperations.cpp
        ${CMAKE_CURRENT_LIST_DIR}/img_ops/ImageConversions.cpp
        ${CMAKE_CURRENT_LIST_DIR}/img_ops/PixelConversions.cpp
        ${CMAKE_CURRENT_LIST_DIR}/img_ops/Resample.cpp
        ${CMAKE_CURRENT_LIST_DIR}/img_ops/Transformations.cpp
        ${CMAKE_CURRENT_LIST_DIR}/thread/ThreadPool.cpp
        )

target_compile_options(selene_tests PRIVATE
        ${SELENE_COMPILER_OPTIONS}
        ${SELENE_TEST_COMPILER_OPTIONS})

target_compile_definitions(selene_tests PRIVATE
        ${SELENE_COMPILER_DEFINITIONS})

target_include_directories(selene_tests PRIVATE
        ${SELENE_DIR}
        ${SELENE_DIR}/external/Catch2/single_include)

target_link_libraries(selene_tests
        selene
        Boost::filesystem
        Threads::Threads)

if (OPENCV_CORE_FOUND)
    target_link_libraries(selene_tests opencv_core)
endif()
