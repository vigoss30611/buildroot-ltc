#
# find your local installation of HDRLibs
# note: HDRLibs are a simulator of HDR merging that may not be present in all
# DDK releases
#
cmake_minimum_required (VERSION 2.8)

get_filename_component (HDRLIBS_PREFIX "${CMAKE_CURRENT_LIST_FILE}" PATH)

if (NOT DEFINED HDRLIBS_INSTALL_DIR)
    set (HDRLIBS_INSTALL_DIR ${CMAKE_BINARY_DIR}/downloads/install/HDRLibs)
endif()

set(HDRLIBS_ARCHIVE ${HDRLIBS_PREFIX}/HDRLibs_d3631954.tgz)
set(HDRLIBS_MD5 dde5634f1579451a5b01ce5af409f669)

set(HDRLIBS_NAME HDRLibs-external)

# assumes not present
set(HDRLIBS_FOUND FALSE)
set(HDRLIBS_DEFINITIONS)
set(HDRLIBS_INCLUDE_DIRS)
set(HDRLIBS_LIBRARIES)

if (EXISTS "${HDRLIBS_ARCHIVE}")

    set(HDRLIBS_FOUND TRUE)
    #set(HDRLIBS_DEFINITIONS)
    set(HDRLIBS_INCLUDE_DIRS ${HDRLIBS_INSTALL_DIR}/include)
    set(HDRLIBS_LIBS
        seqHDR
        util
        rawimage
        image
        registration
        amtc
    )
    
    foreach(lib IN ITEMS ${HDRLIBS_LIBS})
        if(WIN32)
            set(HDRLIBS_LIBRARIES ${HDRLIBS_LIBRARIES} ${HDRLIBS_INSTALL_DIR}/lib/${lib}.lib)
        else()
            set(HDRLIBS_LIBRARIES ${HDRLIBS_LIBRARIES} ${HDRLIBS_INSTALL_DIR}/lib/lib${lib}.a)
        endif()
    endforeach()

endif()

mark_as_advanced (HDRLIBS_DEFINITIONS HDRLIBS_INCLUDE_DIRS HDRLIBS_LIBRARIES)