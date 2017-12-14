cmake_minimum_required (VERSION 2.8)

# ----------------------------------------------------------------------
# Locate and configure the List utils library.
#
# Defines the following variables:
#
#   IMGLIB_LISTUTILS_FOUND         - Found the library
#   IMGLIB_LISTUTILS_PREFIX        - The directory where this file is located
#   IMGLIB_LISTUTILS_INCLUDE_DIRS	- The directories needed on the include paths 
#						  	  for this library
#   IMGLIB_LISTUTILS_DEFINITIONS   - The definitions to apply for this library
#   IMGLIB_LISTUTILS_LIBRARIES     - The libraries to link to
# ----------------------------------------------------------------------

# ----------------------------------------------------------------------
# Set package variables
# ----------------------------------------------------------------------
get_filename_component(IMGLIB_LISTUTILS_PREFIX "${CMAKE_CURRENT_LIST_FILE}" PATH)

set(IMGLIB_LISTUTILS_FOUND TRUE)
set(IMGLIB_LISTUTILS_INCLUDE_DIRS ${IMGLIB_LISTUTILS_PREFIX}/include)
set(IMGLIB_LISTUTILS_LIBRARIES ListUtils)

mark_as_advanced(IMGLIB_LISTUTILS_INCLUDE_DIRS IMGLIB_LISTUTILS_DEFINITIONS IMGLIB_LISTUTILS_LIBRARIES)

