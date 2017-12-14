cmake_minimum_required(VERSION 2.8)

find_package(ISPC2 REQUIRED)
find_package(Transif QUIET) # optional - to use simulator from dll when using fake interface
find_package(HDRLibs QUIET) # optional

if(NOT ${ISPC2TEST_FOUND})
    message(FATAL_ERROR "compiling ispc2 test library but not defined!")
endif()
if(NOT DEFINED CI_EXT_DATA_GENERATOR)
	message(STATUS "CI_EXT_DATA_GENERATOR not defined!")
endif()

include_directories(
    ${ISPC2TEST_INCLUDE_DIRS} 
    ${ISPC2_INCLUDE_DIRS}
)
add_definitions(
    ${ISPC2TEST_DEFINITIONS}
    ${ISPC2_DEFINITIONS}
)

if (CI_EXT_DATA_GENERATOR)
  add_definitions(-DEXT_DATAGEN)
endif()

set (ISPCTEST_C
	info_helper.cpp
	hdf_helper.cpp
	hdr_library.cpp
	perf_controls.cpp	
)

set (ISPCTEST_H
	include/ispctest/hdf_helper.h
	include/ispctest/info_helper.h
	include/ispctest/perf_controls.h
)

message(STATUS "ISPC2Test helper library")

if(NOT CI_BUILD_KERNEL_MODULE)
	message(STATUS "    supports fake driver")

	set(ISPCTEST_C ${ISPCTEST_C} driver_init.cpp)
	set(ISPCTEST_H ${ISPCTEST_H} include/ispctest/driver_init.h)
endif()

	
add_library(${ISPC2TEST_LIBRARIES} ${ISPCTEST_C} ${ISPCTEST_H})
target_link_libraries(${ISPC2TEST_LIBRARIES} ${ISPC2_LIBRARIES})

if (TRANSIF_FOUND AND NOT CI_BUILD_KERNEL_MODULE)
  # transif is not found when building real drivers because it is to load the simulator as a dll when running fake interface - but it should always be found when building fake interface
  message(STATUS "    supports transif")
  include_directories(${TRANSIF_INCLUDE_DIRS})
  add_definitions(${TRANSIF_DEFINITIONS} -DISPCTEST_TRANSIF)
  target_link_libraries(${ISPC2TEST_LIBRARIES} ${TRANSIF_LIBRARIES}) # so even if found it is only linked if not building kernel module
endif()

if (HDRLIBS_FOUND)
    message(STATUS "    supports HDR libraries")
    include_directories(${HDRLIBS_INCLUDE_DIRS})
    add_definitions(
        ${HDRLIBS_DEFINITIONS}
        -DHDRLIBS_SEQHDR
    )
    target_link_libraries(${ISPC2TEST_LIBRARIES} ${HDRLIBS_LIBRARIES})
    add_dependencies(${ISPC2TEST_LIBRARIES} ${HDRLIBS_NAME})
endif()
