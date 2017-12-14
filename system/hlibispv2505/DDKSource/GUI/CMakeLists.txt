
find_package(FelixAPI QUIET)

message (STATUS "== Build GUIs ==")

if (CMAKE_COMPILER_IS_GNUCXX)
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.7)
        add_definitions(-std=c++0x)
    else()
        add_definitions(-std=c++11)
    endif()
endif()

set(QTEXTRA_UNIT_TEST ${BUILD_UNIT_TESTS})
if (WIN32)
	option(FELIXPARAMGUI_WIN32EXE "Hide the command line when running GUIs" ON)
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
	${CMAKE_CURRENT_SOURCE_DIR}/qtextra
	${CMAKE_CURRENT_SOURCE_DIR}/EasyPlot
	${CMAKE_CURRENT_SOURCE_DIR}/paramGui
    ${CMAKE_CURRENT_SOURCE_DIR}/tuning
)

#
# done in top level cmake because some external libraries need the flags too
#
#include(GUI/CheckQtLocation.cmake)

#
# end of generic
#

add_subdirectory(qtextra)
add_subdirectory(EasyPlot)

if (${BUILD_ISPCONTROL} AND ${FELIXAPI_FOUND})

	if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tuning")
		message(STATUS "-- VisionTuning")
		add_subdirectory(tuning)
	endif()
	
	if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/live")
        message(STATUS "-- VisionLive v1")
        add_subdirectory(live)
    endif()
	
	if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/live2")
		message(STATUS "-- VisionLive v2")
		add_subdirectory(live2)
	endif()
	
else()
    message("Skipping VisionLive and VisionTuning")
endif()