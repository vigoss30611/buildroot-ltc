
find_package(FelixAPI QUIET)

message (STATUS "== Build GUIs ==")

set(QTEXTRA_UNIT_TEST ${BUILD_UNIT_TESTS})
if (WIN32)
	option(FELIXPARAMGUI_WIN32EXE "Hide the command line when running GUIs" ON)
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
	${CMAKE_CURRENT_SOURCE_DIR}/qtextra
	${CMAKE_CURRENT_SOURCE_DIR}/paramGui
    ${CMAKE_CURRENT_SOURCE_DIR}/tuning
)

#
# end of generic
#

add_subdirectory(qtextra)

if (${BUILD_ISPCONTROL} AND ${FELIXAPI_FOUND})
	message(STATUS "-- VisionTuning")
	add_subdirectory(tuning)
	
	message(STATUS "-- VisionLive v2")
    add_subdirectory(live2)

    if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/live")
        message(STATUS "-- VisionLive v1")
        add_subdirectory(live)
    endif()
else()
    message("Skipping VisionLive and VisionTuning")
endif()