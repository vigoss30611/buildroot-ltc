cmake_minimum_required(VERSION 2.8)

project(DeviceInterface)

find_package(DeviceInterface REQUIRED) # trick to avoid duplication

if (NOT DEFINED DEVIF_USE_OSA)
    set(TAL_USE_OSA FALSE)
endif()
if (NOT DEFINED DEVIF_USE_MEOS)
    set(TAL_USE_MEOS FALSE)
endif()

if(DEVIF_USE_OSA)
    message(STATUS "    devif uses OSA")
elseif(DEVIF_USE_MEOS)
    message(STATUS "    devif uses MEOS")
else()
    message(STATUS "    devif is not thread-safe")
endif()

add_subdirectory(common)

if((NOT EXCLUDE_DEVICEIF_MASTER) AND (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/master))
  add_subdirectory(master)
endif()

if((NOT EXCLUDE_DEVICEIF_SLAVE) AND (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/slave))
  add_subdirectory(slave)
endif()
