if(NOT IS_DIRECTORY ${SELENE_DIR})
    message(FATAL_ERROR "Please build using the outermost CMakeLists.txt file.")
endif()

#------------------------------------------------------------------------------

set(CPACK_PACKAGE_NAME                ${PROJECT_NAME})
set(CPACK_PACKAGE_VENDOR              "selene-lib.org")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A C++17 image representation, processing and I/O library.")
set(CPACK_PACKAGE_INSTALL_DIRECTORY   ${CPACK_PACKAGE_NAME})
set(CPACK_PACKAGE_VERSION_MAJOR       ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR       ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH       ${PROJECT_VERSION_PATCH})
set(CPACK_VERBATIM_VARIABLES          YES)
set(CPACK_RESOURCE_FILE_LICENSE       ${SELENE_DIR}/LICENSE)

set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Michael Hofmann <kmhofmann+selene@gmail.com>")
set(CPACK_DEBIAN_PACKAGE_SECTION "Libraries")
# To be provided externally: CPACK_DEBIAN_PACKAGE_ARCHITECTURE, CPACK_DEBIAN_PACKAGE_DEPENDS

include(CPack)
