cmake_minimum_required(VERSION 2.8)

project(DynCMD)

message(STATUS "-- Dynamic Command Line")

find_package (DynCMD REQUIRED)
find_package (ImgIncludes REQUIRED)
find_package (GTest QUIET) # Optional to build Unit Tests

if (NOT DEFINED DYNCMD_TESTS)
	set (DYNCMD_TESTS FALSE)
endif()

include_directories(${DYNCMD_INCLUDE_DIRS} ${IMGINCLUDES_INCLUDE_DIRS})
add_definitions(${DYNCMD_DEFINITIONS} ${IMGINCLUDES_DEFINITIONS})

set(SOURCES
	${DYNCMD_PREFIX}/code/commandline.c
)

set(HEADERS
	include/dyncmd/commandline.h
)

add_library(${DYNCMD_NAME} ${SOURCES} ${HEADERS})

# unit tests for command line
if (GTEST_FOUND AND DYNCMD_TESTS)

	message(STATUS "    unit tests")
	add_subdirectory(test)

endif()
