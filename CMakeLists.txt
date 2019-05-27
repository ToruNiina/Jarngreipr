cmake_minimum_required(VERSION 3.0)
project(jarngreipr)
enable_testing()

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD          11) # use c++11
set(CMAKE_CXX_EXTENSIONS       OFF) # not gnu11, c++11

include("${PROJECT_SOURCE_DIR}/cmake/extlib.cmake")
include("${PROJECT_SOURCE_DIR}/cmake/options.cmake")

include_directories("${PROJECT_SOURCE_DIR}")
include_directories("${PROJECT_SOURCE_DIR}/extlib/Mjolnir")

add_subdirectory("${PROJECT_SOURCE_DIR}/src")
add_subdirectory("${PROJECT_SOURCE_DIR}/test")
