cmake_minimum_required(VERSION 2.8)

project(minterp)

if (${CMAKE_CURRENT_SOURCE_DIR} STREQUAL ${CMAKE_SOURCE_DIR})
	if (NOT DEFINED DDKSource_DIR)
		message(FATAL_ERROR "Need DDKSource_DIR")
	endif()
	message (STATUS "DDKSource_DIR=${DDKSource_DIR}")

	include(${DDKSource_DIR}/common/UsePackage.cmake)
	include (${DDKSource_DIR}/build/OverwriteFlags.cmake)

	add_subdirectory(${DDKSource_DIR}/common "ddk_common")
endif()	

find_package(ImgIncludes REQUIRED)
find_package(FelixCommon REQUIRED)

include_directories(
  ${IMGINCLUDES_INCLUDE_DIRS}
  ${FELIXCOMMON_INCLUDE_DIRS}
)

add_definitions(
	${IMGINCLUDES_DEFINITIONS}
	${FELIXCOMMON_DEFINITIONS}
)

add_executable(minterp minterp.cpp)
target_link_libraries(minterp ${FELIXCOMMON_LIBRARIES})

install(TARGETS minterp DESTINATION utils)
