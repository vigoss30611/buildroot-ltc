cmake_minimum_required(VERSION 2.8)

project(DEVICEIFSlave)

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
# List of sources
# ----------------------------------------------------------------------
set(HEADERS
	./device_interface_slave.h
	./device_interface_slave_tcp.h
	./device_interface_slave_socket.h
	./device_interface_slave_filereader.h
	)

set(SOURCES
	./device_interface_slave.cpp
	./device_interface_slave_tcp.cpp
	./device_interface_slave_socket.cpp
	./device_interface_slave_filereader.cpp
	)

if (NOT ${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set(INCLUDES ${INCLUDES}
      device_interface_slave_socket_unix.h
    )
    set(SOURCES ${SOURCES}
      device_interface_slave_socket_unix.cpp
    )
endif()
	
# ----------------------------------------------------------------------
# Build the library
# ----------------------------------------------------------------------
add_library(${DEVICEIF_SLA_LIBRARIES} STATIC ${SOURCES} ${HEADERS})
target_link_libraries(${DEVICEIF_SLA_LIBRARIES} ${DEVICEIF_COM_LIBRARIES})
IF(MSVC)
	# we need to pull in the winsock library when building inside Visual Studio
	target_link_libraries(${DEVICEIF_SLA_LIBRARIES} ws2_32)
ENDIF()
