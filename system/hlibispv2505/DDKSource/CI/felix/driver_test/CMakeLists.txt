cmake_minimum_required(VERSION 2.8)

project(DriverTests)

message (STATUS "-- driver tests")
message (STATUS "    support ext-datagen ${CI_EXT_DATA_GENERATOR}")


find_package(ImgIncludes REQUIRED)
find_package(FelixAPI REQUIRED)
find_package(DynCMD REQUIRED)
find_package(SimImage REQUIRED) # use the save_file functions
find_package(RegDefs REQUIRED)
find_package(LinkedList REQUIRED)
find_package(FelixCommon REQUIRED)
find_package(Transif QUIET) # optional

set(DRV_TEST driver_test)

include_directories(
	${FELIXAPI_INCLUDE_DIRS}
	${DYNCMD_INCLUDE_DIRS}
	${IMGINCLUDES_INCLUDE_DIRS}
	${SIMIMAGE_INCLUDE_DIRS}
	${LINKEDLIST_INCLUDE_DIRS}
	${FELIXCOMMON_INCLUDE_DIRS}
	include/
)

add_definitions(
	${FELIXAPI_DEFINITIONS}
	${DYNCMD_DEFINITIONS}
	${IMGINCLUDES_DEFINITIONS}
	${SIMIMAGE_DEFINITIONS}
	${LINKEDLIST_DEFINITIONS}
	${FELIXCOMMON_DEFINITIONS}
)

set(ADDITIONAL_LIB)

if (CI_EXT_DATA_GENERATOR)
  find_package(FelixDG REQUIRED)

  include_directories(${FELIXDG_INCLUDE_DIRS})
  add_definitions(
    ${FELIXDG_DEFINITIONS}
    -DCI_EXT_DATAGEN
    )
  
  set(ADDITIONAL_LIB ${ADDITIONAL_LIB} ${FELIXDG_LIBRARIES})

else()
     message("Warning: driver_test may not work without CI_EXT_DATA_GENERATOR on all HW version (needs internal DG)")
endif()

if (TRANSIF_FOUND AND NOT CI_BUILD_KERNEL_MODULE)
	# transif is not found when building real drivers because it is to load the simulator as a dll when running fake interface - but it should always be found when building fake interface
	message(STATUS "    supports transif")
	include_directories(${TRANSIF_INCLUDE_DIRS})
	add_definitions(${TRANSIF_DEFINITIONS})
	set(ADDITIONAL_LIB ${TRANSIF_LIBRARIES})
endif()

set(SOURCES
	source/felixtest.c
)

set(HEADERS
	include/felix.h
)

#add_library(${DRIVERTEST_LIBRARIES} ${HEADERS} ${SOURCES})
#target_link_libraries(${DRIVERTEST_LIBRARIES}
#  ${FELIXAPI_LIBRARIES} 
#  ${SIMIMAGE_LIBRARIES}
#  ${ADDITIONAL_LIB} # if transif not used variable is empty
#)
#add_dependencies(${DRIVERTEST_LIBRARIES} ${FELIXAPI_DEPENDENCIES}) # for generated files

add_executable(${DRV_TEST} source/felix_main.c ${HEADERS} ${SOURCES})
target_link_libraries(${DRV_TEST} 
	${DYNCMD_LIBRARIES} #${DRIVERTEST_LIBRARIES}
	${FELIXAPI_LIBRARIES} 
	${SIMIMAGE_LIBRARIES}
	${LINKEDLIST_LIBRARIES}
	${FELIXCOMMON_LIBRARIES}
	${ADDITIONAL_LIB}
)
add_dependencies(${DRV_TEST} ${FELIXAPI_DEPENDENCIES}) # for generated files

file(COPY driver_args.txt DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
if (DEFINED FELIXAPI_INSTALL)
	file(COPY ${FELIXAPI_INSTALL} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
    #install(FILES ${FELIXAPI_INSTALL} DESTINATION ./CI/)
endif()

#
# copy config file
#
CopyVHCOutput(${DRV_TEST} Felix_IPC_Config.txt ${CMAKE_CURRENT_BINARY_DIR}/Felix_SIM_TAL_Config.txt)

#install(TARGETS ${DRV_TEST} DESTINATION ./CI/)
