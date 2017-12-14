cmake_minimum_required(VERSION 2.8)



# should contain already includes and defs from sensor_api
# assumes that BUILD_TEST_APPS is defined

# need to add a build of i2c-test once it has been modified to be flexible on device and address to read/write

if(NOT WIN32)
  find_package(DynCMD REQUIRED)

  include_directories(${DYNCMD_INCLUDE_DIRS})
  add_definitions(${DYNCMD_DEFINITIONS})

  message(STATUS "Build sensor_test")
  #add_executable(sensor_test sensor_test.c)
  #target_link_libraries(sensor_test ${SENSORAPI_LIBRARIES} ${DYNCMD_LIBRARIES})

  #install(TARGETS sensor_test DESTINATION ./utils/)
endif()
