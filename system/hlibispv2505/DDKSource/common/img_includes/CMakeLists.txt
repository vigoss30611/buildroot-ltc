cmake_minimum_required(VERSION 2.8)

project(ImgIncludes)

# ----------------------------------------------------------------------
# Find this library
# ----------------------------------------------------------------------
find_package(ImgIncludes REQUIRED)
include_directories(${IMGINCLUDES_INCLUDE_DIRS})
add_definitions(${IMGINCLUDES_DEFINITIONS})

# ----------------------------------------------------------------------
# List of sources
# ----------------------------------------------------------------------
set(INCLUDES
	img_errors.h   
)

set(SOURCES ${SOURCES} img_errors.c)

add_library(ImgIncludes STATIC ${SOURCES} ${INCLUDES})