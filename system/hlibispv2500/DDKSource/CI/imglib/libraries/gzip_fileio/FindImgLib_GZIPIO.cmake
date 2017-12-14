cmake_minimum_required(VERSION 2.8)
# Locate and configure the ImgLib GZIPIO library.
#
# Possible compile time option (in CMakeLists):  
#   IMGLIB_GZIPIO_ZLIB - if to use ZLIB (default FALSE)
#   IMGLIB_GZIPIO_DEBUG - enable the debug messages
#   IMGLIB_GZIPIO_TESTS - if GTest is found build or not the unit test executable
#
# Defines the following variables:
#
#   IMGLIB_GZIPIO_PREFIX - the directory where ImgLib GZIPIO is located in the project
#   IMGLIB_GZIPIO_FOUND - Found the ImgLib GZIPIO
#   IMGLIB_GZIPIO_INCLUDE_DIRS - The directories needed on the include paths
#   IMGLIB_GZIPIO_LIBRARIES - The libraries to link to

get_filename_component(IMGLIB_GZIPIO_PREFIX "${CMAKE_CURRENT_LIST_FILE}" PATH)

set(IMGLIB_GZIPIO_FOUND TRUE)
set(IMGLIB_GZIPIO_INCLUDE_DIRS ${IMGLIB_GZIPIO_PREFIX}/include)
set(IMGLIB_GZIPIO_DEFINITIONS)
set(IMGLIB_GZIPIO_LIBRARIES ImgLib_GZIPIO)

if(DEBUG_MODULES)
    message("IMGLIB_GZIPIO_INCLUDE_DIRS = ${IMGLIB_GZIPIO_INCLUDE_DIRS}")
    message("IMGLIB_GZIPIO_LIBRARIES = ${IMGLIB_GZIPIO_LIBRARIES}")
endif()

mark_as_advanced(IMGLIB_GZIPIO_INCLUDE_DIRS IMGLIB_GZIPIO_DEFINITIONS IMGLIB_GZIPIO_LIBRARIES)
