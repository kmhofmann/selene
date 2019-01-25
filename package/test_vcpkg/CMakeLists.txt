project(PackageTest CXX)
cmake_minimum_required(VERSION 3.5)

# https://cmake.org/cmake/help/latest/prop_tgt/CXX_STANDARD.html
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(selene REQUIRED)

add_executable(example example.cpp)
target_link_libraries(example PRIVATE selene::selene)
