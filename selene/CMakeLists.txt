if(NOT IS_DIRECTORY ${SELENE_DIR})
    message(FATAL_ERROR "Please build using the outermost CMakeLists.txt file.")
endif()

#------------------------------------------------------------------------------

add_library(selene_base
        ${CMAKE_CURRENT_LIST_DIR}/base/Allocators.cpp
        ${CMAKE_CURRENT_LIST_DIR}/base/Allocators.hpp
        ${CMAKE_CURRENT_LIST_DIR}/base/Assert.hpp
        ${CMAKE_CURRENT_LIST_DIR}/base/Bitcount.hpp
        ${CMAKE_CURRENT_LIST_DIR}/base/MemoryBlock.hpp
        ${CMAKE_CURRENT_LIST_DIR}/base/MessageLog.hpp
        ${CMAKE_CURRENT_LIST_DIR}/base/ExplicitType.hpp
        ${CMAKE_CURRENT_LIST_DIR}/base/Promote.hpp
        ${CMAKE_CURRENT_LIST_DIR}/base/Round.hpp
        ${CMAKE_CURRENT_LIST_DIR}/base/Types.hpp
        ${CMAKE_CURRENT_LIST_DIR}/base/Utils.hpp
        )
add_library(selene::selene_base ALIAS selene_base)

target_compile_options(selene_base PRIVATE ${SELENE_COMPILER_OPTIONS})

target_compile_definitions(selene_base PRIVATE ${SELENE_COMPILER_DEFINITIONS})

target_include_directories(selene_base PUBLIC
        $<BUILD_INTERFACE:${SELENE_DIR}>
        $<INSTALL_INTERFACE:include>)

#------------------------------------------------------------------------------

add_library(selene_img
        ${CMAKE_CURRENT_LIST_DIR}/img/BorderAccessors.hpp
        ${CMAKE_CURRENT_LIST_DIR}/img/BoundingBox.hpp
        ${CMAKE_CURRENT_LIST_DIR}/img/Image.hpp
        ${CMAKE_CURRENT_LIST_DIR}/img/ImageAccess.hpp
        ${CMAKE_CURRENT_LIST_DIR}/img/ImageData.hpp
        ${CMAKE_CURRENT_LIST_DIR}/img/ImageDataBase.hpp
        ${CMAKE_CURRENT_LIST_DIR}/img/ImageDataStorage.hpp
        ${CMAKE_CURRENT_LIST_DIR}/img/ImageDataToImage.hpp
        ${CMAKE_CURRENT_LIST_DIR}/img/ImageToImageData.hpp
        ${CMAKE_CURRENT_LIST_DIR}/img/Interpolators.hpp
        ${CMAKE_CURRENT_LIST_DIR}/img/OpenCV.hpp
        ${CMAKE_CURRENT_LIST_DIR}/img/Pixel.hpp
        ${CMAKE_CURRENT_LIST_DIR}/img/PixelFormat.cpp
        ${CMAKE_CURRENT_LIST_DIR}/img/PixelFormat.hpp
        ${CMAKE_CURRENT_LIST_DIR}/img/PixelTraits.hpp
        ${CMAKE_CURRENT_LIST_DIR}/img/RelativeAccessor.hpp
        ${CMAKE_CURRENT_LIST_DIR}/img/RowPointers.hpp
        ${CMAKE_CURRENT_LIST_DIR}/img/Types.hpp
        ${CMAKE_CURRENT_LIST_DIR}/img_io/IO.cpp
        ${CMAKE_CURRENT_LIST_DIR}/img_io/IO.hpp
        ${CMAKE_CURRENT_LIST_DIR}/img_io/JPEGCommon.hpp
        ${CMAKE_CURRENT_LIST_DIR}/img_io/JPEGRead.cpp
        ${CMAKE_CURRENT_LIST_DIR}/img_io/JPEGRead.hpp
        ${CMAKE_CURRENT_LIST_DIR}/img_io/JPEGWrite.cpp
        ${CMAKE_CURRENT_LIST_DIR}/img_io/JPEGWrite.hpp
        ${CMAKE_CURRENT_LIST_DIR}/img_io/PNGRead.cpp
        ${CMAKE_CURRENT_LIST_DIR}/img_io/PNGRead.hpp
        ${CMAKE_CURRENT_LIST_DIR}/img_io/PNGWrite.cpp
        ${CMAKE_CURRENT_LIST_DIR}/img_io/PNGWrite.hpp
        ${CMAKE_CURRENT_LIST_DIR}/img_io/detail/JPEGCommon.hpp
        ${CMAKE_CURRENT_LIST_DIR}/img_io/detail/JPEGDetail.cpp
        ${CMAKE_CURRENT_LIST_DIR}/img_io/detail/JPEGDetail.hpp
        ${CMAKE_CURRENT_LIST_DIR}/img_io/detail/PNGDetail.cpp
        ${CMAKE_CURRENT_LIST_DIR}/img_io/detail/PNGDetail.hpp
        ${CMAKE_CURRENT_LIST_DIR}/img_io/detail/Util.hpp
        ${CMAKE_CURRENT_LIST_DIR}/img_ops/Algorithms.hpp
        ${CMAKE_CURRENT_LIST_DIR}/img_ops/ImageConversions.hpp
        ${CMAKE_CURRENT_LIST_DIR}/img_ops/PixelConversions.cpp
        ${CMAKE_CURRENT_LIST_DIR}/img_ops/PixelConversions.hpp
        ${CMAKE_CURRENT_LIST_DIR}/img_ops/Resample.hpp
        ${CMAKE_CURRENT_LIST_DIR}/img_ops/Transformations.hpp
        )
add_library(selene::selene_img ALIAS selene_img)

target_compile_options(selene_img PRIVATE ${SELENE_COMPILER_OPTIONS} ${SELENE_IMG_COMPILER_OPTIONS})

target_compile_definitions(selene_img PRIVATE ${SELENE_COMPILER_DEFINITIONS})

target_include_directories(selene_img PUBLIC
        $<BUILD_INTERFACE:${SELENE_DIR}>
        $<INSTALL_INTERFACE:include>)

if (JPEG_FOUND)
    target_compile_definitions(selene_img PUBLIC SELENE_WITH_LIBJPEG)
    if (JPEG_PARTIAL_DECODING)
        target_compile_definitions(selene_img PUBLIC SELENE_LIBJPEG_PARTIAL_DECODING)
    endif()
    if (JPEG_EXTENDED_COLORSPACES)
        target_compile_definitions(selene_img PUBLIC SELENE_LIBJPEG_EXTENDED_COLORSPACES)
    endif()
    target_include_directories(selene_img PUBLIC ${JPEG_INCLUDE_DIR})
    target_link_libraries(selene_img PUBLIC ${JPEG_LIBRARIES})
    # Imported target support upcoming in CMake (commit 87f2cf3b1ce0ef7b8e71eae0f44cf9753660d423)
    #target_link_libraries(selene_img PUBLIC JPEG::JPEG)
endif()

if (PNG_FOUND)
    target_compile_definitions(selene_img PUBLIC SELENE_WITH_LIBPNG)
    target_link_libraries(selene_img PUBLIC PNG::PNG)
endif()

if (OPENCV_CORE_FOUND)
    target_compile_definitions(selene_img PUBLIC SELENE_WITH_OPENCV)
    target_link_libraries(selene_img PUBLIC opencv_core)
endif()

target_link_libraries(selene_img PUBLIC selene_base)

#------------------------------------------------------------------------------

add_library(selene_io
        ${CMAKE_CURRENT_LIST_DIR}/io/FileReader.hpp
        ${CMAKE_CURRENT_LIST_DIR}/io/FileUtils.cpp
        ${CMAKE_CURRENT_LIST_DIR}/io/FileUtils.hpp
        ${CMAKE_CURRENT_LIST_DIR}/io/FileWriter.hpp
        ${CMAKE_CURRENT_LIST_DIR}/io/MemoryReader.hpp
        ${CMAKE_CURRENT_LIST_DIR}/io/MemoryWriter.hpp
        ${CMAKE_CURRENT_LIST_DIR}/io/VectorReader.hpp
        ${CMAKE_CURRENT_LIST_DIR}/io/VectorWriter.hpp
        ${CMAKE_CURRENT_LIST_DIR}/io/WriterMode.hpp
        )
add_library(selene::selene_io ALIAS selene_io)

target_compile_options(selene_io PRIVATE ${SELENE_COMPILER_OPTIONS})

target_compile_definitions(selene_io PRIVATE ${SELENE_COMPILER_DEFINITIONS})

target_include_directories(selene_io PUBLIC
        $<BUILD_INTERFACE:${SELENE_DIR}>
        $<INSTALL_INTERFACE:include>)

#------------------------------------------------------------------------------

add_library(selene_thread INTERFACE)
add_library(selene::selene_thread ALIAS selene_thread)

target_sources(selene_thread INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/thread/ThreadPool.hpp>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/thread/detail/Callable.hpp>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/thread/detail/TaskQueue.hpp>
        )

target_include_directories(selene_thread INTERFACE
        $<BUILD_INTERFACE:${SELENE_DIR}>
        $<INSTALL_INTERFACE:include>)

#------------------------------------------------------------------------------

add_library(selene INTERFACE)
add_library(selene::selene ALIAS selene)

target_include_directories(selene INTERFACE
        $<BUILD_INTERFACE:${SELENE_DIR}>
        $<INSTALL_INTERFACE:include>)

target_link_libraries(selene INTERFACE selene_base selene_img selene_io selene_thread)

#------------------------------------------------------------------------------

# Installation

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

set(SELENE_INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/cmake/selene)

# Create selene-config-version.cmake
write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/selene-config-version.cmake
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY AnyNewerVersion)

# Create selene-config.cmake
configure_package_config_file(
        ${CMAKE_SOURCE_DIR}/cmake/selene-config.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/selene-config.cmake
        INSTALL_DESTINATION ${SELENE_INSTALL_CONFIGDIR})

# Install library files
install(TARGETS selene_base selene_img selene_io selene_thread selene
        EXPORT selene-targets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

# Install headers
install(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        FILES_MATCHING PATTERN "*.hpp")

# Install exports
install(EXPORT selene-targets
        FILE selene-targets.cmake
        NAMESPACE selene::
        DESTINATION ${SELENE_INSTALL_CONFIGDIR})

# Install config and config-version files
install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/selene-config.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/selene-config-version.cmake
        DESTINATION ${SELENE_INSTALL_CONFIGDIR})

#------------------------------------------------------------------------------

# Exporting from the build tree

export(EXPORT selene-targets
        FILE ${CMAKE_CURRENT_BINARY_DIR}/selene-targets.cmake
        NAMESPACE selene::)

export(PACKAGE selene)
