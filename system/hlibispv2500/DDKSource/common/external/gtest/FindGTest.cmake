#
# find your local installation of google test
#
# inputs:
#        GTEST_PREFIX location of the library, automatically found if not specified
#
# outputs:
#        GTEST_FOUND - defined if the library is found
#        GTEST_DEFINITIONS - C flags to enable testing
#        GTEST_INCLUDE_DIRS - header file directory
#        GTEST_LIBRARIES - path to the library
#
cmake_minimum_required (VERSION 2.8)

get_filename_component (GTEST_PREFIX "${CMAKE_CURRENT_LIST_FILE}" PATH)
if (NOT DEFINED GTEST_INSTALL_DIR)
	set (GTEST_INSTALL_DIR ${CMAKE_BINARY_DIR}/downloads/install/gtest)
endif()

set (GTEST_FOUND TRUE)
set (GTEST_DEFINITIONS)
set (GTEST_INCLUDE_DIRS ${GTEST_INSTALL_DIR}/include ${GTEST_INSTALL_DIR})
set (GTEST_INCLUDE_DIR ${GTEST_INCLUDE_DIRS})
set (GTEST_NAME gtest-external)

set (GTEST_LIBRARIES gtest_main gtest)
if(WIN32)
  if(MINGW)
    set(GTEST_LIBRARIES ${GTEST_INSTALL_DIR}/lib/libgtest_main.a ${GTEST_INSTALL_DIR}/lib/libgtest.a)
  else()
    set(GTEST_LIBRARIES ${GTEST_INSTALL_DIR}/lib/gtest_main.lib ${GTEST_INSTALL_DIR}/lib/gtest.lib)
  endif()
else()
  set(GTEST_LIBRARIES ${GTEST_INSTALL_DIR}/lib/libgtest_main.a ${GTEST_INSTALL_DIR}/lib/libgtest.a -lpthread)
endif()

if( MSVC ) # VS2012 doesn't support correctly the tuples yet
	set (GTEST_DEFINITIONS ${GTEST_DEFINITIONS}  /D_VARIADIC_MAX=10)
endif()

if (DEBUG_MODULES)
	message ("GTEST_PREFIX=${GTEST_PREFIX}")
	message ("GTEST_LIBRARIES=${GTEST_LIBRARIES}")
	message ("GTEST_INCLUDE_DIRS=${GTEST_INCLUDE_DIRS}")
endif()

mark_as_advanced (GTEST_LIBRARIES GTEST_DEBUG_LIBRARIES)
