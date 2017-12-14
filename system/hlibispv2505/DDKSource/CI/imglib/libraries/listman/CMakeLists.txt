cmake_minimum_required(VERSION 2.8)

# ----------------------------------------------------------------------
# Find this library
# ----------------------------------------------------------------------
find_package(ImgLib_LISTMAN REQUIRED)

# ----------------------------------------------------------------------
# External dependencies
# ----------------------------------------------------------------------
find_package(ImgIncludes REQUIRED)

include_directories(
	${IMGLIB_LISTMAN_INCLUDE_DIRS}
	${IMGINCLUDES_INCLUDE_DIRS}
)

add_definitions(
	${IMGLIB_LISTMAN_DEFINITIONS}
	${IMGINCLUDES_DEFINTIONS}
)
# ----------------------------------------------------------------------
# List of sources
# ----------------------------------------------------------------------
set(HEADERS
	include/list_manager.h
)

set(EXTRA
	FindImgLib_LISTMAN.cmake
)

set(SOURCES
	code/list_manager.c
)

# ----------------------------------------------------------------------
# Build the library
# ----------------------------------------------------------------------

add_library(${IMGLIB_LISTMAN_LIBRARIES} STATIC ${SOURCES} ${HEADERS} ${EXTRA})
