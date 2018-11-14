cmake_minimum_required(VERSION 3.0)
project(jarngreipr)
enable_testing()

set(JARNGREIPR_CMAKE_DIR  "${PROJECT_SOURCE_DIR}/cmake")
set(JARNGREIPR_EXTLIB_DIR "${PROJECT_SOURCE_DIR}/extlib")
set(JARNGREIPR_BIN_DIR    "${PROJECT_SOURCE_DIR}/bin")

include("${JARNGREIPR_CMAKE_DIR}/extlib.cmake")
include("${JARNGREIPR_CMAKE_DIR}/options.cmake")

include_directories("${PROJECT_SOURCE_DIR}")

add_subdirectory("${PROJECT_SOURCE_DIR}/src")
add_subdirectory("${PROJECT_SOURCE_DIR}/test")
