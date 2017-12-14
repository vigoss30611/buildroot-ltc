cmake_minimum_required (VERSION 2.8)

find_package(GTest REQUIRED)

# CMP0020 NEW: link automatically to qtmain.lib when linking with QtCore for WIN32
# CMP0020 OLD: do not link with qtmain.lib automatically for WIN32
cmake_policy(SET CMP0020 OLD)

include_directories(
	${GTEST_INCLUDE_DIRS}
)

add_definitions(
	${GTEST_DEFINITIONS}
)

set(SOURCES
	ccm_tests.cpp
	main.cpp
)

add_executable(${GUI_NAME}_unit_tests ${SOURCES})
target_link_libraries(${GUI_NAME}_unit_tests ${GUI_NAME}_lib ${GTEST_LIBRARIES})

if (WIN32)
	set(DEBUG_DLLS) # empty
	GetQt5OtherDLLs(DEBUG_DLLS) # find all dlls not being Qt modules but needed
	set(RELEASE_DLLS ${DEBUG_DLLS})
	
	# now both debug and release have by default all dlls that are not modules
	GetQt5DLLs(Qt5::Core DEBUG_DLLS RELEASE_DLLS)
	GetQt5DLLs(Qt5::Widgets DEBUG_DLLS RELEASE_DLLS)
	
	CopyDLLs(DEBUG_DLLS, RELEASE_DLLS, CMAKE_CURRENT_BINARY_DIR)
endif()

if (DEFINED FELIXAPI_INSTALL)
	file(COPY ${FELIXAPI_INSTALL} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
    install(FILES ${FELIXAPI_INSTALL} DESTINATION ./CI/)
endif()

add_test(${GUI_NAME} ${GUI_NAME}_unit_tests --gtest_output=xml:xunits_visiontuning.xml)