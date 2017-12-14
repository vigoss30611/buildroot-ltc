cmake_minimum_required (VERSION 2.8)

project(ParamSocket2)

find_package(ParamSocket2 REQUIRED)
find_package(ImgIncludes REQUIRED) # needed for FelixParam

message (STATUS "-- Param Socket 2")


include_directories(${IMGINCLUDES_INCLUDE_DIRS} ${PARAMSOCKET2_INCLUDE_DIRS})
add_definitions(${IMGINCLUDES_DEFINITIONS} ${PARAMSOCKET2_DEFINITIONS})
#add_definitions(-DSOCK_DBG)

set (SOURCES
	src/paramsocket.cpp
	src/paramsocket_server.cpp
	src/paramsocket_client.cpp
)

set (HEADERS
	include/paramsocket/paramsocket.hpp
	include/paramsocket/demoguicommands.hpp
)

add_library(${PARAMSOCKET2_NAME} ${SOURCES} ${HEADERS})
target_link_libraries(${PARAMSOCKET2_NAME})

