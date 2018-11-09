# BOOST
if(BOOST_ROOT)
    find_package(Boost 1.66.0 REQUIRED)
    include_directories(${Boost_INCLUDE_DIRS})
else()
    if(EXISTS "${EXT}/boost_1_67_0/boost/version.hpp")
        message(STATUS "boost 1.67.0 exists.")
        include_directories("${EXT}/boost_1_67_0")
    else()
        find_program(WGET wget)
        if(NOT WGET)
            message(FATAL_ERROR "wget is not installed. Can't download BOOST library.")
        endif()
        message(STATUS "downloading Boost 1.67.0 to ./extlib/ ...")
        execute_process(COMMAND "${WGET}" https://dl.bintray.com/boostorg/release/1.67.0/source/boost_1_67_0.tar.bz2
            WORKING_DIRECTORY "${EXT}"
            RESULT_VARIABLE DOWNLOAD_BOOST_RESULT OUTPUT_QUIET ERROR_QUIET)
        if(NOT ${DOWNLOAD_BOOST_RESULT} EQUAL "0")
            message(FATAL_ERROR "failed to download Boost 1.67.0. please confirm network connections.")
        endif()

        execute_process(COMMAND which sha256sum RESULT_VARIABLE SHA256SUM_EXISTS OUTPUT_QUIET ERROR_QUIET)
        execute_process(COMMAND which shasum    RESULT_VARIABLE SHASUM_EXISTS    OUTPUT_QUIET ERROR_QUIET)
        if(${SHA256SUM_EXISTS} EQUAL "0")
            execute_process(COMMAND sha256sum --check boost_1_67_0_tar_bz2_sha256sum.dat
                WORKING_DIRECTORY "${EXT}"
                RESULT_VARIABLE   DOWNLOADED_SHA256SUM_RESULT)
        elseif(${SHASUM_EXISTS} EQUAL "0")
            execute_process(COMMAND shasum --algorithm 256 --check boost_1_67_0_tar_bz2_sha256sum.dat
                WORKING_DIRECTORY "${EXT}"
                RESULT_VARIABLE   DOWNLOADED_SHA256SUM_RESULT)
        endif()

        if(NOT ${DOWNLOADED_SHA256SUM_RESULT} EQUAL "0")
            message(FATAL_ERROR "boost_1_67_0.tar.bz2 has invalid sha256sum!")
        endif()

        execute_process(COMMAND tar xf boost_1_67_0.tar.bz2
            WORKING_DIRECTORY "${EXT}"
            OUTPUT_QUIET ERROR_QUIET)

        include_directories(${EXT}/boost_1_67_0)
        message(STATUS "done.")
    endif()
endif()

# TOML
if(NOT EXISTS "${EXT}/Boost.toml/toml/toml.hpp")
    execute_process(COMMAND git submodule update --init --recursive
                    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
endif()
