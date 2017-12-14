# This file is designed to be included in a top level CMakeLists.txt like so:
#
#     include(path/to/here/CheckQtLocation.cmake)
#
# It will check that the needed modules locations are either guessable or found

macro(QtLibDefined LIBNAME)
if(NOT DEFINED ${LIBNAME}_DIR)
	set(${LIBNAME}_DIR ${QT_LIBRARY_CMAKE_DIR}/${LIBNAME})
	message("${LIBNAME}_DIR is not defined: guessing location from QT_LIBRARY_CMAKE_DIR is '${${LIBNAME}_DIR}' (if incorrect clear cache and define variable)")
endif()
endmacro()

#
# This uses Qt, this is common CMake configuration when building with Qt - done at top level to avoid repeating it
#
find_program(QT_QMAKE_EXECUTABLE qmake HINTS ${QT_BINARY_DIR})
if (${QT_QMAKE_EXECUTABLE} MATCHES QT_QMAKE_EXECUTABLE-NOTFOUND)
	if (NOT DEFINED QT_BINARY_DIR)
		message(FATAL_ERROR "qmake is not in your path, try defining QT_BINARY_DIR to your QT installation /bin path")
	else()
		message(FATAL_ERROR "qmake is not in your path, including your QT_BINARY_DIR (${QT_BINARY_DIR})")
	endif()
endif()
#set(QWT_OPENGL TRUE PARENT_SCOPE) # turn on Qt OpenGL support for this library

if(DEFINED QT_BINARY_DIR)
	if(NOT DEFINED QT_LIBRARY_CMAKE_DIR)
		set(QT_LIBRARY_CMAKE_DIR ${QT_BINARY_DIR}/../lib/cmake/)
		message("QT_LIBRARY_CMAKE_DIR is not defined - guess from QT_BINARY_DIR is '${QT_LIBRARY_CMAKE_DIR}' (if incorrect clear cache and define variable)")
	endif()
endif()
	
QtLibDefined(Qt5Core)
QtLibDefined(Qt5Widgets)
QtLibDefined(Qt5Gui)
QtLibDefined(Qt5WebKit)
QtLibDefined(Qt5WebKitWidgets)
QtLibDefined(Qt5Sensors)
QtLibDefined(Qt5Positioning)
QtLibDefined(Qt5Quick)
QtLibDefined(Qt5Qml)
QtLibDefined(Qt5Network)
QtLibDefined(Qt5Multimedia)
QtLibDefined(Qt5WebChannel)
QtLibDefined(Qt5Sql)
QtLibDefined(Qt5MultimediaWidgets)
QtLibDefined(Qt5OpenGL)
QtLibDefined(Qt5Test)
QtLibDefined(Qt5PrintSupport)
QtLibDefined(Qt5Concurrent)
QtLibDefined(Qt5Help)
	
if (DEBUG_MODULES)
	message("Qt5Concurrent_DIR=${Qt5Concurrent_DIR}")
endif()
