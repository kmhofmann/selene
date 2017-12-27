include(CMakeFindDependencyMacro)

find_dependency(JPEG)
find_dependency(PNG 1.6)
find_dependency(OpenCV)

include("${CMAKE_CURRENT_LIST_DIR}/selene-targets.cmake")