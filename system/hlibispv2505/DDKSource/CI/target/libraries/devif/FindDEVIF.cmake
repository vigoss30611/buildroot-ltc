cmake_minimum_required (VERSION 2.8)

# ----------------------------------------------------------------------
# Locate and configure the DEVIF library.
#
# Possible options:
#   DEVIF_DEFINE_DEVIF1_FUNCS - Define DEV1 funcs (default: FALSE)
#	DEVIF_DEBUG - Enable debug file output from DEVIF
#	DEVIF_USE_DEVICEINTERFACE - enable device interface code
#	DEVIF_EXCLUDE - list of interfaces to exclude
#	DEVIF_PCI_DRIVER_HEADERS - Path to PCI driver headers

# Defines the following variables:
#
#   DEVIF_FOUND			- Found the library
#   DEVIF_PREFIX			- The directory where this file is located
#   DEVIF_INCLUDE_DIRS	- The directories needed on the include paths 
#						  for this library
#   DEVIF_DEFINITIONS	- The definitions to apply for this library
#   DEVIF_LIBRARIES		- The libraries to link to
# ----------------------------------------------------------------------

# ----------------------------------------------------------------------
# Set package variables
# ----------------------------------------------------------------------
get_filename_component(DEVIF_PREFIX "${CMAKE_CURRENT_LIST_FILE}" PATH)

set(DEVIF_FOUND TRUE)
set(DEVIF_INCLUDE_DIRS ${DEVIF_PREFIX}/include)
set(DEVIF_LIBRARIES devif)
set(DEVIF_DEFINITIONS)

if(DEVIF_DEFINE_DEVIF1_FUNCS)
    set(DEVIF_DEFINITIONS ${DEVIF_DEFINITIONS} -DDEVIF_DEFINE_DEVIF1_FUNCS)
endif()

mark_as_advanced(DEVIF_INCLUDE_DIRS DEVIF_DEFINITIONS DEVIF_LIBRARIES)


