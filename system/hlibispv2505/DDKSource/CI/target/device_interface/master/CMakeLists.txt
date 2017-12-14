cmake_minimum_required(VERSION 2.8)

project(DEVICEIFMaster)

find_package(DeviceInterface REQUIRED) # trick to avoid duplication

# ----------------------------------------------------------------------
# External dependencies
# ----------------------------------------------------------------------
find_package(ImgIncludes REQUIRED)

include_directories(${IMGINCLUDES_INCLUDE_DIRS}
					${DEVICEIF_INCLUDE_DIRS})

add_definitions(${IMGINCLUDES_DEFINITIONS}
				${DEVICEIF_DEFINITIONS})

set(THREADSAFE_LIB)
if (DEVIF_USE_MEOS)
    find_package(MeOSAL REQUIRED)
    include_directories(${MEOSAL_INCLUDE_DIRS})
    add_definitions(
        ${MEOSAL_DEFINITIONS}
        -DMEOS_MUTEX
    )
    set(THREADSAFE_LIB ${THREADSAFE_LIB} ${MEOSAL_LIBRARIES})
elseif(DEVIF_USE_OSA)
    find_package(OSA REQUIRED)
    include_directories(${OSA_INCLUDE_DIRS})
    add_definitions(
        ${OSA_DEFINITIONS}
        -DOSA_MUTEX
    )
    set(THREADSAFE_LIB ${THREADSAFE_LIB} ${OSA_LIBRARIES})
endif()

# ----------------------------------------------------------------------
# Internal headers
# ----------------------------------------------------------------------
#include_directories(include)

# ----------------------------------------------------------------------
# List of sources
# ----------------------------------------------------------------------
set(INCLUDES
  device_interface_master.h
  device_interface_master_socket.h
  device_interface_master_tcp.h
)

set(HEADERS
)

set(SOURCES
  device_interface_master.cpp
  device_interface_master_tcp.cpp
  device_interface_master_socket.cpp
)

if (NOT ${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set(INCLUDES ${INCLUDES}
      device_interface_master_unix.h
    )
    set(SOURCES ${SOURCES}
      device_interface_master_unix.cpp
    )
endif()

# ----------------------------------------------------------------------
# Build the library
# ----------------------------------------------------------------------
add_library(${DEVICEIF_MAS_LIBRARIES} STATIC ${SOURCES} ${HEADERS} ${INCLUDES})
target_link_libraries(${DEVICEIF_MAS_LIBRARIES} ${DEVICEIF_COM_LIBRARIES} ${THREADSAFE_LIB})

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  # we need to pull in the winsock library when building inside Visual Studio
  target_link_libraries(${DEVICEIF_MAS_LIBRARIES} ws2_32)
endif()

