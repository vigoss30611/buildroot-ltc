cmake_minimum_required(VERSION 2.8)

project(GZipFileIO)

message(STATUS "GZip File IO")

find_package(ImgLib_GZIPIO REQUIRED) # trick to avoid duplication
find_package(ImgIncludes REQUIRED)
find_package(GTest QUIET) # optional - just not to build unit tests if not found
find_package(ZLIB QUIET) # optional
#
# Compilation options
#

# even if Zlib is found do not use it
if(NOT DEFINED IMGLIB_GZIPIO_FORCE_NO_ZLIB)
    set(IMGLIB_GZIPIO_FORCE_NO_ZLIB FALSE)
endif()

if (NOT DEFINED IMGLIB_GZIPIO_DEBUG)
	set (IMGLIB_GZIPIO_DEBUG FALSE)
endif()
if (NOT DEFINED IMGLIB_GZIPIO_TESTS)
	set (IMGLIB_GZIPIO_TESTS FALSE)
endif()

# ----------------------------------------------------------------------
# External dependencies
# ----------------------------------------------------------------------

include_directories(
	${IMGINCLUDES_INCLUDE_DIRS}
	${IMGLIB_GZIPIO_INCLUDE_DIRS}
)

add_definitions(
	${IMGINCLUDES_DEFINITIONS}
	${IMGLIB_GZIPIO_DEFINITIONS}
)

if (${IMGLIB_GZIPIO_DEBUG})
	# adds the debug function
	add_definitions(-DIMG_GZIPIO_DEBUG)
endif()

# ----------------------------------------------------------------------
# List of sources
# ----------------------------------------------------------------------
set(INCLUDES include/gzip_fileio.h)

set(SOURCES
	code/fileio_basic.c
	code/fileio_special.c
)

# ----------------------------------------------------------------------
# Build the library
# ----------------------------------------------------------------------
add_library(${IMGLIB_GZIPIO_LIBRARIES} STATIC ${SOURCES} ${INCLUDES} ${HEADERS})

if(NOT IMGLIB_GZIPIO_FORCE_NO_ZLIB AND ZLIB_FOUND)

	message(STATUS "    using zlib")

	include_directories(${ZLIB_INCLUDE_DIRS})
	add_definitions(-DFILEIO_USEZLIB ${ZLIB_DEFINITIONS})
	
	target_link_libraries(${IMGLIB_GZIPIO_LIBRARIES} ${ZLIB_LIBRARIES})
	if(DEFINED ZLIB_NAME) # if using local zlib
		add_dependencies(${IMGLIB_GZIPIO_LIBRARIES} ${ZLIB_NAME})
	endif()
endif()

if(IMGLIB_GZIPIO_TESTS AND GTEST_FOUND)
	add_subdirectory(test)
endif()
