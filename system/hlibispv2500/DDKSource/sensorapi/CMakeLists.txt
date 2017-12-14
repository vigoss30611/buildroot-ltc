cmake_minimum_required (VERSION 2.8)

project(SENSORAPI)

find_package(ImgIncludes REQUIRED)
find_package(FelixAPI REQUIRED) # FelixAPI used to acquire the gasket
find_package(FelixCommon REQUIRED) # pixel formats
find_package(SimImage REQUIRED) # for internal DG support

find_package(SensorApi REQUIRED) # find itself for config
find_package(GTest QUIET) # optional

if(WIN32 OR NOT CI_BUILD_KERNEL_MODULE)
  add_definitions(-Dfile_i2c=1) # needs to be implemented
endif()

if(NOT DEFINED SENSORAPI_UNIT_TESTS)
  set(SENSORAPI_UNIT_TESTS FALSE)
endif()

include_directories(
  ${IMGINCLUDES_INCLUDE_DIRS}
  ${FELIXAPI_INCLUDE_DIRS}
  ${FELIXCOMMON_INCLUDE_DIRS}
  ${SENSORAPI_INCLUDE_DIRS}
  ${SIMIMAGE_INCLUDE_DIRS}
  )

add_definitions(
  ${IMGINCLUDES_DEFINITIONS}
  ${FELIXAPI_DEFINITIONS}
  ${FELIXCOMMON_DEFINITIONS}
  ${SIMIMAGE_DEFINITIONS}
  )

set(SENSORAPI_H
  #include/sensorapi/sensor_phy.h
  #include/sensorapi/pciuser.h
  include/sensorapi/sensorapi.h
  )
#set(SENSORAPI_C   
#  source/sensorapi.c
#)

set(SENSORS_H include/sensors/iifdatagen.h)
set(SENSORS_C sensors/iifdatagen.c)

#if (SENSOR_AR330)
#  message(STATUS "add sensor AR330")
#  set(SENSORS_H ${SENSORS_H} include/sensors/ar330.h)
#  #set(SENSORS_C ${SENSORS_C} sensors/ar330.c)
#  add_definitions(-DSENSOR_AR330)
#endif()

if (SENSOR_AR330_DVP)
  message(STATUS "add sensor AR330 DVP")  
  add_definitions(-DSENSOR_AR330_DVP)
endif()

if (SENSOR_AR330_MIPI)
  message(STATUS "add sensor AR330 MIPI")  
  add_definitions(-DSENSOR_AR330_MIPI)
endif()

if (SENSOR_IMX322_DVP)
  message(STATUS "add sensor IMX322 DVP")  
  add_definitions(-DSENSOR_IMX322_DVP)
endif()

if (SENSOR_IMX179_MIPI)
  message(STATUS "add sensor IMX179 MIPI")  
  add_definitions(-DSENSOR_IMX179_MIPI)
endif()

if (SENSOR_99141_DVP)
  message(STATUS "add sensor 99141 DVP") 
  add_definitions(-DSENSOR_99141_DVP)
endif()

if (SENSOR_683Dual_MIPI)
  message(STATUS "add sensor 683Dual MIPI")  
  add_definitions(-DSENSOR_683Dual_MIPI)
endif()

#if (SENSOR_P401)
#  message(STATUS "add sensor P401")
#  set(SENSORS_H ${SENSORS_H} include/sensors/p401.h)
#  set(SENSORS_C ${SENSORS_C} sensors/p401.c)
#  add_definitions(-DSENSOR_P401)
#endif()

#if (SENSOR_OV4688)
#  message(STATUS "add sensor OV4688")
#  set(SENSORS_H ${SENSORS_H} include/sensors/ov4688.h)
#  set(SENSORS_C ${SENSORS_C} sensors/ov4688.c)
#  add_definitions(-DSENSOR_OV4688)
#endif()

if (SENSOR_OV2710)
  message(STATUS "add sensor OV2710")  
  add_definitions(-DSENSOR_OV2710)
endif()

if (SENSOR_OV4689)
  message(STATUS "add sensor OV4689")  
  add_definitions(-DSENSOR_OV4689)
endif()

#add_library (sensori2c ${SENSORI2C_C} ${SENSORI2C_H})
#target_link_libraries(sensori2c ${AARDVARK_LIBS} ${FELIXAPI_LIBRARIES})
#add_dependencies(sensori2c ${FELIXAPI_DEPENDENCIES})

if (CI_EXT_DATA_GENERATOR)
  find_package(FelixDG REQUIRED)

  add_definitions(
    -DSENSOR_DG
    ${FELIDG_DEFINITIONS}
    )
  include_directories(${FELIXDG_INCLUDE_DIRS})

  set(SENSORS_H ${SENSORS_H} include/sensors/dgsensor.h)
  set(SENSORS_C ${SENSORS_C} sensors/dgsensor.c)
endif()

#add_library(${SENSORAPI_NAME} ${SENSORAPI_H} ${SENSORS_H} ${SENSORAPI_C} ${SENSORS_C})
link_libraries(hlibcamsensor)
add_library(${SENSORAPI_NAME} ${SENSORAPI_H} ${SENSORS_H} ${SENSORS_C})

target_link_libraries(${SENSORAPI_NAME}  ${FELIXAPI_LIBRARIES} ${FELIXCOMMON_LIBRARIES} ${FELIXDG_LIBRARIES})
add_dependencies(${SENSORAPI_NAME} ${FELIXAPI_DEPENDENCIES})

source_group(Sensors_H FILES ${SENSORS_H})
source_group(Sensors_C FILES ${SENSORS_C})

if (BUILD_TEST_APPS)
  add_subdirectory(apps)
endif()

if (DEFINED GTEST_FOUND AND ${SENSORAPI_UNIT_TESTS})
  add_subdirectory(tests)
endif()
