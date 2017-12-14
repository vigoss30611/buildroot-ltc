cmake_minimum_required (VERSION 2.8)

# ----------------------------------------------------------------------
# Locate and configure the ImgLib UTILS library.
#
# Possible options:
#
#   IMGLIB_UTILS_PREFIX - the directory where ImgLib UTILS is located in the project (REQUIRED)
#
# Defines the following variables:
#
#   IMGLIB_UTILS_FOUND - Found the ImgLib UTILS
#   IMGLIB_UTILS_PREFIX			- The directory where this file is located
#   IMGLIB_UTILS_INCLUDE_DIRS - The directories needed on the include paths							  for this library
#   IMGLIB_UTILS_DEFINITIONS	- The definitions to apply for this library
#   IMGLIB_UTILS_LIBRARIES		- The libraries to link to
# ----------------------------------------------------------------------

# ----------------------------------------------------------------------
# Set package variables
# ----------------------------------------------------------------------
if(NOT DEFINED IMGLIB_UTILS_PREFIX)
	# set IMGLIB_UTILS_PREFIX to directory where the current file file is located
	get_filename_component(IMGLIB_UTILS_PREFIX "${CMAKE_CURRENT_LIST_FILE}" PATH)
endif()

set(IMGLIB_UTILS_FOUND TRUE)
set(IMGLIB_UTILS_INCLUDE_DIRS ${IMGLIB_UTILS_PREFIX}/include)
set(IMGLIB_UTILS_LIBRARIES ImgLib_UTILS)

set(IMGLIB_UTILS_BUILD_KO ${IMGLIB_UTILS_PREFIX}/ImgLib_UTILS_kernel.cmake)

if(DEBUG_MODULES)
    message("IMGLIB_UTILS_INCLUDE_DIRS = ${IMGLIB_UTILS_INCLUDE_DIRS}")
    message("IMGLIB_UTILS_LIBRARIES = ${IMGLIB_UTILS_LIBRARIES}")
endif()

mark_as_advanced(IMGLIB_UTILS_INCLUDE_DIRS IMGLIB_UTILS_DEFINITIONS IMGLIB_UTILS_LIBRARIES )