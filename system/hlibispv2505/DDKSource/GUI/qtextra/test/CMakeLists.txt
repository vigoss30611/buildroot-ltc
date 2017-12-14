cmake_minimum_required (VERSION 3.2)

project(QtExtraTests)


find_package(Qt5Widgets REQUIRED)
find_package(Qt5Test REQUIRED)
find_package(Qt5Help REQUIRED)
find_package(QtExtra REQUIRED)

set (QT_USE_QTGUI TRUE)
set (QT_USE_QTTEST TRUE)
#include(${QT_USE_FILE})

# CMP0020 NEW: link automatically to qtmain.lib when linking with QtCore for WIN32
# CMP0020 OLD: do not link with qtmain.lib automatically for WIN32
#cmake_policy(SET CMP0020 OLD)

include_directories(${QTEXTRA_INCLUDE_DIRS} ${CMAKE_CURRENT_BINARY_DIR})
add_definitions(
  ${Qt5Widgets_DEFINITIONS} 
  ${Qt5Test}
  ${Qt5Help}
  ${QTEXTRA_DEFINITIONS})

set (SOURCES
	doubleslider_test.cpp
	#main_test.cpp
)
set (HEADERS
	doubleslider_test.hpp
)

set(HEADERS_MOC) # clean
QT5_WRAP_CPP(HEADERS_MOC ${HEADERS})

add_executable(${QTEXTRA_NAME}_test ${SOURCES} ${HEADERS_MOC} ${HEADERS})
set_source_files_properties(${HEADERS_MOC} PROPERTIES GENERATED TRUE)

target_link_libraries(${QTEXTRA_NAME}_test ${QTEXTRA_LIBRARIES} Qt5::Widgets Qt5::Test Qt5::Help) # including ${QT_QTTEST_LIBRARY})
# that's not gtest... so we disable it from auto testing
#add_test(NAME ${QTEXTRA_NAME}_test COMMAND ${QTEXTRA_NAME}_test --gtest_output=xml:xunits_qtextra.xml)

if (WIN32)
	set(DEBUG_DLLS) # empty
	GetQt5OtherDLLs(DEBUG_DLLS) # find all dlls not being Qt modules but needed
	set(RELEASE_DLLS ${DEBUG_DLLS})
	# now both debug and release have by default all dlls that are not modules
	
	GetQt5DLLs(Qt5::Core DEBUG_DLLS RELEASE_DLLS)
	GetQt5DLLs(Qt5::Widgets DEBUG_DLLS RELEASE_DLLS)
	GetQt5DLLs(Qt5::Test DEBUG_DLLS RELEASE_DLLS)
	GetQt5DLLs(Qt5::Help DEBUG_DLLS RELEASE_DLLS)
	
	foreach(dll ${DEBUG_DLLS} ${RELEASE_DLLS})
		file(COPY ${dll} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
	endforeach()
endif()
