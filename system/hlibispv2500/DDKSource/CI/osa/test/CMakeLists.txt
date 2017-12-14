cmake_minimum_required(VERSION 2.8)

project(OSATest)

# ----------------------------------------------------------------------
# If this test is on top level
# ----------------------------------------------------------------------
if (${CMAKE_CURRENT_SOURCE_DIR} MATCHES ${CMAKE_SOURCE_DIR})
	set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${OSATest_SOURCE_DIR}/.. ${IMGINCLUDES_PREFIX} ${SYSTEMC_PREFIX})
	set(IMGLIB_BUILD_LIST list_utils)
	include(${IMGLIB_PREFIX}/UsePackage.cmake)
	add_subdirectory(.. OSA)
	add_subdirectory(${IMGLIB_PREFIX} imglib)
	if(${OSA_SYSTEMC})
		add_subdirectory(${SYSTEMC_PREFIX} sysc)
	endif()
endif()

# ----------------------------------------------------------------------
# Find this library
# ----------------------------------------------------------------------
find_package(OSA REQUIRED)
include_directories(${OSA_INCLUDE_DIRS})
add_definitions(${OSA_DEFINITIONS})

# ----------------------------------------------------------------------
# External dependencies
# ----------------------------------------------------------------------
find_package(ImgIncludes REQUIRED)
include_directories(${IMGINCLUDES_INCLUDE_DIRS})
add_definitions(${IMGINCLUDES_DEFINITIONS})


# ----------------------------------------------------------------------
# List of sources
# ----------------------------------------------------------------------
set(INCLUDES
)

set(HEADERS
	mutexTest.h
	eventTest.h
	threadTest.h
	TLSTest.h
)

set(SOURCES 
	test.c
	mutexTest.c
	eventTest.c
	threadTest.c
	TLSTest.c
)

if(${OSA_SYSTEMC})
	find_package(Systemc REQUIRED)
	include_directories(${SYSTEMC_INCLUDE_DIRS})
  add_definitions(${SYSTEMC_DEFINITIONS})
  set(SOURCES ${SOURCES} sysc_test.cpp)
  add_definitions(-DOSA_SYSTEMC)
endif()

# ----------------------------------------------------------------------
# Build the library
# ----------------------------------------------------------------------
add_executable(OSATest
               ${SOURCES}
               ${HEADERS}
)

target_link_libraries(OSATest
	${OSA_LIBRARIES}
)

