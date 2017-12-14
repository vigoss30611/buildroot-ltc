cmake_minimum_required (VERSION 3.2)

find_package(Qt5Widgets REQUIRED)
find_package(QtExtra REQUIRED)

set(QT_USE_QTGUI TRUE)
#include(${QT_USE_FILE})

# CMP0020 NEW: link automatically to qtmain.lib when linking with QtCore for WIN32
# CMP0020 OLD: do not link with qtmain.lib automatically for WIN32
#cmake_policy(SET CMP0020 OLD)

include_directories(${QTEXTRA_INCLUDE_DIRS})
add_definitions(${Qt5Widgets_DEFINITIONS} ${QTEXTRA_DEFINITIONS})

set (SOURCES
	qtextra_example.cpp
)
set (HEADERS
	swapbutton.hpp
)

set (HEADERS_MOC)
QT5_WRAP_CPP(HEADERS_MOC ${HEADERS})
add_executable(${QTEXTRA_NAME}_example ${SOURCES} ${HEADERS_MOC} ${HEADERS})
target_link_libraries(${QTEXTRA_NAME}_example ${QTEXTRA_LIBRARIES} Qt5::Widgets)

if (WIN32)
	set(DEBUG_DLLS) # empty
	GetQt5OtherDLLs(DEBUG_DLLS) # find all dlls not being Qt modules but needed
	set(RELEASE_DLLS ${DEBUG_DLLS})
	# now both debug and release have by default all dlls that are not modules
	
	GetQt5DLLs(Qt5::Core DEBUG_DLLS RELEASE_DLLS)
	GetQt5DLLs(Qt5::Widgets DEBUG_DLLS RELEASE_DLLS)
	GetQt5DLLs(Qt5::Gui DEBUG_DLLS RELEASE_DLLS)
	
	foreach(dll ${DEBUG_DLLS} ${RELEASE_DLLS})
		file(COPY ${dll} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
	endforeach()
endif()
