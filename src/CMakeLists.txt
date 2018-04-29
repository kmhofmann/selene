if(NOT IS_DIRECTORY ${SELENE_DIR})
    message(FATAL_ERROR "Please build using the outermost CMakeLists.txt file.")
endif()

#------------------------------------------------------------------------------

find_package(JPEG)
if (JPEG_FOUND)
    message("Building with libjpeg support.")
    # Detect some features provided by libjpeg-turbo: partial image decoding and extended colorspaces
    set(JPEG_PARTIAL_DECODING OFF)
    set(JPEG_EXTENDED_COLORSPACES OFF)
    if (JPEG_INCLUDE_DIR AND EXISTS "${JPEG_INCLUDE_DIR}/jpeglib.h")
        file(STRINGS "${JPEG_INCLUDE_DIR}/jpeglib.h" jpeg_supports_skip_scanlines REGEX "jpeg_skip_scanlines")
        file(STRINGS "${JPEG_INCLUDE_DIR}/jpeglib.h" jpeg_supports_crop_scanline REGEX "jpeg_crop_scanline")
        file(STRINGS "${JPEG_INCLUDE_DIR}/jpeglib.h" jpeg_supports_jcs_ext REGEX "JCS_EXT_")
        if (jpeg_supports_skip_scanlines AND jpeg_supports_crop_scanline)
            set(JPEG_PARTIAL_DECODING ON)
        endif()
        if (jpeg_supports_jcs_ext)
            set(JPEG_EXTENDED_COLORSPACES ON)
        endif()
        unset(jpeg_supports_skip_scanlines)
        unset(jpeg_supports_crop_scanline)
        unset(jpeg_supports_jcs_ext)
    endif()
    if (JPEG_PARTIAL_DECODING)
        message("libjpeg-turbo supports partial decoding.")
    endif()
    if (JPEG_EXTENDED_COLORSPACES)
        message("libjpeg-turbo supports extended color spaces.")
    endif()
else()
    message(WARNING "Building without libjpeg support.")
endif()

find_package(PNG)
if (PNG_FOUND)
    message("Building with libpng support.")
else()
    message(WARNING "Building without libpng support.")
endif()

find_package(OpenCV)
if (OPENCV_CORE_FOUND)
    message("Building with OpenCV support.")
else()
    message(WARNING "Building without OpenCV support.")
endif()

#------------------------------------------------------------------------------

add_library(selene_base
        ${CMAKE_CURRENT_LIST_DIR}/selene/base/Allocators.cpp
        ${CMAKE_CURRENT_LIST_DIR}/selene/base/Allocators.hpp
        ${CMAKE_CURRENT_LIST_DIR}/selene/base/Assert.hpp
        ${CMAKE_CURRENT_LIST_DIR}/selene/base/Bitcount.hpp
        ${CMAKE_CURRENT_LIST_DIR}/selene/base/MemoryBlock.hpp
        ${CMAKE_CURRENT_LIST_DIR}/selene/base/MessageLog.hpp
        ${CMAKE_CURRENT_LIST_DIR}/selene/base/ExplicitType.hpp
        ${CMAKE_CURRENT_LIST_DIR}/selene/base/Promote.hpp
        ${CMAKE_CURRENT_LIST_DIR}/selene/base/Round.hpp
        ${CMAKE_CURRENT_LIST_DIR}/selene/base/Types.hpp
        ${CMAKE_CURRENT_LIST_DIR}/selene/base/Utils.hpp
        )
add_library(selene::selene_base ALIAS selene_base)

target_compile_options(selene_base PRIVATE ${SELENE_COMPILER_OPTIONS})

target_compile_definitions(selene_base PRIVATE ${SELENE_COMPILER_DEFINITIONS})

target_include_directories(selene_base PUBLIC
        $<BUILD_INTERFACE:${SELENE_DIR}/src>
        $<INSTALL_INTERFACE:include>)

#------------------------------------------------------------------------------

add_library(selene_img
        ${CMAKE_CURRENT_LIST_DIR}/selene/img/Algorithms.hpp
        ${CMAKE_CURRENT_LIST_DIR}/selene/img/BorderAccessors.hpp
        ${CMAKE_CURRENT_LIST_DIR}/selene/img/BoundingBox.hpp
        ${CMAKE_CURRENT_LIST_DIR}/selene/img/Image.hpp
        ${CMAKE_CURRENT_LIST_DIR}/selene/img/ImageAccess.hpp
        ${CMAKE_CURRENT_LIST_DIR}/selene/img/ImageConversions.hpp
        ${CMAKE_CURRENT_LIST_DIR}/selene/img/ImageData.hpp
        ${CMAKE_CURRENT_LIST_DIR}/selene/img/ImageDataBase.hpp
        ${CMAKE_CURRENT_LIST_DIR}/selene/img/ImageDataStorage.hpp
        ${CMAKE_CURRENT_LIST_DIR}/selene/img/ImageDataToImage.hpp
        ${CMAKE_CURRENT_LIST_DIR}/selene/img/ImageToImageData.hpp
        ${CMAKE_CURRENT_LIST_DIR}/selene/img/Interpolators.hpp
        ${CMAKE_CURRENT_LIST_DIR}/selene/img/IO.cpp
        ${CMAKE_CURRENT_LIST_DIR}/selene/img/IO.hpp
        ${CMAKE_CURRENT_LIST_DIR}/selene/img/JPEGCommon.hpp
        ${CMAKE_CURRENT_LIST_DIR}/selene/img/JPEGRead.cpp
        ${CMAKE_CURRENT_LIST_DIR}/selene/img/JPEGRead.hpp
        ${CMAKE_CURRENT_LIST_DIR}/selene/img/JPEGWrite.cpp
        ${CMAKE_CURRENT_LIST_DIR}/selene/img/JPEGWrite.hpp
        ${CMAKE_CURRENT_LIST_DIR}/selene/img/OpenCV.hpp
        ${CMAKE_CURRENT_LIST_DIR}/selene/img/Pixel.hpp
        ${CMAKE_CURRENT_LIST_DIR}/selene/img/PixelConversions.cpp
        ${CMAKE_CURRENT_LIST_DIR}/selene/img/PixelConversions.hpp
        ${CMAKE_CURRENT_LIST_DIR}/selene/img/PixelFormat.cpp
        ${CMAKE_CURRENT_LIST_DIR}/selene/img/PixelFormat.hpp
        ${CMAKE_CURRENT_LIST_DIR}/selene/img/PixelTraits.hpp
        ${CMAKE_CURRENT_LIST_DIR}/selene/img/PNGRead.cpp
        ${CMAKE_CURRENT_LIST_DIR}/selene/img/PNGRead.hpp
        ${CMAKE_CURRENT_LIST_DIR}/selene/img/PNGWrite.cpp
        ${CMAKE_CURRENT_LIST_DIR}/selene/img/PNGWrite.hpp
        ${CMAKE_CURRENT_LIST_DIR}/selene/img/RelativeAccessor.hpp
        ${CMAKE_CURRENT_LIST_DIR}/selene/img/Resample.hpp
        ${CMAKE_CURRENT_LIST_DIR}/selene/img/RowPointers.hpp
        ${CMAKE_CURRENT_LIST_DIR}/selene/img/Transformations.hpp
        ${CMAKE_CURRENT_LIST_DIR}/selene/img/Types.hpp
        ${CMAKE_CURRENT_LIST_DIR}/selene/img/detail/JPEGCommon.hpp
        ${CMAKE_CURRENT_LIST_DIR}/selene/img/detail/JPEGDetail.cpp
        ${CMAKE_CURRENT_LIST_DIR}/selene/img/detail/JPEGDetail.hpp
        ${CMAKE_CURRENT_LIST_DIR}/selene/img/detail/PNGDetail.cpp
        ${CMAKE_CURRENT_LIST_DIR}/selene/img/detail/PNGDetail.hpp
        ${CMAKE_CURRENT_LIST_DIR}/selene/img/detail/Util.hpp
        )
add_library(selene::selene_img ALIAS selene_img)

target_compile_options(selene_img PRIVATE ${SELENE_COMPILER_OPTIONS} ${SELENE_IMG_COMPILER_OPTIONS})

target_compile_definitions(selene_img PRIVATE ${SELENE_COMPILER_DEFINITIONS})

target_include_directories(selene_img PUBLIC
        $<BUILD_INTERFACE:${SELENE_DIR}/src>
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
endif()

if (PNG_FOUND)
    target_compile_definitions(selene_img PUBLIC SELENE_WITH_LIBPNG)
    target_include_directories(selene_img PUBLIC ${PNG_INCLUDE_DIR})
    target_link_libraries(selene_img PUBLIC ${PNG_LIBRARIES})
endif()

if (OPENCV_CORE_FOUND)
    target_compile_definitions(selene_img PUBLIC SELENE_WITH_OPENCV)
    target_include_directories(selene_img PUBLIC ${OpenCV_INCLUDE_DIRS})
    target_link_libraries(selene_img PUBLIC ${OpenCV_LIBS})
endif()

target_link_libraries(selene_img PUBLIC selene_base)

#------------------------------------------------------------------------------

add_library(selene_io
        ${CMAKE_CURRENT_LIST_DIR}/selene/io/FileReader.hpp
        ${CMAKE_CURRENT_LIST_DIR}/selene/io/FileUtils.cpp
        ${CMAKE_CURRENT_LIST_DIR}/selene/io/FileUtils.hpp
        ${CMAKE_CURRENT_LIST_DIR}/selene/io/FileWriter.hpp
        ${CMAKE_CURRENT_LIST_DIR}/selene/io/MemoryReader.hpp
        ${CMAKE_CURRENT_LIST_DIR}/selene/io/MemoryWriter.hpp
        ${CMAKE_CURRENT_LIST_DIR}/selene/io/VectorReader.hpp
        ${CMAKE_CURRENT_LIST_DIR}/selene/io/VectorWriter.hpp
        ${CMAKE_CURRENT_LIST_DIR}/selene/io/WriterMode.hpp
        )
add_library(selene::selene_io ALIAS selene_io)

target_compile_options(selene_io PRIVATE ${SELENE_COMPILER_OPTIONS})

target_compile_definitions(selene_io PRIVATE ${SELENE_COMPILER_DEFINITIONS})

target_include_directories(selene_io PUBLIC
        $<BUILD_INTERFACE:${SELENE_DIR}/src>
        $<INSTALL_INTERFACE:include>)

#------------------------------------------------------------------------------

add_library(selene_thread INTERFACE)
add_library(selene::selene_thread ALIAS selene_thread)

target_sources(selene_thread INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/selene/thread/ThreadPool.hpp>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/selene/thread/detail/Callable.hpp>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/selene/thread/detail/TaskQueue.hpp>
        )

target_include_directories(selene_thread INTERFACE
        $<BUILD_INTERFACE:${SELENE_DIR}/src>
        $<INSTALL_INTERFACE:include>)

#------------------------------------------------------------------------------

add_library(selene INTERFACE)
add_library(selene::selene ALIAS selene)

target_include_directories(selene INTERFACE
        $<BUILD_INTERFACE:${SELENE_DIR}/src>
        $<INSTALL_INTERFACE:include>)

target_link_libraries(selene INTERFACE selene_base selene_img selene_io selene_thread)

#------------------------------------------------------------------------------

# Installation

include(CMakePackageConfigHelpers)

set(SELENE_INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/cmake/selene)

# Install library files
install(TARGETS selene_base selene_img selene_io selene_thread selene
        EXPORT selene-targets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

# Install headers
install(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/selene
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        FILES_MATCHING PATTERN "*.hpp")

# Install exports
install(EXPORT selene-targets
        FILE selene-targets.cmake
        NAMESPACE selene::
        DESTINATION ${SELENE_INSTALL_CONFIGDIR})

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