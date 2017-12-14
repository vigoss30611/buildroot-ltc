cmake_minimum_required(VERSION 2.8)

project(IMGLIB_gzipio_test)
# ----------------------------------------------------------------------
# if this test is on top level
# ----------------------------------------------------------------------
if (${CMAKE_CURRENT_SOURCE_DIR} STREQUAL ${CMAKE_SOURCE_DIR})
	set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${IMGLIB_gzipio_test_SOURCE_DIR}/.. ${IMGINCLUDES_PREFIX})
	set(DOWNLOADS_BUILD_LIST gtest)
	if(IMGLIB_GZIPIO_ZLIB)
		set(DOWNLOADS_BUILD_LIST ${DOWNLOADS_BUILD_LIST} zlib)
	endif()
	include(${EXTERNAL_PREFIX}/UsePackage.cmake)
	add_subdirectory(${EXTERNAL_PREFIX} external)
	add_subdirectory(.. gzipio)
endif()


message(STATUS "    unit tests")

find_package(GTest REQUIRED)
find_package(ImgLib_GZIPIO REQUIRED)
find_package(ImgIncludes REQUIRED)

#
# assumes that this file is included from gzip_fileio folder
# it inherists SOURCES, include directories and definitions
#

set (TEST_SOURCES 
	highlevel_test.cpp
)

include_directories(
  ${GTEST_INCLUDE_DIRS}
  ${IMGINCLUDES_INCLUDE_DIRS}
  ${IMGLIB_GZIPIO_INCLUDE_DIRS}
)
add_definitions(
  ${GTEST_DEFINITIONS}
  ${IMGINCLUDES_DEFINITIONS}
  ${IMGLIB_GZIPIO_DEFINITIONS}
)

add_executable(${IMGLIB_GZIPIO_LIBRARIES}_unit_tests ${TEST_SOURCES})
target_link_libraries(${IMGLIB_GZIPIO_LIBRARIES}_unit_tests ${IMGLIB_GZIPIO_LIBRARIES} ${GTEST_LIBRARIES})
add_dependencies(${IMGLIB_GZIPIO_LIBRARIES}_unit_tests ${GTEST_NAME})
add_test(GZipFileIO ${IMGLIB_GZIPIO_LIBRARIES}_unit_tests --gtest_output=xml:xunits_gzipfileio.xml)

file(COPY resources/ DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/resources)
