if(NOT IS_DIRECTORY ${SELENE_DIR})
    message(FATAL_ERROR "Please build using the outermost CMakeLists.txt file.")
endif()

#------------------------------------------------------------------------------

install(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/selene DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

#------------------------------------------------------------------------------

add_library(selene_base "")
target_sources(selene_base
        PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/selene/base/Allocators.cpp
        INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/selene/base/Allocators.hpp
        ${CMAKE_CURRENT_LIST_DIR}/selene/base/Assert.hpp
        ${CMAKE_CURRENT_LIST_DIR}/selene/base/MemoryBlock.hpp
        ${CMAKE_CURRENT_LIST_DIR}/selene/base/MessageLog.hpp
        ${CMAKE_CURRENT_LIST_DIR}/selene/base/Types.hpp
        ${CMAKE_CURRENT_LIST_DIR}/selene/base/Utils.hpp
        )
target_include_directories(selene_base PUBLIC ${SELENE_DIR}/src)

install(TARGETS selene_base
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

#------------------------------------------------------------------------------

add_library(selene_io "")
target_sources(selene_io
        PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/selene/io/FileUtils.cpp
        INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/selene/io/FileReader.hpp
        ${CMAKE_CURRENT_LIST_DIR}/selene/io/FileUtils.hpp
        ${CMAKE_CURRENT_LIST_DIR}/selene/io/FileWriter.hpp
        ${CMAKE_CURRENT_LIST_DIR}/selene/io/MemoryReader.hpp
        ${CMAKE_CURRENT_LIST_DIR}/selene/io/MemoryWriter.hpp
        ${CMAKE_CURRENT_LIST_DIR}/selene/io/VectorReader.hpp
        ${CMAKE_CURRENT_LIST_DIR}/selene/io/VectorWriter.hpp
        ${CMAKE_CURRENT_LIST_DIR}/selene/io/WriterMode.hpp
        )
target_include_directories(selene_io PUBLIC ${SELENE_DIR}/src)

install(TARGETS selene_io
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

#------------------------------------------------------------------------------

add_library(selene_img "")
target_sources(selene_img
        PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/selene/img/JPEGRead.cpp
        ${CMAKE_CURRENT_LIST_DIR}/selene/img/JPEGWrite.cpp
        ${CMAKE_CURRENT_LIST_DIR}/selene/img/PixelFormat.cpp
        ${CMAKE_CURRENT_LIST_DIR}/selene/img/PNGRead.cpp
        ${CMAKE_CURRENT_LIST_DIR}/selene/img/PNGWrite.cpp
        ${CMAKE_CURRENT_LIST_DIR}/selene/img/detail/JPEGCommon.hpp
        ${CMAKE_CURRENT_LIST_DIR}/selene/img/detail/JPEGDetail.cpp
        ${CMAKE_CURRENT_LIST_DIR}/selene/img/detail/JPEGDetail.hpp
        ${CMAKE_CURRENT_LIST_DIR}/selene/img/detail/PNGDetail.cpp
        ${CMAKE_CURRENT_LIST_DIR}/selene/img/detail/PNGDetail.hpp
        ${CMAKE_CURRENT_LIST_DIR}/selene/img/detail/Util.hpp
        INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/selene/img/BoundingBox.hpp
        ${CMAKE_CURRENT_LIST_DIR}/selene/img/Image.hpp
        ${CMAKE_CURRENT_LIST_DIR}/selene/img/ImageData.hpp
        ${CMAKE_CURRENT_LIST_DIR}/selene/img/ImageDataToImage.hpp
        ${CMAKE_CURRENT_LIST_DIR}/selene/img/ImageToImageData.hpp
        ${CMAKE_CURRENT_LIST_DIR}/selene/img/JPEGCommon.hpp
        ${CMAKE_CURRENT_LIST_DIR}/selene/img/JPEGRead.hpp
        ${CMAKE_CURRENT_LIST_DIR}/selene/img/JPEGWrite.hpp
        ${CMAKE_CURRENT_LIST_DIR}/selene/img/OpenCV.hpp
        ${CMAKE_CURRENT_LIST_DIR}/selene/img/Pixel.hpp
        ${CMAKE_CURRENT_LIST_DIR}/selene/img/PixelFormat.hpp
        ${CMAKE_CURRENT_LIST_DIR}/selene/img/PixelTraits.hpp
        ${CMAKE_CURRENT_LIST_DIR}/selene/img/PNGRead.hpp
        ${CMAKE_CURRENT_LIST_DIR}/selene/img/PNGWrite.hpp
        ${CMAKE_CURRENT_LIST_DIR}/selene/img/RowPointers.hpp
        ${CMAKE_CURRENT_LIST_DIR}/selene/img/Types.hpp
        )
target_include_directories(selene_img PUBLIC ${SELENE_DIR}/src)
target_link_libraries(selene_img selene_base)

if (JPEG_FOUND)
    target_compile_definitions(selene_img PUBLIC SELENE_WITH_LIBJPEG)
    if (JPEG_PARTIAL_DECODING)
        target_compile_definitions(selene_img PUBLIC SELENE_LIBJPEG_PARTIAL_DECODING)
    endif()
    if (JPEG_EXTENDED_COLORSPACES)
        target_compile_definitions(selene_img PUBLIC SELENE_LIBJPEG_EXTENDED_COLORSPACES)
    endif()
    target_include_directories(selene_img PRIVATE ${JPEG_INCLUDE_DIR})
    target_link_libraries(selene_img ${JPEG_LIBRARIES})
endif()

if (PNG_FOUND)
    target_compile_definitions(selene_img PUBLIC SELENE_WITH_LIBPNG)
    target_include_directories(selene_img PRIVATE ${PNG_INCLUDE_DIR})
    target_link_libraries(selene_img ${PNG_LIBRARIES})
endif()

if (OPENCV_CORE_FOUND)
    target_compile_definitions(selene_img PUBLIC SELENE_WITH_OPENCV)
    target_include_directories(selene_img PRIVATE ${OpenCV_INCLUDE_DIRS})
    target_link_libraries(selene_img ${OpenCV_LIBS})
endif()

install(TARGETS selene_img
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

#------------------------------------------------------------------------------

add_library(selene_thread INTERFACE)
target_sources(selene_thread
        INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/selene/thread/ThreadPool.hpp
        ${CMAKE_CURRENT_LIST_DIR}/selene/thread/detail/Callable.hpp
        ${CMAKE_CURRENT_LIST_DIR}/selene/thread/detail/TaskQueue.hpp
        )
target_include_directories(selene_thread INTERFACE ${SELENE_DIR}/src)

#------------------------------------------------------------------------------

add_library(selene INTERFACE)
target_include_directories(selene INTERFACE ${SELENE_DIR}/src)
target_link_libraries(selene
        INTERFACE
        selene_base
        selene_img
        selene_io
        selene_thread)
