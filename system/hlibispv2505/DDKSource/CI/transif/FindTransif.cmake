cmake_minimum_required(VERSION 2.8)

# Locate and configure the Transif library.
#
# Possible options:
#
#	TRANSIF_BUS_WIDTH			- the bus width, typically 128 or 256 bits (REQUIRED)
#	TRANSIF_LIBRARY_NAME		- the name of the dll, will default to transif
#	TRANSIF_LINK_LIBS			- list of libraries that need to be linked into the transif DLL
#	TRANSIF_LINK_IMPORT_LIBS	- same as TRANSIF_LINK_LIBS but using the --whole-archive linker flag
#	TRANSIF_TIMED_MODEL			- set to yes to enable timed model, otherwise untimed model assumed
#	TRANSIF_NO_PDUMP_PLAYER		- set to yes to omit built-in pdumpplayer code
#	PTHREAD_INCLUDE_DIRS		- if set, then look for the pthread include files in this directory
#
# Defines the following variables:
#
#   TRANSIF_FOUND			- Set to TRUE on success
#   TRANSIF_DEFINITIONS		- Any flags that should be added with add_definitions()
#   TRANSIF_INCLUDE_DIRS	- The directories needed on the include paths for use with include_directories()
#   TRANSIF_LIBRARIES		- The libraries to link for use with target_link_libraries()

if (NOT DEFINED TRANSIF_LIBRARY_NAME)
	if (TRANSIF_BUILD_MASTER)
		set (TRANSIF_LIBRARY_NAME transif_master)
	elseif (TRANSIF_BUILD_SLAVE)
		set(TRANSIF_LIBRARY_NAME transif_slave)
	else()
		set(TRANSIF_LIBRARY_NAME transif)
	endif()
endif()

# set TRANSIF_PREFIX to directory where the current file file is located
get_filename_component(TRANSIF_PREFIX "${CMAKE_CURRENT_LIST_FILE}" PATH)

set (TRANSIF_FOUND			TRUE)
set (TRANSIF_DEFINITIONS	"")
set (TRANSIF_INCLUDE_DIRS	${TRANSIF_PREFIX} ${TRANSIF_PREFIX}/mem_models)
set (TRANSIF_LIBRARIES		${TRANSIF_LIBRARY_NAME})

if (DEBUG_MODULES)
    message("TRANSIF_LIBRARY_NAME       = ${TRANSIF_LIBRARY_NAME}")
    message("TRANSIF_DEFINITIONS        = ${TRANSIF_DEFINITIONS}")
    message("TRANSIF_INCLUDE_DIRS       = ${TRANSIF_INCLUDE_DIRS}")
    message("TRANSIF_LIBRARIES          = ${TRANSIF_LIBRARIES}")
endif()

mark_as_advanced(TRANSIF_DEFINITIONS TRANSIF_INCLUDE_DIRS TRANSIF_LIBRARIES)
