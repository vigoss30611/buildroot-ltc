cmake_minimum_required (VERSION 2.8)

# ----------------------------------------------------------------------
# Locate and configure the OSA library.
#
# Options:
# 	OSA_SYSTEMC		- Tells OSA to use system C threading rather than OS threading
#
# Defines the following variables:
#
#   OSA_FOUND		- Found the library
#   OSA_PREFIX		- The directory where this file is located
#   OSA_INCLUDE_DIRS	- The directories needed on the include paths 
#						  for this library
#   OSA_DEFINITIONS	- The definitions to apply for this library
#   OSA_LIBRARIES	- The libraries to link to
# ----------------------------------------------------------------------

# ----------------------------------------------------------------------
# Set package variables
# ----------------------------------------------------------------------
get_filename_component(OSA_PREFIX "${CMAKE_CURRENT_LIST_FILE}" PATH)

set(OSA_FOUND TRUE)
set(OSA_INCLUDE_DIRS ${OSA_PREFIX}/include)
set(OSA_LIBRARIES OSA)

mark_as_advanced(OSA_INCLUDE_DIRS OSA_DEFINITIONS OSA_LIBRARIES)

