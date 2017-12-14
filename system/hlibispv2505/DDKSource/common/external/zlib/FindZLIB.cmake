# Locate and configure the downloaded zlib library (windows only)
#
#
# Defines the following variables:
#
#   ZLIB_FOUND - Found the ImgLib GZIPIO
#   ZLIB_INCLUDE_DIRS - The directories needed on the include paths
#   ZLIB_LIBRARIES - The libraries to link to

# set ZLIB_PREFIX to directory where the current file file is located
get_filename_component(ZLIB_PREFIX "${CMAKE_CURRENT_LIST_FILE}" PATH)
if (NOT DEFINED ZLIB_INSTALL_DIR)
	set(ZLIB_INSTALL_DIR ${CMAKE_BINARY_DIR}/downloads/install/zlib)
endif()
set(ZLIB_NAME zlib-external)


set(ZLIB_FOUND TRUE)
set(ZLIB_INCLUDE_DIRS ${ZLIB_INSTALL_DIR}/include ${ZLIB_INSTALL_DIR})
set(ZLIB_INCLUDE_DIR ${ZLIB_INCLUDE_DIRS})

if(WIN32)
  if(MINGW)
    set(ZLIB_LIBRARIES ${ZLIB_INSTALL_DIR}/lib/libzlib.a debug ${ZLIB_INSTALL_DIR}/lib/libzlibd.a)
  else()
    set(ZLIB_LIBRARIES optimized ${ZLIB_INSTALL_DIR}/lib/zlibstatic.lib debug ${ZLIB_INSTALL_DIR}/lib/zlibstaticd.lib)
  endif()
else()
  set(ZLIB_LIBRARIES ${ZLIB_INSTALL_DIR}/lib/libz.a)
endif()

set(ZLIB_LIBRARY ${ZLIB_LIBRARIES})

if(DEBUG_MODULES)
  message("ZLIB_INCLUDE_DIRS = ${ZLIB_INCLUDE_DIRS}")
  message("ZLIB_LIBRARIES = ${ZLIB_LIBRARIES}")
endif()

mark_as_advanced(ZLIB_INCLUDE_DIRS ZLIB_INCLUDE_DIR ZLIB_LIBRARIES ZLIB_LIBRARY)

