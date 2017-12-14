# Locate and configure the downloaded Mini-XML library
#
#
# Defines the following variables:
#
#   MXML_FOUND - Found the MXML library
#   MXML_INCLUDE_DIRS - The directories needed on the include paths
#   MXML_LIBRARIES - The libraries to link to
#   MXML_PREFIX - where the MXML library patch are located (used internally)
#   MXML_INSTALL_DIR - where to install the MXML library (used internally)
#   MXML_NAME - name of the project (use it to add dependency)

# set MXML_PREFIX to directory where the current file file is located
get_filename_component(MXML_PREFIX "${CMAKE_CURRENT_LIST_FILE}" PATH)
if(NOT DEFINED MXML_INSTALL_DIR)
	set(MXML_INSTALL_DIR ${CMAKE_BINARY_DIR}/downloads/install/mxml)
endif()
set(MXML_NAME mxml-external)

set(MXML_FOUND TRUE)
set(MXML_INCLUDE_DIRS ${MXML_INSTALL_DIR}/include ${MXML_INSTALL_DIR})
set(MXML_INCLUDE_DIR ${MXML_INSTALL_DIR}/include ${MXML_INSTALL_DIR})

if(WIN32 AND NOT MINGW)
  set(MXML_LIBRARIES optimized ${MXML_INSTALL_DIR}/lib/libmxml_static.lib debug ${MXML_INSTALL_DIR}/lib/libmxml_staticd.lib)
else()
  set(MXML_LIBRARIES ${MXML_INSTALL_DIR}/lib/libmxml.a)
endif()

if(DEBUG_MODULES)
  message("MXML_PREFIX = ${MXML_PREFIX}")
  message("MXML_INCLUDE_DIRS = ${MXML_INCLUDE_DIRS}")
  message("MXML_INSTALL_DIR = ${MXML_INSTALL_DIR}")
  message("MXML_LIBRARIES = ${MXML_LIBRARIES}")
endif()

mark_as_advanced(MXML_INCLUDE_DIRS MXML_LIBRARIES)

