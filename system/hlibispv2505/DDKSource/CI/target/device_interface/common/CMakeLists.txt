cmake_minimum_required(VERSION 2.8)

project(DEVICEIFCommon)

find_package(DeviceInterface REQUIRED) # trick to avoid duplication

# ----------------------------------------------------------------------
# External dependencies
# ----------------------------------------------------------------------
find_package(ImgIncludes REQUIRED)

include_directories(${IMGINCLUDES_INCLUDE_DIRS})
include_directories(${DEVICEIF_INCLUDE_DIRS})

add_definitions(${IMGINCLUDES_DEFINITIONS})
add_definitions(${DEVICEIF_DEFINITIONS})

# ----------------------------------------------------------------------
# Internal headers
# ----------------------------------------------------------------------
#include_directories(include)

# ----------------------------------------------------------------------
# List of sources
# ----------------------------------------------------------------------
set(INCLUDES
device_interface.h
device_interface_message.h
)

set(HEADERS
)

set(SOURCES
device_interface.cpp
)

# ----------------------------------------------------------------------
# Build the library
# ----------------------------------------------------------------------
add_library(${DEVICEIF_COM_LIBRARIES} STATIC ${SOURCES} ${HEADERS} ${INCLUDES})

