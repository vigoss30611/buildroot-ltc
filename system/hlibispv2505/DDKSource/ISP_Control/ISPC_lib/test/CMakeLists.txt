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
#    ${ISPC2TEST_INCLUDE_DIRS}
    include/
)

add_definitions(
    ${GTEST_DEFINITIONS}
    ${CITEST_DEFINITIONS}
#    ${ISPC2TEST_DEFINITIONS}
    
    -DFELIX_UNIT_TESTS
)

if (CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_C_FLAGS_DEBUG      "${CMAKE_C_FLAGS_DEBUG} -g -O0")
    set(CMAKE_CXX_FLAGS_DEBUG    "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.7)
        add_definitions(-std=c++0x)
    else()
        add_definitions(-std=c++11)
    endif()
endif()

set (TEST_SRC
    src/main_test.cpp
    include/ispc_utest.h
    src/ispc_utest.cpp
    
    # generic class tests
    src/sensor_test.cpp
    src/TempCorr_test.cpp
    src/helpers_test.cpp
    src/parameters_test.cpp
    src/ParameterFileParser_test.cpp
    src/Module_test.cpp
    
    # module tests
    src/ModuleLSH_test.cpp
    
    # control tests
    src/ControlAE_test.cpp
    src/ControlAWB_test.cpp
    src/ControlLSH_test.cpp
    
    # re-add the whole library source code as it is recompiled with -DFELIX_UNIT_TESTS
    ${ISPC_CPP}
)

source_group(ISPC FILES ${ISPC_CPP})

add_executable(${ISPC2_LIBRARIES}_unit_tests ${TEST_SRC})
target_link_libraries(${ISPC2_LIBRARIES}_unit_tests 
    ${CITEST_LIBRARIES}
    ${GTEST_LIBRARIES}
#    ${ISPC2TEST_LIBRARIES}
    #
    ${SENSORAPI_LIBRARIES}
    ${SIMIMAGE_LIBRARIES}
    ${FELIXCOMMON_LIBRARIES}
)

add_dependencies (${ISPC2_LIBRARIES}_unit_tests ${FELIXAPI_DEPENDENCIES} ${GTEST_NAME})

if (DEFINED FELIXAPI_INSTALL)
    file(COPY ${FELIXAPI_INSTALL} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endif()
file(COPY ${ISPC2_TEST_DATA} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_test(ISPControl ${ISPC2_LIBRARIES}_unit_tests --gtest_output=xml:xunits_ispc.xml)
