if(NOT IS_DIRECTORY ${SELENE_DIR})
    message(FATAL_ERROR "Please build using the outermost CMakeLists.txt file.")
endif()

#------------------------------------------------------------------------------

find_package(Boost REQUIRED filesystem system)
find_package(Threads REQUIRED)

add_executable(selene_tests
        ${CMAKE_CURRENT_LIST_DIR}/Catch.cpp
        ${CMAKE_CURRENT_LIST_DIR}/Utils.hpp
        ${CMAKE_CURRENT_LIST_DIR}/base/Allocators.cpp
        ${CMAKE_CURRENT_LIST_DIR}/base/Bitcount.cpp
        ${CMAKE_CURRENT_LIST_DIR}/base/Utils.cpp
        ${CMAKE_CURRENT_LIST_DIR}/img/_TestImages.cpp
        ${CMAKE_CURRENT_LIST_DIR}/img/_TestImages.hpp
        ${CMAKE_CURRENT_LIST_DIR}/img/BorderAccessors.cpp
        ${CMAKE_CURRENT_LIST_DIR}/img/Algorithms.cpp
        ${CMAKE_CURRENT_LIST_DIR}/img/Image.cpp
        ${CMAKE_CURRENT_LIST_DIR}/img/ImageConversions.cpp
        ${CMAKE_CURRENT_LIST_DIR}/img/ImageAccess.cpp
        ${CMAKE_CURRENT_LIST_DIR}/img/ImageDataToImage.cpp
        ${CMAKE_CURRENT_LIST_DIR}/img/ImageToImageData.cpp
        ${CMAKE_CURRENT_LIST_DIR}/img/Interpolators.cpp
        ${CMAKE_CURRENT_LIST_DIR}/img/IO.cpp
        ${CMAKE_CURRENT_LIST_DIR}/img/IO_JPEG.cpp
        ${CMAKE_CURRENT_LIST_DIR}/img/IO_PNG.cpp
        ${CMAKE_CURRENT_LIST_DIR}/img/OpenCV.cpp
        ${CMAKE_CURRENT_LIST_DIR}/img/Pixel.cpp
        ${CMAKE_CURRENT_LIST_DIR}/img/PixelConversions.cpp
        ${CMAKE_CURRENT_LIST_DIR}/img/Resample.cpp
        ${CMAKE_CURRENT_LIST_DIR}/img/Transformations.cpp
        ${CMAKE_CURRENT_LIST_DIR}/io/IO.cpp
        ${CMAKE_CURRENT_LIST_DIR}/thread/ThreadPool.cpp
        )

target_compile_options(selene_tests
        PRIVATE
        ${SELENE_COMPILER_OPTIONS})

target_compile_definitions(selene_tests
        PRIVATE
        ${SELENE_COMPILER_DEFINITIONS})

target_include_directories(selene_tests
        PRIVATE
        ${Boost_INCLUDE_DIR}
        ${SELENE_DIR}/test
        ${SELENE_DIR}/external/Catch2/single_include)

target_link_libraries(selene_tests
        selene
        ${Boost_LIBRARIES})

if(THREADS_HAVE_PTHREAD_ARG)
    target_compile_options(my_app PUBLIC "-pthread")
endif()

if(CMAKE_THREAD_LIBS_INIT)
    target_link_libraries(selene_tests "${CMAKE_THREAD_LIBS_INIT}")
endif()
