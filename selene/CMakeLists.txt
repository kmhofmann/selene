if(NOT IS_DIRECTORY ${SELENE_DIR})
    message(FATAL_ERROR "Please build using the outermost CMakeLists.txt file.")
endif()

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

#------------------------------------------------------------------------------

# Write configure files

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/selene_config.hpp.in selene_config.hpp)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/selene_version.hpp.in selene_version.hpp)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/selene_version.cpp.in selene_version.cpp)

#------------------------------------------------------------------------------

add_library(selene_base "")
add_library(selene::selene_base ALIAS selene_base)

target_sources(selene_base PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR}/selene_config.hpp
        ${CMAKE_CURRENT_BINARY_DIR}/selene_version.hpp
        ${CMAKE_CURRENT_BINARY_DIR}/selene_version.cpp
        ${CMAKE_CURRENT_LIST_DIR}/base/Assert.hpp
        ${CMAKE_CURRENT_LIST_DIR}/base/Bitcount.hpp
        ${CMAKE_CURRENT_LIST_DIR}/base/Kernel.hpp
        ${CMAKE_CURRENT_LIST_DIR}/base/MemoryBlock.hpp
        ${CMAKE_CURRENT_LIST_DIR}/base/MessageLog.cpp
        ${CMAKE_CURRENT_LIST_DIR}/base/MessageLog.hpp
        ${CMAKE_CURRENT_LIST_DIR}/base/Promote.hpp
        ${CMAKE_CURRENT_LIST_DIR}/base/Round.hpp
        ${CMAKE_CURRENT_LIST_DIR}/base/Types.hpp
        ${CMAKE_CURRENT_LIST_DIR}/base/Utils.hpp
        ${CMAKE_CURRENT_LIST_DIR}/base/_impl/CompressedPair.hpp
        ${CMAKE_CURRENT_LIST_DIR}/base/_impl/ExplicitType.hpp
        ${CMAKE_CURRENT_LIST_DIR}/base/_impl/TypeTraits.hpp
        ${CMAKE_CURRENT_LIST_DIR}/base/_impl/Utils.cpp
        ${CMAKE_CURRENT_LIST_DIR}/base/_impl/Utils.hpp
        )

target_compile_options(selene_base PRIVATE ${SELENE_COMPILE_OPTIONS})

target_compile_definitions(selene_base PRIVATE ${SELENE_COMPILE_DEFINITIONS})

if(SELENE_USE_DEFAULT_SINGLE_PRECISION)
    target_compile_definitions(selene_base PUBLIC SELENE_DEFAULT_SCALAR_SINGLE_PRECISION)
endif()

target_include_directories(selene_base PUBLIC
        $<BUILD_INTERFACE:${SELENE_DIR}>
        $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}>)

set(SELENE_INSTALL_TARGETS ${SELENE_INSTALL_TARGETS} selene_base)

#------------------------------------------------------------------------------

add_library(selene_base_io "")
add_library(selene::selene_base_io ALIAS selene_base_io)

target_sources(selene_base_io PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/base/io/FileReader.hpp
        ${CMAKE_CURRENT_LIST_DIR}/base/io/FileUtils.cpp
        ${CMAKE_CURRENT_LIST_DIR}/base/io/FileUtils.hpp
        ${CMAKE_CURRENT_LIST_DIR}/base/io/FileWriter.hpp
        ${CMAKE_CURRENT_LIST_DIR}/base/io/MemoryReader.hpp
        ${CMAKE_CURRENT_LIST_DIR}/base/io/MemoryRegion.hpp
        ${CMAKE_CURRENT_LIST_DIR}/base/io/MemoryWriter.hpp
        ${CMAKE_CURRENT_LIST_DIR}/base/io/VectorReader.hpp
        ${CMAKE_CURRENT_LIST_DIR}/base/io/VectorWriter.hpp
        ${CMAKE_CURRENT_LIST_DIR}/base/io/WriterMode.hpp
        )

target_compile_options(selene_base_io PRIVATE ${SELENE_COMPILE_OPTIONS})
target_compile_definitions(selene_base_io PRIVATE ${SELENE_COMPILE_DEFINITIONS})

target_link_libraries(selene_base_io PUBLIC selene_base)

set(SELENE_INSTALL_TARGETS ${SELENE_INSTALL_TARGETS} selene_base_io)

#------------------------------------------------------------------------------

add_library(selene_img "")
add_library(selene::selene_img ALIAS selene_img)

target_sources(selene_img PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/img/common/BoundingBox.hpp
        ${CMAKE_CURRENT_LIST_DIR}/img/common/DataPtr.hpp
        ${CMAKE_CURRENT_LIST_DIR}/img/common/Iterators.hpp
        ${CMAKE_CURRENT_LIST_DIR}/img/common/PixelFormat.cpp
        ${CMAKE_CURRENT_LIST_DIR}/img/common/PixelFormat.hpp
        ${CMAKE_CURRENT_LIST_DIR}/img/common/Types.hpp

        ${CMAKE_CURRENT_LIST_DIR}/img/pixel/Pixel.hpp
        ${CMAKE_CURRENT_LIST_DIR}/img/pixel/PixelTraits.hpp
        ${CMAKE_CURRENT_LIST_DIR}/img/pixel/PixelTypeAliases.hpp

        ${CMAKE_CURRENT_LIST_DIR}/img/dynamic/DynImage.hpp
        ${CMAKE_CURRENT_LIST_DIR}/img/dynamic/DynImageView.hpp
        ${CMAKE_CURRENT_LIST_DIR}/img/dynamic/UntypedLayout.hpp
        ${CMAKE_CURRENT_LIST_DIR}/img/dynamic/_impl/DynImageFwd.hpp
        ${CMAKE_CURRENT_LIST_DIR}/img/dynamic/_impl/RuntimeChecks.hpp
        ${CMAKE_CURRENT_LIST_DIR}/img/dynamic/_impl/StaticChecks.hpp
        ${CMAKE_CURRENT_LIST_DIR}/img/dynamic/_impl/Utils.hpp

        ${CMAKE_CURRENT_LIST_DIR}/img/typed/Image.hpp
        ${CMAKE_CURRENT_LIST_DIR}/img/typed/ImageBase.hpp
        ${CMAKE_CURRENT_LIST_DIR}/img/typed/ImageTypeAliases.hpp
        ${CMAKE_CURRENT_LIST_DIR}/img/typed/ImageView.hpp
        ${CMAKE_CURRENT_LIST_DIR}/img/typed/ImageViewTypeAliases.hpp
        ${CMAKE_CURRENT_LIST_DIR}/img/typed/TypedLayout.hpp
        ${CMAKE_CURRENT_LIST_DIR}/img/typed/Utilities.hpp
        ${CMAKE_CURRENT_LIST_DIR}/img/typed/_impl/ImageExprTraits.hpp
        ${CMAKE_CURRENT_LIST_DIR}/img/typed/_impl/ImageFwd.hpp
        ${CMAKE_CURRENT_LIST_DIR}/img/typed/_impl/StaticChecks.hpp

        ${CMAKE_CURRENT_LIST_DIR}/img/typed/access/BorderAccessors.hpp
        ${CMAKE_CURRENT_LIST_DIR}/img/typed/access/GetPixel.hpp
        ${CMAKE_CURRENT_LIST_DIR}/img/typed/access/Interpolators.hpp
        ${CMAKE_CURRENT_LIST_DIR}/img/typed/access/RelativeAccessor.hpp

        ${CMAKE_CURRENT_LIST_DIR}/img/interop/DynImageToImage.hpp
        ${CMAKE_CURRENT_LIST_DIR}/img/interop/ImageToDynImage.hpp
        ${CMAKE_CURRENT_LIST_DIR}/img/interop/OpenCV.hpp
        )

target_compile_options(selene_img PRIVATE ${SELENE_COMPILE_OPTIONS} ${SELENE_IMG_COMPILE_OPTIONS})
target_compile_definitions(selene_img PRIVATE ${SELENE_COMPILE_DEFINITIONS})

target_link_libraries(selene_img PUBLIC selene_base)

set(SELENE_INSTALL_TARGETS ${SELENE_INSTALL_TARGETS} selene_img)

#------------------------------------------------------------------------------

if(JPEG_FOUND)

    add_library(selene_img_io_jpeg "")
    add_library(selene::selene_img_io_jpeg ALIAS selene_img_io_jpeg)

    target_sources(selene_img_io_jpeg PRIVATE
            ${CMAKE_CURRENT_LIST_DIR}/img_io/jpeg/Common.hpp
            ${CMAKE_CURRENT_LIST_DIR}/img_io/jpeg/Read.cpp
            ${CMAKE_CURRENT_LIST_DIR}/img_io/jpeg/Read.hpp
            ${CMAKE_CURRENT_LIST_DIR}/img_io/jpeg/Write.cpp
            ${CMAKE_CURRENT_LIST_DIR}/img_io/jpeg/Write.hpp
            ${CMAKE_CURRENT_LIST_DIR}/img_io/jpeg/_impl/Common.hpp
            ${CMAKE_CURRENT_LIST_DIR}/img_io/jpeg/_impl/Detail.cpp
            ${CMAKE_CURRENT_LIST_DIR}/img_io/jpeg/_impl/Detail.hpp
            )

    target_compile_options(selene_img_io_jpeg PRIVATE ${SELENE_COMPILE_OPTIONS} ${SELENE_IMG_COMPILE_OPTIONS})
    target_compile_definitions(selene_img_io_jpeg PRIVATE ${SELENE_COMPILE_DEFINITIONS})

    target_link_libraries(selene_img_io_jpeg PUBLIC selene_base_io selene_img JPEG::JPEG)

    set(SELENE_INSTALL_TARGETS ${SELENE_INSTALL_TARGETS} selene_img_io_jpeg)

endif()

#------------------------------------------------------------------------------

if(PNG_FOUND)

    add_library(selene_img_io_png "")
    add_library(selene::selene_img_io_png ALIAS selene_img_io_png)

    target_sources(selene_img_io_png PRIVATE
            ${CMAKE_CURRENT_LIST_DIR}/img_io/png/Read.cpp
            ${CMAKE_CURRENT_LIST_DIR}/img_io/png/Read.hpp
            ${CMAKE_CURRENT_LIST_DIR}/img_io/png/Write.cpp
            ${CMAKE_CURRENT_LIST_DIR}/img_io/png/Write.hpp
            ${CMAKE_CURRENT_LIST_DIR}/img_io/png/_impl/Detail.cpp
            ${CMAKE_CURRENT_LIST_DIR}/img_io/png/_impl/Detail.hpp
            )

    target_compile_options(selene_img_io_png PRIVATE ${SELENE_COMPILE_OPTIONS} ${SELENE_IMG_COMPILE_OPTIONS})
    target_compile_definitions(selene_img_io_png PRIVATE ${SELENE_COMPILE_DEFINITIONS})

    target_link_libraries(selene_img_io_png PUBLIC selene_base_io selene_img PNG::PNG)

    set(SELENE_INSTALL_TARGETS ${SELENE_INSTALL_TARGETS} selene_img_io_png)

endif()

#------------------------------------------------------------------------------

if(TIFF_FOUND)

    add_library(selene_img_io_tiff "")
    add_library(selene::selene_img_io_tiff ALIAS selene_img_io_tiff)

    target_sources(selene_img_io_tiff PRIVATE
            ${CMAKE_CURRENT_LIST_DIR}/img_io/tiff/Common.cpp
            ${CMAKE_CURRENT_LIST_DIR}/img_io/tiff/Common.hpp
            ${CMAKE_CURRENT_LIST_DIR}/img_io/tiff/Read.cpp
            ${CMAKE_CURRENT_LIST_DIR}/img_io/tiff/Read.hpp
            ${CMAKE_CURRENT_LIST_DIR}/img_io/tiff/Write.cpp
            ${CMAKE_CURRENT_LIST_DIR}/img_io/tiff/Write.cpp
            ${CMAKE_CURRENT_LIST_DIR}/img_io/tiff/_impl/TIFFDetail.cpp
            ${CMAKE_CURRENT_LIST_DIR}/img_io/tiff/_impl/TIFFDetail.hpp
            ${CMAKE_CURRENT_LIST_DIR}/img_io/tiff/_impl/TIFFIOFunctions.hpp
            ${CMAKE_CURRENT_LIST_DIR}/img_io/tiff/_impl/TIFFReadHighLevel.cpp
            ${CMAKE_CURRENT_LIST_DIR}/img_io/tiff/_impl/TIFFReadHighLevel.hpp
            ${CMAKE_CURRENT_LIST_DIR}/img_io/tiff/_impl/TIFFReadStrips.cpp
            ${CMAKE_CURRENT_LIST_DIR}/img_io/tiff/_impl/TIFFReadStrips.hpp
            ${CMAKE_CURRENT_LIST_DIR}/img_io/tiff/_impl/TIFFReadTiles.cpp
            ${CMAKE_CURRENT_LIST_DIR}/img_io/tiff/_impl/TIFFReadTiles.hpp
            )

    target_compile_options(selene_img_io_tiff PRIVATE ${SELENE_COMPILE_OPTIONS} ${SELENE_IMG_COMPILE_OPTIONS})
    target_compile_definitions(selene_img_io_tiff PRIVATE ${SELENE_COMPILE_DEFINITIONS})

    target_link_libraries(selene_img_io_tiff PUBLIC selene_base_io selene_img TIFF::TIFF)

    set(SELENE_INSTALL_TARGETS ${SELENE_INSTALL_TARGETS} selene_img_io_tiff)

endif()

#------------------------------------------------------------------------------

if(JPEG_FOUND OR PNG_FOUND OR TIFF_FOUND)

    add_library(selene_img_io "")
    add_library(selene::selene_img_io ALIAS selene_img_io)

    target_sources(selene_img_io PRIVATE
            ${CMAKE_CURRENT_LIST_DIR}/img_io/IO.cpp
            ${CMAKE_CURRENT_LIST_DIR}/img_io/IO.hpp
            ${CMAKE_CURRENT_LIST_DIR}/img_io/_impl/Util.hpp
            )

    target_compile_options(selene_img_io PRIVATE ${SELENE_COMPILE_OPTIONS} ${SELENE_IMG_COMPILE_OPTIONS})
    target_compile_definitions(selene_img_io PRIVATE ${SELENE_COMPILE_DEFINITIONS})

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
        ${CMAKE_CURRENT_LIST_DIR}/img_ops/Algorithms.hpp
        ${CMAKE_CURRENT_LIST_DIR}/img_ops/Allocate.hpp
        ${CMAKE_CURRENT_LIST_DIR}/img_ops/ChannelOperations.hpp
        ${CMAKE_CURRENT_LIST_DIR}/img_ops/Clone.hpp
        ${CMAKE_CURRENT_LIST_DIR}/img_ops/Convolution.hpp
        ${CMAKE_CURRENT_LIST_DIR}/img_ops/Crop.hpp
        ${CMAKE_CURRENT_LIST_DIR}/img_ops/DynView.hpp
        ${CMAKE_CURRENT_LIST_DIR}/img_ops/Fill.hpp
        ${CMAKE_CURRENT_LIST_DIR}/img_ops/ImageConversions.hpp
        ${CMAKE_CURRENT_LIST_DIR}/img_ops/PixelConversions.cpp
        ${CMAKE_CURRENT_LIST_DIR}/img_ops/PixelConversions.hpp
        ${CMAKE_CURRENT_LIST_DIR}/img_ops/Resample.hpp
        ${CMAKE_CURRENT_LIST_DIR}/img_ops/Transformations.hpp
        ${CMAKE_CURRENT_LIST_DIR}/img_ops/TransformationDirections.hpp
        ${CMAKE_CURRENT_LIST_DIR}/img_ops/View.hpp
        ${CMAKE_CURRENT_LIST_DIR}/img_ops/_impl/CropExpr.hpp
        ${CMAKE_CURRENT_LIST_DIR}/img_ops/_impl/FlipExpr.hpp
        ${CMAKE_CURRENT_LIST_DIR}/img_ops/_impl/IdentityExpr.hpp
        ${CMAKE_CURRENT_LIST_DIR}/img_ops/_impl/ImageConversionAlphaExpr.hpp
        ${CMAKE_CURRENT_LIST_DIR}/img_ops/_impl/ImageConversionExpr.hpp
        ${CMAKE_CURRENT_LIST_DIR}/img_ops/_impl/TransformExpr.hpp
        ${CMAKE_CURRENT_LIST_DIR}/img_ops/_impl/TransformWithPositionExpr.hpp
        ${CMAKE_CURRENT_LIST_DIR}/img_ops/_impl/TransposeExpr.hpp
        )

target_compile_options(selene_img_ops PRIVATE ${SELENE_COMPILE_OPTIONS} ${SELENE_IMG_COMPILE_OPTIONS})
target_compile_definitions(selene_img_ops PRIVATE ${SELENE_COMPILE_DEFINITIONS})

target_link_libraries(selene_img_ops PUBLIC selene_img)

set(SELENE_INSTALL_TARGETS ${SELENE_INSTALL_TARGETS} selene_img_ops)

#------------------------------------------------------------------------------

add_library(selene INTERFACE)
add_library(selene::selene ALIAS selene)

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
        INSTALL_DESTINATION ${SELENE_INSTALL_CONFIGDIR}
        NO_SET_AND_CHECK_MACRO
        NO_CHECK_REQUIRED_COMPONENTS_MACRO)

# Install library files
install(TARGETS ${SELENE_INSTALL_TARGETS}
        EXPORT selene-targets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# Install headers
install(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        FILES_MATCHING PATTERN "*.hpp"
                       PATTERN "_doc.hpp" EXCLUDE)

# Install config headers
install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/selene_config.hpp
        ${CMAKE_CURRENT_BINARY_DIR}/selene_version.hpp
        DESTINATION
        ${CMAKE_INSTALL_INCLUDEDIR}/selene)

# Install export set
install(EXPORT selene-targets
        FILE selene-targets.cmake
        NAMESPACE selene::
        DESTINATION ${SELENE_INSTALL_CONFIGDIR})

# Install config and config-version files
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/selene-config.cmake
              ${CMAKE_CURRENT_BINARY_DIR}/selene-config-version.cmake
        DESTINATION ${SELENE_INSTALL_CONFIGDIR})

#------------------------------------------------------------------------------

# Exporting from the build tree

export(EXPORT selene-targets
       FILE ${CMAKE_CURRENT_BINARY_DIR}/selene-targets.cmake
       NAMESPACE selene::)

export(PACKAGE selene)
