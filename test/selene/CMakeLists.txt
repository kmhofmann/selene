if(NOT IS_DIRECTORY ${SELENE_DIR})
    message(FATAL_ERROR "Please build using the outermost CMakeLists.txt file.")
endif()

#------------------------------------------------------------------------------

add_executable(selene_DEPRECATED_tests
        ${CMAKE_CURRENT_LIST_DIR}/Catch.cpp
        ${CMAKE_CURRENT_LIST_DIR}/Utils.hpp
        ${CMAKE_CURRENT_LIST_DIR}/base/Allocators.cpp
        ${CMAKE_CURRENT_LIST_DIR}/base/Bitcount.cpp
        ${CMAKE_CURRENT_LIST_DIR}/base/Round.cpp
        ${CMAKE_CURRENT_LIST_DIR}/base/_Utils.cpp
        ${CMAKE_CURRENT_LIST_DIR}/base/io/IO.cpp

        ${CMAKE_CURRENT_LIST_DIR}/DEPRECATED_img/_TestImages.cpp
        ${CMAKE_CURRENT_LIST_DIR}/DEPRECATED_img/_TestImages.hpp
        ${CMAKE_CURRENT_LIST_DIR}/DEPRECATED_img/BorderAccessors.cpp
        ${CMAKE_CURRENT_LIST_DIR}/DEPRECATED_img/Image.cpp
        ${CMAKE_CURRENT_LIST_DIR}/DEPRECATED_img/ImageAccess.cpp
        ${CMAKE_CURRENT_LIST_DIR}/DEPRECATED_img/ImageData.cpp
        ${CMAKE_CURRENT_LIST_DIR}/DEPRECATED_img/ImageDataToImage.cpp
        ${CMAKE_CURRENT_LIST_DIR}/DEPRECATED_img/ImageToImageData.cpp
        ${CMAKE_CURRENT_LIST_DIR}/DEPRECATED_img/Interpolators.cpp
        ${CMAKE_CURRENT_LIST_DIR}/DEPRECATED_img/OpenCV.cpp
        ${CMAKE_CURRENT_LIST_DIR}/DEPRECATED_img/Pixel.cpp
        ${CMAKE_CURRENT_LIST_DIR}/DEPRECATED_img/Utilities.cpp
        ${CMAKE_CURRENT_LIST_DIR}/DEPRECATED_img_io/IO.cpp
        ${CMAKE_CURRENT_LIST_DIR}/DEPRECATED_img_io/IO_JPEG.cpp
        ${CMAKE_CURRENT_LIST_DIR}/DEPRECATED_img_io/IO_PNG.cpp
        ${CMAKE_CURRENT_LIST_DIR}/DEPRECATED_img_ops/Algorithms.cpp
        ${CMAKE_CURRENT_LIST_DIR}/DEPRECATED_img_ops/ChannelOperations.cpp
        ${CMAKE_CURRENT_LIST_DIR}/DEPRECATED_img_ops/ImageConversions.cpp
        ${CMAKE_CURRENT_LIST_DIR}/DEPRECATED_img_ops/PixelConversions.cpp
        ${CMAKE_CURRENT_LIST_DIR}/DEPRECATED_img_ops/Resample.cpp
        ${CMAKE_CURRENT_LIST_DIR}/DEPRECATED_img_ops/Transformations.cpp
        )

target_compile_options(selene_DEPRECATED_tests PRIVATE
        ${SELENE_COMPILER_OPTIONS}
        ${SELENE_TEST_COMPILER_OPTIONS})

target_compile_definitions(selene_DEPRECATED_tests PRIVATE
        ${SELENE_COMPILER_DEFINITIONS})

target_include_directories(selene_DEPRECATED_tests PRIVATE
        ${SELENE_DIR}
        ${SELENE_DIR}/external/Catch2/single_include)

target_link_libraries(selene_DEPRECATED_tests
        selene_DEPRECATED
        ${SELENE_LINK_LIBRARY_FILESYSTEM})

if (OPENCV_CORE_FOUND)
    target_link_libraries(selene_DEPRECATED_tests opencv_core)
endif()

#------------------------------------------------------------------------------

add_executable(selene_tests
        ${CMAKE_CURRENT_LIST_DIR}/Catch.cpp
        ${CMAKE_CURRENT_LIST_DIR}/Utils.hpp

        ${CMAKE_CURRENT_LIST_DIR}/base/Allocators.cpp
        ${CMAKE_CURRENT_LIST_DIR}/base/Bitcount.cpp
        ${CMAKE_CURRENT_LIST_DIR}/base/Round.cpp
        ${CMAKE_CURRENT_LIST_DIR}/base/_Utils.cpp
        ${CMAKE_CURRENT_LIST_DIR}/base/io/IO.cpp

        ${CMAKE_CURRENT_LIST_DIR}/img/common/BoundingBox.cpp
        ${CMAKE_CURRENT_LIST_DIR}/img/common/PixelFormat.cpp
        ${CMAKE_CURRENT_LIST_DIR}/img/common/Types.cpp

        ${CMAKE_CURRENT_LIST_DIR}/img/pixel/Pixel.cpp

        ${CMAKE_CURRENT_LIST_DIR}/img/dynamic/DynImage.cpp
        ${CMAKE_CURRENT_LIST_DIR}/img/dynamic/DynImageIterators.cpp
        ${CMAKE_CURRENT_LIST_DIR}/img/dynamic/UntypedLayout.cpp
        ${CMAKE_CURRENT_LIST_DIR}/img/dynamic/Utils.hpp

        ${CMAKE_CURRENT_LIST_DIR}/img/typed/Image.cpp
        ${CMAKE_CURRENT_LIST_DIR}/img/typed/ImageIterators.cpp
        ${CMAKE_CURRENT_LIST_DIR}/img/typed/TypedLayout.cpp
        ${CMAKE_CURRENT_LIST_DIR}/img/typed/Utilities.cpp
        ${CMAKE_CURRENT_LIST_DIR}/img/typed/_Utils.hpp

        ${CMAKE_CURRENT_LIST_DIR}/img/typed/access/BorderAccessors.cpp
        ${CMAKE_CURRENT_LIST_DIR}/img/typed/access/ImageAccess.cpp
        ${CMAKE_CURRENT_LIST_DIR}/img/typed/access/Interpolators.cpp
        ${CMAKE_CURRENT_LIST_DIR}/img/typed/access/RelativeAccessor.cpp
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
        ${SELENE_LINK_LIBRARY_FILESYSTEM})

if (OPENCV_CORE_FOUND)
    target_link_libraries(selene_tests opencv_core)
endif()