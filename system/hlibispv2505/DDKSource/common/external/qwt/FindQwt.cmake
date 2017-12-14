#
# find your local intallation of Qwt library
#
# inputs:
#        QWT_PREFIX location of the library, automatically found if not specified
#
# outputs:
#        QWT_FOUND - defined if the library is found
#        QWT_DEFINITIONS - C flags to enable testing
#        QWT_INCLUDE_DIRS - header file directory
#        QWT_LIBRARIES - path to the library
#
cmake_minimum_required (VERSION 2.8)

get_filename_component (QWT_PREFIX "${CMAKE_CURRENT_LIST_FILE}" PATH)
if (NOT DEFINED QWT_INSTALL_DIR)
	set (QWT_INSTALL_DIR ${CMAKE_BINARY_DIR}/downloads/install/qwt)
endif()

set (QWT_FOUND TRUE)
set (QWT_DEFINITIONS)
set (QWT_INCLUDE_DIRS ${QWT_INSTALL_DIR}/include ${QWT_INSTALL_DIR})
set (QWT_INCLUDE_DIR ${QWT_INCLUDE_DIRS})
set (QWT_NAME qwt-external)

if(WIN32)
  if(MINGW)
    set(QWT_LIBRARIES ${QWT_INSTALL_DIR}/lib/libqwt.a)
  else()
    set(QWT_LIBRARIES ${QWT_INSTALL_DIR}/lib/qwt.lib)
  endif()
else()
  set(QWT_LIBRARIES ${QWT_INSTALL_DIR}/lib/libqwt.a)
endif()

if (DEBUG_MODULES)
	message ("QWT_PREFIX=${QWT_PREFIX}")
	message ("QWT_LIBRARIES=${QWT_LIBRARIES}")
	message ("QWT_INCLUDE_DIRS=${QWT_INCLUDE_DIRS}")
endif()

mark_as_advanced (QWT_LIBRARIES QWT_DEBUG_LIBRARIES)
