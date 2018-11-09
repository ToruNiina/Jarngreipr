#################
# Project setup #
#################

# Cmake version
cmake_minimum_required(VERSION 3.0)
# Project name
project(jarngreipr)
# Testing support
enable_testing()
# Executable name
set(JARNGREIPR_EXE "jarngreipr")

#########################
# Paths and directories #
#########################

# Define directories
set(BIN "${PROJECT_SOURCE_DIR}/bin")
set(EXT "${PROJECT_SOURCE_DIR}/extlib")
set(LIB "${PROJECT_SOURCE_DIR}/lib")
set(SRC "${PROJECT_SOURCE_DIR}/src")
set(TEST "${PROJECT_SOURCE_DIR}/test")
set(CMAKE "${PROJECT_SOURCE_DIR}/cmake")
#set(MJOLNIR "${PROJECT_SOURCE_DIR}/mjolnir")
#set(JARNGREIPR "${PROJECT_SOURCE_DIR}/jarngreipr")

# Add paths
include_directories("${EXT}")
include_directories("${EXT}/Boost.toml")
include_directories("${PROJECT_SOURCE_DIR}")
#include_directories("${MJOLNIR}/core")
#include_directories("${MJOLNIR}/input")
#include_directories("${MJOLNIR}/interaction")
#include_directories("${MJOLNIR}/math")
#include_directories("${MJOLNIR}/potential")
#include_directories("${MJOLNIR}/util")
#include_directories("${JARNGREIPR}/")
#include_directories("${JARNGREIPR}/geninp")

# Source cmake files
include("${CMAKE}/extlib.cmake")
include("${CMAKE}/options.cmake")
include("${CMAKE}/compiler.cmake")

# Add executable directories
add_subdirectory("${SRC}")

# Add test directories
add_subdirectory("${TEST}")
