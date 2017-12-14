# Locate and configure the DeviceInterface library.
#
# Possible options:
#
#   DEVIF_PREFIX - the directory where DeviceInterface is located in the project (optional)
#
# Defines the following variables:
#
#   DEVIF_TRANSIF_FOUND - Found the DeviceInterface
#   DEVIF_TRANSIF_INCLUDE_DIRS - includes path
#   DEVIF_TRANSIF_LIBRARIES - The libraries to link
get_filename_component(DEVIF_TRANSIF_PREFIX "${CMAKE_CURRENT_LIST_FILE}" PATH)

find_package(DeviceInterface REQUIRED)

set(DEVIF_TRANSIF_FOUND TRUE)
set(DEVIF_TRANSIF_INCLUDE_DIRS ${DEVIF_TRANSIF_PREFIX} ${DEVICEIF_INCLUDE_DIRS})
set(DEVIF_TRANSIF_LIBRARIES DeviceInterfaceTransif ${DEVICEIF_LIBRARIES})

if(DEBUG_MODULES)
    message("DEVIF_TRANSIF_INCLUDE_DIRS         = ${DEVIF_TRANSIF_INCLUDE_DIRS}")
    message("DEVIF_TRANSIF_LIBRARIES            = ${DEVIF_TRANSIF_LIBRARIES}")
endif()

mark_as_advanced(DEVIF_TRANSIF_INCLUDE_DIRS DEVIF_TRANSIF_LIBRARIES)
