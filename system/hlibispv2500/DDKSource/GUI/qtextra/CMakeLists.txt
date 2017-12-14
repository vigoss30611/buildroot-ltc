cmake_minimum_required (VERSION 3.2)

#
# apparently for Qt this HAS TO be first
#
project(QtExtra)
message (STATUS "-- QtExtra")

find_package(Qt5Widgets REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(QtExtra REQUIRED)
find_package(Qwt QUIET) # optional

if (NOT DEFINED QTEXTRA_UNIT_TEST)
	set (QTEXTRA_UNIT_TEST FALSE)
endif()

#set (QT_USE_QTGUI TRUE)
#include(${QT_USE_FILE})

include_directories(${Qt5Widgets_INCLUDES} ${QTEXTRA_INCLUDE_DIRS})
add_definitions(${Qt5Widgets_DEFINITIONS} ${QTEXTRA_DEFINITIONS})

set (SOURCES
	source/doubleslider.cpp
	source/boxslider.cpp
	source/exception.cpp
	source/filewidget.cpp
	source/textparser.cpp
    source/scalableview.cpp
)
set (HEADERS
	include/QtExtra/doubleslider.hpp
	include/QtExtra/boxslider.hpp
	include/QtExtra/exception.hpp
	include/QtExtra/ostream.hpp
	include/QtExtra/filewidget.hpp
	include/QtExtra/scalableview.hpp
	include/QtExtra/textparser.hpp
)
set (FORMS
	forms/dialog_textparser.ui
)
# include the directory where the forms are generating the uis (eg: #include "ui/ui_dialog_textparser.h")
include_directories(${CMAKE_CURRENT_BINARY_DIR})

if (${QWT_FOUND})
	message (STATUS "   QWT extra")

	include_directories(${QWT_INCLUDE_DIRS})
	add_definitions(${QWT_DEFINITIONS})

	set (SOURCES ${SOURCES}
		source/editablecurve.cpp
	)
	set (HEADERS ${HEADERS}
		include/QwtExtra/editablecurve.hpp
	)

endif()

if (Qt5_POSITION_INDEPENDENT_CODE)
  set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

set(HEADERS_MOC) # clean
QT5_WRAP_CPP(HEADERS_MOC ${HEADERS})

QtWrapUI(FORMS_HEADERS "${FORMS}" ${CMAKE_CURRENT_BINARY_DIR}/ui)
#QT5_WRAP_UI(FORMS_HEADERS "${FORMS}")

add_library(${QTEXTRA_NAME} ${SOURCES} ${HEADERS_MOC} ${HEADERS} ${FORMS_HEADERS})
set_source_files_properties(${HEADERS_MOC} ${FORMS_HEADERS} PROPERTIES GENERATED TRUE)
source_group(MOC FILES ${HEADERS_MOC})
source_group(UI_H FILES ${FORMS_HEADERS})
source_group(UI FILES ${FORMS})

# if QWT is not found the QWT_LIBRARIES is empty and has no effect
target_link_libraries(${QTEXTRA_NAME} Qt5::Widgets ${QWT_LIBRARIES})
if(DEFINED QWT_NAME)
	add_dependencies(${QTEXTRA_NAME} ${QWT_NAME})
endif()

if (QTEXTRA_UNIT_TEST)
	add_subdirectory(example)
	add_subdirectory(test)
endif()
