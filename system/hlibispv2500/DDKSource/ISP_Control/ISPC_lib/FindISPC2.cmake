cmake_minimum_required (VERSION 2.8)

get_filename_component (ISPC2_PREFIX "${CMAKE_CURRENT_LIST_FILE}" PATH)

#find_package(ImgIncludes REQUIRED) # needed in the headers - but should be included by application
find_package(FelixAPI REQUIRED) # needed in the headers
find_package(SensorApi REQUIRED) # needed in the headers

set (ISPC2_FOUND TRUE)
set (ISPC2_INCLUDE_DIRS 
  ${ISPC2_PREFIX}/include
  ${ISPC2_PREFIX}/include/Modules
  ${ISPC2_PREFIX}/include/Auxiliar
  ${ISPC2_PREFIX}/include/Controls
  ${FELIXAPI_INCLUDE_DIRS}
  ${SENSORAPI_INCLUDE_DIRS}
)
set (ISPC2_DEFINITIONS 
  ${FELIXAPI_DEFINITIONS}
  ${SENSORAPI_DEFINITIONS}
)
set (ISPC2_LIBRARIES ispv2500 ) #modify ISPC to ispv2500 

#
# small library that can be used when doing fake drivers to fake insmod
# when building fake driver but also helps for HDF merging
#
set(ISPC2TEST_FOUND TRUE)
set(ISPC2TEST_DEFINITIONS)
set(ISPC2TEST_INCLUDE_DIRS ${ISPC2_PREFIX}/ispc2test/include)
set(ISPC2TEST_LIBRARIES ISPC2Test)
