cmake_minimum_required (VERSION 2.8)

message (STATUS "    ISPC unit tests")

#set (IMGINCLUDES_EXIT_ON_ASSERT FALSE)
remove_definitions(${IMGINCLUDES_DEFINITIONS})
set (IMGINCLUDES_MALLOC_TEST TRUE)
find_package(ImgIncludes REQUIRED)
# special configuration for the unit tests

find_package(GTest REQUIRED)
find_package(FelixAPI REQUIRED)
#find_package(FelixCommon REQUIRED) # should be done by higher level

if(NOT ${CITEST_FOUND})
    message(FATAL_ERROR "need CITEST_FOUND to be true to compile ISPC unit tests")
endif()

include_directories(
    ${CITEST_INCLUDE_DIRS}
    ${GTEST_INCLUDE_DIRS}
)

add_definitions(
    ${GTEST_DEFINITIONS}
    ${CITEST_DEFINITIONS}
    
    -DFELIX_UNIT_TESTS
)

set (TEST_SRC
    src/main_test.cpp
    
    # generic class tests
    src/sensor_test.cpp
    
    # module tests
    
    # control tests
    
    # re-add the whole library source code as it is recompiled with -DFELIX_UNIT_TESTS
    ${ISPC_CPP}
)

source_group(ISPC FILES ${ISPC_CPP})

add_executable(${ISPC2_LIBRARIES}_unit_tests ${TEST_SRC})
target_link_libraries(${ISPC2_LIBRARIES}_unit_tests 
    ${CITEST_LIBRARIES}
    ${GTEST_LIBRARIES}
    #
    ${SENSORAPI_LIBRARIES}
    ${SIMIMAGE_LIBRARIES}
    ${FELIXCOMMON_LIBRARIES}
)

add_dependencies (${ISPC2_LIBRARIES}_unit_tests ${FELIXAPI_DEPENDENCIES} ${GTEST_NAME})

if (DEFINED FELIXAPI_INSTALL)
	file(COPY ${FELIXAPI_INSTALL} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
    install(FILES ${FELIXAPI_INSTALL} DESTINATION ./CI/)
endif()

add_test(ISPControl ${ISPC2_LIBRARIES}_unit_tests --gtest_output=xml:xunits_ispc.xml)
