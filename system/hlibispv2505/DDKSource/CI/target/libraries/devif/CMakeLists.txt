cmake_minimum_required(VERSION 2.8)

project(DEVIF)

message(STATUS "DEVIF Device Interface")

find_package(DEVIF REQUIRED) # trick to avoid duplication

# ----------------------------------------------------------------------
# External dependencies
# ----------------------------------------------------------------------
find_package(ImgIncludes REQUIRED)

include_directories(
	${IMGINCLUDES_INCLUDE_DIRS}
)

add_definitions(
	${IMGINCLUDES_DEFINITIONS}
	${DEVIF_DEFINITIONS}
)

if (DEVIF_DEBUG)
	add_definitions(-DDEVIF_DEBUG)
endif()
#if(DEVIF_USE_OSA)
#    add_definitions(-DOSA_MUTEX)
#elseif(DEVIF_USE_MEOS)
#    add_definitions(-DMEOS_MUTEX)
#endif()

# ----------------------------------------------------------------------
# Common header files
# ----------------------------------------------------------------------
include_directories(include codescape)

set (EXTRA
	FindDEVIF.cmake
)

set(HEADERS
	include/devif_api.h
	include/devif_config.h
	include/devif_setup.h
  codescape/DAtinyscript.h
)

set(SOURCES
	code/devif.c
	code/devif_debug.c
)

# ----------------------------------------------------------------------
# Setup Device_interface if requested
# ----------------------------------------------------------------------

if(DEVIF_USE_DEVICEINTERFACE)
	list(FIND DEVIF_EXCLUDE socket soc_index)
	list(FIND DEVIF_EXCLUDE direct dir_index)
	if ((${soc_index} EQUAL -1) OR (${dir_index} EQUAL -1)) # Check socket and direct are not all in the exclude list
	    find_package(DeviceInterface REQUIRED)
    	include_directories(${DEVICEIF_INCLUDE_DIRS})
    	add_definitions(${DEVICEIF_DEFINITIONS})
        		
		set(DEVIF_LINK_LIB ${DEVIF_LINK_LIB} ${DEVICEIF_MAS_LIBRARIES})
	endif()
else()
	list(APPEND DEVIF_EXCLUDE socket direct)
endif()

# No PCI support in MacOs at the moment
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	list(APPEND DEVIF_EXCLUDE pci)
endif()



# ----------------------------------------------------------------------
# Add option source files
# ----------------------------------------------------------------------
# Check to see if PCI interface is excluded
list(FIND DEVIF_EXCLUDE pci index)
if(${index} EQUAL -1)
	include_directories(${DEVIF_PCI_DRIVER_HEADERS})
	add_definitions(-DENABLE_PCI_DEVIF)
	if ((${META} MATCHES true) OR (ANDROID))
		set(SOURCES ${SOURCES} code/pciif.c code/phys_to_virt.c)			
	else()
		set(SOURCES ${SOURCES} code/pciif.c code/phys_to_virt.cpp)
	endif()
else()
	list(APPEND DEVIF_EXCLUDE posted bmem)
endif()
	
# Check to see if Burnmem interface is excluded
list(FIND DEVIF_EXCLUDE bmem index)
if(${index} EQUAL -1)
	add_definitions(-DENABLE_BMEM_DEVIF)
	set(SOURCES ${SOURCES} code/bmem.c)
endif()
	
# Check to see if Socket interface is excluded
list(FIND DEVIF_EXCLUDE socket index)
if(${index} EQUAL -1)
	add_definitions(-DENABLE_SOCKET_DEVIF)
	set(SOURCES ${SOURCES} code/sif_api.cpp)
endif()
	
# Check to see if Direct interface is excluded
list(FIND DEVIF_EXCLUDE direct index)
if(${index} EQUAL -1)
	add_definitions(-DENABLE_DIRECT_DEVIF)
	set(SOURCES ${SOURCES} code/direct_devif.cpp)
endif()
	
# Check to see if Dash interface is excluded
list(FIND DEVIF_EXCLUDE dash index)
if(${index} EQUAL -1)
	add_definitions(-DENABLE_DASH_DEVIF)
	if(NOT DEFINED DA_TOOLS_PATH)
	  if(CMAKE_SYSTEM_NAME MATCHES "Linux")
		set(DA_TOOLS_PATH "/opt/imgtec/DA-tools/")
	  elseif(CMAKE_SYSTEM_NAME MATCHES "Windows")
		set(DA_TOOLS_PATH "c:\\Program Files (x86)\\Imagination Technologoes\\DA-tools")
	  else()
		message(FATAL "Dash Interface supported on Linux and Windows only")
	  endif()
	endif()
	
	include_directories(${DA_TOOLS_PATH})
	set(SOURCES ${SOURCES} code/dashif.cpp)
	if (UNIX)
		set(DEVIF_LINK_LIB ${DEVIF_LINK_LIB} -ldl)
	endif()
endif()
	
# Check to see if Pdump1 interface is excluded
list(FIND DEVIF_EXCLUDE pdump1 index)
if(${index} EQUAL -1)
	add_definitions(-DENABLE_PDUMP1_DEVIF)
	set(SOURCES ${SOURCES} code/pdump1if.c)
endif()
	
# Check to see if Null interface is excluded
list(FIND DEVIF_EXCLUDE null index)
if(${index} EQUAL -1)
	add_definitions(-DENABLE_NULL_DEVIF)
	set(SOURCES ${SOURCES} code/null_dev.cpp)
endif()
	
# Check to see if Posted interface is excluded
list(FIND DEVIF_EXCLUDE posted index)
if(${index} EQUAL -1)
	find_package(RegDefsUtils REQUIRED) # posted.c needs reg_io2.h
	include_directories(${REGDEFSUTILS_INCLUDE_DIRS})
	add_definitions(${REGDEFSUTILS_DEFINITIONS})
	add_definitions(-DENABLE_POSTED_DEVIF)
	set(HEADERS ${HEADERS} code/posted_regio.h)
	set(SOURCES ${SOURCES} code/posted.c)
endif()

# ----------------------------------------------------------------------
# Build the library
# ----------------------------------------------------------------------
add_library(${DEVIF_LIBRARIES} STATIC ${SOURCES} ${HEADERS} ${EXTRA})
if(DEFINED DEVIF_LINK_LIB)
	target_link_libraries(${DEVIF_LIBRARIES} ${DEVIF_LINK_LIB})
endif()

