cmake_minimum_required(VERSION 3.0)

project(QWT)

#
# This uses Qt, this is common CMake configuration when building with Qt
#
find_program(QT_QMAKE_EXECUTABLE qmake HINTS ${QT_BINARY_DIR})
if (${QT_QMAKE_EXECUTABLE} MATCHES QT_QMAKE_EXECUTABLE-NOTFOUND)
	if (NOT DEFINED QT_BINARY_DIR)
		message(FATAL_ERROR "qmake is not in your path, try defining QT_BINARY_DIR to your QT installation /bin path")
	else()
		message(FATAL_ERROR "qmake is not in your path, including your QT_BINARY_DIR (${QT_BINARY_DIR})")
	endif()
endif()

add_subdirectory(src)
