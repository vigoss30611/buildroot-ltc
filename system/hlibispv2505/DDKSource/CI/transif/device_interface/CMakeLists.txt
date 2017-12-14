cmake_minimum_required(VERSION 2.8)

project(DeviceInterfaceTransif)

find_package(DeviceInterfaceTransif REQUIRED)

# ----------------------------------------------------------------------
# External dependencies
# ----------------------------------------------------------------------
find_package(ImgIncludes REQUIRED)
find_package(DeviceInterface REQUIRED)
find_package(Transif REQUIRED)

include_directories(${IMGINCLUDES_INCLUDE_DIRS})
include_directories(${DEVICEIF_INCLUDE_DIRS})
include_directories(${TRANSIF_INCLUDE_DIRS})

add_definitions(${IMGINCLUDE_DEFINITIONS})

# ----------------------------------------------------------------------
# Internal headers
# ----------------------------------------------------------------------
#include_directories(include)

# ----------------------------------------------------------------------
# List of sources
# ----------------------------------------------------------------------
set(HEADERS
	device_interface_transif.h
)

set(SOURCES
	device_interface_transif.cpp
)

add_library(DeviceInterfaceTransif STATIC ${SOURCES} ${HEADERS})

