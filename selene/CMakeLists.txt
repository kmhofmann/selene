if(NOT IS_DIRECTORY ${SELENE_DIR})
    message(FATAL_ERROR "Please build using the outermost CMakeLists.txt file.")
endif()

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

#------------------------------------------------------------------------------

add_library(selene_base "")
add_library(selene::selene_base ALIAS selene_base)

target_sources(selene_base PRIVATE
        base/Assert.hpp
        base/Bitcount.hpp
        base/Kernel.hpp
        base/MemoryBlock.hpp
        base/MessageLog.cpp
        base/MessageLog.hpp
        base/Promote.hpp
        base/Round.hpp
        base/Types.hpp
        base/Utils.hpp
        base/_impl/CompressedPair.hpp
        base/_impl/ExplicitType.hpp
        base/_impl/TypeTraits.hpp
        base/_impl/Utils.cpp
        base/_impl/Utils.hpp
        )

target_compile_options(selene_base PRIVATE ${SELENE_COMPILER_OPTIONS})

target_compile_definitions(selene_base PRIVATE ${SELENE_COMPILER_DEFINITIONS})

if(SELENE_USE_DEFAULT_SINGLE_PRECISION)
    target_compile_definitions(selene_base PUBLIC SELENE_DEFAULT_SCALAR_SINGLE_PRECISION)
endif()

target_include_directories(selene_base PUBLIC
        $<BUILD_INTERFACE:${SELENE_DIR}>
        $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

set(SELENE_INSTALL_TARGETS ${SELENE_INSTALL_TARGETS} selene_base)

#------------------------------------------------------------------------------

add_library(selene_base_io "")
add_library(selene::selene_base_io ALIAS selene_base_io)

target_sources(selene_base_io PRIVATE
        base/io/FileReader.hpp
        base/io/FileUtils.cpp
        base/io/FileUtils.hpp
        base/io/FileWriter.hpp
        base/io/MemoryReader.hpp
        base/io/MemoryRegion.hpp
        base/io/MemoryWriter.hpp
        base/io/VectorReader.hpp
        base/io/VectorWriter.hpp
        base/io/WriterMode.hpp
        )

target_compile_options(selene_base_io PRIVATE ${SELENE_COMPILER_OPTIONS})

target_compile_definitions(selene_base_io PRIVATE ${SELENE_COMPILER_DEFINITIONS})

target_include_directories(selene_base_io PUBLIC
        $<BUILD_INTERFACE:${SELENE_DIR}>
        $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

target_link_libraries(selene_base_io PUBLIC selene_base)

set(SELENE_INSTALL_TARGETS ${SELENE_INSTALL_TARGETS} selene_base_io)

#------------------------------------------------------------------------------

add_library(selene_img "")
add_library(selene::selene_img ALIAS selene_img)

target_sources(selene_img PRIVATE
        img/common/BoundingBox.hpp
        img/common/DataPtr.hpp
        img/common/Iterators.hpp
        img/common/PixelFormat.cpp
        img/common/PixelFormat.hpp
        img/common/Types.hpp

        img/pixel/Pixel.hpp
        img/pixel/PixelTraits.hpp
        img/pixel/PixelTypeAliases.hpp

        img/dynamic/DynImage.hpp
        img/dynamic/DynImageView.hpp
        img/dynamic/UntypedLayout.hpp
        img/dynamic/_impl/DynImageFwd.hpp
        img/dynamic/_impl/RuntimeChecks.hpp
        img/dynamic/_impl/StaticChecks.hpp
        img/dynamic/_impl/Utils.hpp

        img/typed/Image.hpp
        img/typed/ImageBase.hpp
        img/typed/ImageTypeAliases.hpp
        img/typed/ImageView.hpp
        img/typed/ImageViewTypeAliases.hpp
        img/typed/TypedLayout.hpp
        img/typed/Utilities.hpp
        img/typed/_impl/ImageBaseTraits.hpp
        img/typed/_impl/ImageFwd.hpp
        img/typed/_impl/StaticChecks.hpp

        img/typed/access/BorderAccessors.hpp
        img/typed/access/GetPixel.hpp
        img/typed/access/Interpolators.hpp
        img/typed/access/RelativeAccessor.hpp

        img/interop/DynImageToImage.hpp
        img/interop/ImageToDynImage.hpp
        img/interop/OpenCV.hpp
        )

target_compile_options(selene_img PRIVATE ${SELENE_COMPILER_OPTIONS} ${SELENE_IMG_COMPILER_OPTIONS})

target_compile_definitions(selene_img PRIVATE ${SELENE_COMPILER_DEFINITIONS})

target_include_directories(selene_img PUBLIC
        $<BUILD_INTERFACE:${SELENE_DIR}>
        $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

target_link_libraries(selene_img PUBLIC selene_base)

set(SELENE_INSTALL_TARGETS ${SELENE_INSTALL_TARGETS} selene_img)

#------------------------------------------------------------------------------

if(JPEG_FOUND)

    add_library(selene_img_io_jpeg "")
    add_library(selene::selene_img_io_jpeg ALIAS selene_img_io_jpeg)

    target_sources(selene_img_io_jpeg PRIVATE
            img_io/jpeg/Common.hpp
            img_io/jpeg/Read.cpp
            img_io/jpeg/Read.hpp
            img_io/jpeg/Write.cpp
            img_io/jpeg/Write.hpp
            img_io/jpeg/_impl/Common.hpp
            img_io/jpeg/_impl/Detail.cpp
            img_io/jpeg/_impl/Detail.hpp
            )

    target_compile_options(selene_img_io_jpeg PRIVATE ${SELENE_COMPILER_OPTIONS} ${SELENE_IMG_COMPILER_OPTIONS})

    target_compile_definitions(selene_img_io_jpeg PRIVATE ${SELENE_COMPILER_DEFINITIONS})

    target_include_directories(selene_img_io_jpeg PUBLIC
            $<BUILD_INTERFACE:${SELENE_DIR}>
            $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}>
            $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

    target_link_libraries(selene_img_io_jpeg PUBLIC JPEG::JPEG)
    target_link_libraries(selene_img_io_jpeg PUBLIC selene_base_io selene_img)

    set(SELENE_INSTALL_TARGETS ${SELENE_INSTALL_TARGETS} selene_img_io_jpeg)

endif()

#------------------------------------------------------------------------------

if(PNG_FOUND)

    add_library(selene_img_io_png "")
    add_library(selene::selene_img_io_png ALIAS selene_img_io_png)

    target_sources(selene_img_io_png PRIVATE
            img_io/png/Read.cpp
            img_io/png/Read.hpp
            img_io/png/Write.cpp
            img_io/png/Write.hpp
            img_io/png/_impl/Detail.cpp
            img_io/png/_impl/Detail.hpp
            )

    target_compile_options(selene_img_io_png PRIVATE ${SELENE_COMPILER_OPTIONS} ${SELENE_IMG_COMPILER_OPTIONS})

    target_compile_definitions(selene_img_io_png PRIVATE ${SELENE_COMPILER_DEFINITIONS})

    target_include_directories(selene_img_io_png PUBLIC
            $<BUILD_INTERFACE:${SELENE_DIR}>
            $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}>
            $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

    target_link_libraries(selene_img_io_png PUBLIC PNG::PNG)
    target_link_libraries(selene_img_io_png PUBLIC selene_base_io selene_img)

    set(SELENE_INSTALL_TARGETS ${SELENE_INSTALL_TARGETS} selene_img_io_png)

endif()

#------------------------------------------------------------------------------

if(TIFF_FOUND)

    add_library(selene_img_io_tiff "")
    add_library(selene::selene_img_io_tiff ALIAS selene_img_io_tiff)

    target_sources(selene_img_io_tiff PRIVATE
            img_io/tiff/Common.cpp
            img_io/tiff/Common.hpp
            img_io/tiff/Read.cpp
            img_io/tiff/Read.hpp
            img_io/tiff/Write.cpp
            img_io/tiff/Write.cpp
            img_io/tiff/_impl/TIFFDetail.cpp
            img_io/tiff/_impl/TIFFDetail.hpp
            img_io/tiff/_impl/TIFFIOFunctions.hpp
            img_io/tiff/_impl/TIFFReadHighLevel.cpp
            img_io/tiff/_impl/TIFFReadHighLevel.hpp
            img_io/tiff/_impl/TIFFReadStrips.cpp
            img_io/tiff/_impl/TIFFReadStrips.hpp
            img_io/tiff/_impl/TIFFReadTiles.cpp
            img_io/tiff/_impl/TIFFReadTiles.hpp
            )

    target_compile_options(selene_img_io_tiff PRIVATE ${SELENE_COMPILER_OPTIONS} ${SELENE_IMG_COMPILER_OPTIONS})

    target_compile_definitions(selene_img_io_tiff PRIVATE ${SELENE_COMPILER_DEFINITIONS})

    target_include_directories(selene_img_io_tiff PUBLIC
            $<BUILD_INTERFACE:${SELENE_DIR}>
            $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}>
            $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

    target_link_libraries(selene_img_io_tiff PUBLIC TIFF::TIFF)
    target_link_libraries(selene_img_io_tiff PUBLIC selene_base_io selene_img)

    set(SELENE_INSTALL_TARGETS ${SELENE_INSTALL_TARGETS} selene_img_io_tiff)

endif()

#------------------------------------------------------------------------------

if(JPEG_FOUND OR PNG_FOUND OR TIFF_FOUND)

    add_library(selene_img_io "")
    add_library(selene::selene_img_io ALIAS selene_img_io)

    target_sources(selene_img_io PRIVATE
            img_io/IO.cpp
            img_io/IO.hpp
            img_io/_impl/Util.hpp
            )

    target_compile_options(selene_img_io PRIVATE ${SELENE_COMPILER_OPTIONS} ${SELENE_IMG_COMPILER_OPTIONS})

    target_compile_definitions(selene_img_io PRIVATE ${SELENE_COMPILER_DEFINITIONS})

    target_include_directories(selene_img_io PUBLIC
            $<BUILD_INTERFACE:${SELENE_DIR}>
            $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}>
            $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

    target_link_libraries(selene_img_io PUBLIC selene_base_io selene_img)
    if(JPEG_FOUND)
        target_link_libraries(selene_img_io PUBLIC selene_img_io_jpeg)
    endif()
    if(PNG_FOUND)
        target_link_libraries(selene_img_io PUBLIC selene_img_io_png)
    endif()
    if(TIFF_FOUND)
        target_link_libraries(selene_img_io PUBLIC selene_img_io_tiff)
    endif()

    set(SELENE_INSTALL_TARGETS ${SELENE_INSTALL_TARGETS} selene_img_io)

endif()

#------------------------------------------------------------------------------

add_library(selene_img_ops "")
add_library(selene::selene_img_ops ALIAS selene_img_ops)

target_sources(selene_img_ops PRIVATE
        img_ops/Algorithms.hpp
        img_ops/Allocate.hpp
        img_ops/ChannelOperations.hpp
        img_ops/Clone.hpp
        img_ops/Convolution.hpp
        img_ops/Crop.hpp
        img_ops/DynView.hpp
        img_ops/Fill.hpp
        img_ops/ImageConversions.hpp
        img_ops/PixelConversions.cpp
        img_ops/PixelConversions.hpp
        img_ops/Resample.hpp
        img_ops/Transformations.hpp
        img_ops/View.hpp
        )

target_compile_options(selene_img_ops PRIVATE ${SELENE_COMPILER_OPTIONS} ${SELENE_IMG_COMPILER_OPTIONS})

target_compile_definitions(selene_img_ops PRIVATE ${SELENE_COMPILER_DEFINITIONS})

target_include_directories(selene_img_ops PUBLIC
        $<BUILD_INTERFACE:${SELENE_DIR}>
        $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

target_link_libraries(selene_img_ops PUBLIC selene_img)

set(SELENE_INSTALL_TARGETS ${SELENE_INSTALL_TARGETS} selene_img_ops)

#------------------------------------------------------------------------------

add_library(selene INTERFACE)
add_library(selene::selene ALIAS selene)

target_include_directories(selene INTERFACE
        $<BUILD_INTERFACE:${SELENE_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

target_link_libraries(selene INTERFACE
        selene_base
        selene_base_io
        selene_img
        selene_img_ops)

if(JPEG_FOUND)
    target_link_libraries(selene INTERFACE selene_img_io_jpeg)
endif()

if(PNG_FOUND)
    target_link_libraries(selene INTERFACE selene_img_io_png)
endif()

if(TIFF_FOUND)
    target_link_libraries(selene INTERFACE selene_img_io_tiff)
endif()

if(JPEG_FOUND OR PNG_FOUND OR TIFF_FOUND)
    target_link_libraries(selene INTERFACE selene_img_io)
endif()

set(SELENE_INSTALL_TARGETS ${SELENE_INSTALL_TARGETS} selene)

#------------------------------------------------------------------------------

# Write configure file

configure_file(${CMAKE_SOURCE_DIR}/cmake/selene_config.hpp.in selene_config.hpp)

#------------------------------------------------------------------------------

# Installation

set(SELENE_INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/cmake/selene)

# Create selene-config-version.cmake
write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/selene-config-version.cmake
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY AnyNewerVersion)

# Create selene-config.cmake
configure_package_config_file(
        ${SELENE_DIR}/cmake/selene-config.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/selene-config.cmake
        INSTALL_DESTINATION ${SELENE_INSTALL_CONFIGDIR})

# Install library files
install(TARGETS ${SELENE_INSTALL_TARGETS}
        EXPORT selene-targets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# Install headers
install(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        FILES_MATCHING PATTERN "*.hpp"
                       PATTERN "_doc.hpp" EXCLUDE)

# Install config file
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/selene_config.hpp
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/selene)

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
