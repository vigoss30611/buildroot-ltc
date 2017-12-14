cmake_minimum_required(VERSION 2.8.4)

find_package(HDRLibs REQUIRED) # optional because HDRLibs archive may not be present

include(ExternalProject)

if (${HDRLIBS_FOUND})

    set(HDRLIBS_DOWNLOAD_DIR ${CMAKE_CURRENT_BINARY_DIR}/libraries/HDRlibs/download)
    set(HDRLIBS_SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/libraries/HDRLibs/source)
    set(HDRLIBS_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/libraries/HDRLibs/)
    
    if (NOT DEFINED HDRLIBS_DEBUG_IMAGES)
        set(HDRLIBS_DEBUG_IMAGES FALSE)
    endif()
    
    message(STATUS "-- HDRLibs")
    message(STATUS "      built with debug images: ${HDRLIBS_DEBUG_IMAGES}")
    get_filename_component (HDRLIB_ARCH_FILE "${HDRLIBS_ARCHIVE}" NAME)
    message(STATUS "      using archive ${HDRLIB_ARCH_FILE}")
    
    set(HDRLIBS_CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=${HDRLIBS_INSTALL_DIR}
        -DBUILD_LIBRAW=OFF
        -DBUILD_FLXIMG=OFF
        -DENABLE_HDR_DEBUG_IMAGES=${HDRLIBS_DEBUG_IMAGES}
        -DBUILD_APPLICATIONS=OFF
    )
    set(HDRLIBS_PATCH_COMMAND "")
    set(HDRLIBS_CMAKE_CACHE_ARGS "")
    
    
    
    
    # Pass toolchain
    if(NOT "${CMAKE_TOOLCHAIN_FILE}" STREQUAL "")
        set(HDRLIBS_CMAKE_CACHE_ARGS ${HDRLIBS_CMAKE_CACHE_ARGS} -DCMAKE_TOOLCHAIN_FILE:STRING=${CMAKE_TOOLCHAIN_FILE})
    endif()

    # Need to set the build type
    set(HDRLIBS_CMAKE_CACHE_ARGS ${HDRLIBS_CMAKE_CACHE_ARGS} -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE})

    # ExternalProject_Add has CMAKE_CACHE_ARGS and URL_MD5 from 2.8.4
    ExternalProject_Add(
        ${HDRLIBS_NAME}
        URL ${HDRLIBS_ARCHIVE}
        URL_MD5 ${HDRLIBS_MD5}
        DOWNLOAD_DIR ${HDRLIBS_DOWNLOAD_DIR}
        SOURCE_DIR ${HDRLIBS_SOURCE_DIR}
        BINARY_DIR ${HDRLIBS_BINARY_DIR}
        INSTALL_DIR ${HDRLIBS_INSTALL_DIR}
        CMAKE_ARGS ${HDRLIBS_CMAKE_ARGS}
        CMAKE_CACHE_ARGS ${HDRLIBS_CMAKE_CACHE_ARGS}
        UPDATE_COMMAND ""
        PATCH_COMMAND ${HDRLIBS_PATCH_COMMAND}
    )
    
else()    
    message(STATUS "-- HDRLibs not present - skipping")
endif()
