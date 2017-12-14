# Locate and configure the DeviceInterface library.
#
# Possible options:
#
#   DEVICEIF_PREFIX - the directory where DeviceInterface is located in the project (optional)
#
# Defines the following variables:
#
#   DEVICEIF_FOUND - Found the DeviceInterface
#   DEVICEIF_INCLUDE_DIRS - The directories needed on the include paths
#   DEVICEIF_LIBRARIES - The libraries to link
#   DEVICEIF_DEFINITIONS - definitions required for this library

if(NOT DEFINED DEVICEIF_PREFIX)
	# set DEVICEIF_PREFIX to directory where the current file file is located
	get_filename_component(DEVICEIF_PREFIX "${CMAKE_CURRENT_LIST_FILE}" PATH)
endif()

if (NOT DEFINED DEVIF_USE_OSA)
    set(DEVIF_USE_OSA FALSE)
endif()
if (NOT DEFINED DEVIF_USE_MEOS)
    set(DEVIF_USE_MEOS FALSE)
endif()

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  set(DEVICEIF_DEFINITIONS ${DEVICEIF_DEFINITIONS} "-DNOMINMAX")
endif()

if(DEVIF_USE_OSA)
    set(DEVICEIF_DEFINITIONS ${DEVICEIF_DEFINITIONS} -DOSA_MUTEX)
elseif(DEVIF_USE_MEOS)
    set(DEVICEIF_DEFINITIONS ${DEVICEIF_DEFINITIONS} -DMEOS_MUTEX)
endif()

set(DEVICEIF_FOUND TRUE)
set(DEVICEIF_INCLUDE_DIRS ${DEVICEIF_PREFIX}/common ${DEVICEIF_PREFIX}/master ${DEVICEIF_PREFIX}/slave)
set(DEVICEIF_MAS_LIBRARIES DEVICEIFMaster)
set(DEVICEIF_SLA_LIBRARIES DEVICEIFSlave)
# Not required externally
set(DEVICEIF_COM_LIBRARIES DEVICEIFCommon)

if(DEBUG_MODULES)
    message("DEVICEIF_INCLUDE_DIRS         = ${DEVICEIF_INCLUDE_DIRS}")
    message("DEVICEIF_DEFINITIONS          = ${DEVICEIF_DEFINITIONS}")
    message("DEVICEIF_MAS_LIBRARIES        = ${DEVICEIF_MAS_LIBRARIES}")
    message("DEVICEIF_SLA_LIBRARIES        = ${DEVICEIF_SLA_LIBRARIES}")
endif()

mark_as_advanced(DEVICEIF_INCLUDE_DIRS DEVICEIF_DEFINITIONS DEVICEIF_MAS_LIBRARIES DEVICEIF_SLA_LIBRARIES)
