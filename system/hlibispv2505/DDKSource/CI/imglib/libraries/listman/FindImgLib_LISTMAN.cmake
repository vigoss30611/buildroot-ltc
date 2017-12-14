cmake_minimum_required (VERSION 2.8)

# ----------------------------------------------------------------------
# Locate and configure the ImgLib LISTMAN library.
#
# Possible options:
#
#   IMGLIB_LISTMAN_PREFIX - the directory where ImgLib LISTMAN is located in the project (optional)
#
# Defines the following variables:
#
#   IMGLIB_LISTMAN_FOUND - Found the ImgLib LISTMAN
#   IMGLIB_LISTMAN_PREFIX		- The directory where this file is located
#   IMGLIB_LISTMAN_INCLUDE_DIRS	- The directories needed on the include paths 
#								  for this library
#   IMGLIB_LISTMAN_DEFINITIONS	- The definitions to apply for this library
#   IMGLIB_LISTMAN_LIBRARIES	- The libraries to link to
# ----------------------------------------------------------------------

# ----------------------------------------------------------------------
# Set package variables
# ----------------------------------------------------------------------
if(NOT DEFINED IMGLIB_LISTMAN_PREFIX)
	# set IMGLIB_LISTMAN_PREFIX to directory where the current file file is located
	get_filename_component(IMGLIB_LISTMAN_PREFIX "${CMAKE_CURRENT_LIST_FILE}" PATH)
endif()

set(IMGLIB_LISTMAN_FOUND TRUE)
set(IMGLIB_LISTMAN_INCLUDE_DIRS ${IMGLIB_LISTMAN_PREFIX}/include)
set(IMGLIB_LISTMAN_LIBRARIES ImgLib_LISTMAN)

set(IMGLIB_LISTMAN_BUILD_KO ${IMGLIB_LISTMAN_PREFIX}/IMGLIB_LISTMAN_kernel.cmake)

if(IMGLIB_LISTMAN_ENABLE)
    set(IMGLIB_LISTMAN_DEFINITIONS ${IMGLIB_LISTMAN_DEFINITIONS} -D_LOG_EVENTS_)
endif()

if(DEBUG_MODULES)
    message("IMGLIB_LISTMAN_INCLUDE_DIRS = ${IMGLIB_LISTMAN_INCLUDE_DIRS}")
    message("IMGLIB_LISTMAN_LIBRARIES = ${IMGLIB_LISTMAN_LIBRARIES}")
endif()

mark_as_advanced(IMGLIB_LISTMAN_INCLUDE_DIRS IMGLIB_LISTMAN_DEFINITIONS IMGLIB_LISTMAN_LIBRARIES)
