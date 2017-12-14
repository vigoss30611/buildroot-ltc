cmake_minimum_required(VERSION 2.8)

project(DeviceInterface)

find_package(DeviceInterface REQUIRED) # trick to avoid duplication

add_subdirectory(common)

if((NOT EXCLUDE_DEVICEIF_MASTER) AND (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/master))
  add_subdirectory(master)
endif()

if((NOT EXCLUDE_DEVICEIF_SLAVE) AND (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/slave))
  add_subdirectory(slave)
endif()
