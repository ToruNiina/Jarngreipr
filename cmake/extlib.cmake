if(BOOST_ROOT)
    find_package(Boost 1.67.0 REQUIRED)
    include_directories(${Boost_INCLUDE_DIRS})
else()
    if(EXISTS "${PROJECT_SOURCE_DIR}/extlib/boost_1_67_0/boost/version.hpp")

        message(STATUS "boost 1.67.0 exists.")
        include_directories("${PROJECT_SOURCE_DIR}/extlib/boost_1_67_0")

    else()
        message(STATUS "Boost library not found. Donwloading...")

        file(DOWNLOAD https://dl.bintray.com/boostorg/release/1.67.0/source/boost_1_67_0.tar.bz2
            ${PROJECT_SOURCE_DIR}/extlib/boost_1_67_0.tar.bz2
            EXPECTED_HASH SHA256=2684c972994ee57fc5632e03bf044746f6eb45d4920c343937a465fd67a5adba
            STATUS JARNGREIPR_DOWNLOAD_BOOST_STATUS)

        # check status
        # STATUS variable is a list.
        # If the first element is not 0, it means downloading is failed.
        # And the second element has the error reason as a string (if any).
        list(GET JARNGREIPR_DOWNLOAD_BOOST_STATUS 0 JARNGREIPR_DOWNLOAD_BOOST_RESULT)
        if(NOT ${JARNGREIPR_DOWNLOAD_BOOST_RESULT} EQUAL 0)
            list(GET JARNGREIPR_DOWNLOAD_BOOST_STATUS 1 JARNGREIPR_DOWNLOAD_BOOST_ERROR)
            message(FATAL_ERROR "failed to download Boost 1.67.0."
                ${JARNGREIPR_DOWNLOAD_BOOST_ERROR})
        endif()

        message(STATUS "downloading completed. Unpacking...")

        execute_process(COMMAND tar xf boost_1_67_0.tar.bz2
            WORKING_DIRECTORY "${JARNGREIPR_EXTLIB_DIR}"
            OUTPUT_QUIET ERROR_QUIET)

        include_directories(${JARNGREIPR_EXTLIB_DIR}/boost_1_67_0)
        message(STATUS "done.")
    endif()
endif()

if(NOT EXISTS "${JARNGREIPR_EXTLIB_DIR}/toml/toml.hpp")
    execute_process(COMMAND git submodule update --init --recursive
                    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
endif()
