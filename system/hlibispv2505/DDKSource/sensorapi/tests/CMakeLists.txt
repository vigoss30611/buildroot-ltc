cmake_minimum_required (VERSION 2.8)

message (STATUS "    unit tests")

find_package(ImgIncludes REQUIRED)
find_package(GTest REQUIRED)
find_package(SensorApi REQUIRED)

include_directories(
    ${IMGINCLUDES_INCLUDE_DIRS}
    ${GTEST_INCLUDE_DIRS}
    ${SENSORAPI_INCLUDE_DIRS}
)

add_definitions(
    ${IMGINCLUDES_DEFINITIONS}
    ${GTEST_DEFINITIONS}
    ${SENSORAPI_DEFINITIONS}
)

set (SOURCES_TEST
# headers
    sensorapi_test.hpp
# code
    sensorapi_test.cpp
    sensors_test.cpp
)

add_executable(${SENSORAPI_NAME}_unit_tests ${SOURCES_TEST})
target_link_libraries(${SENSORAPI_NAME}_unit_tests 
    ${GTEST_LIBRARIES}
    #
    ${SENSORAPI_LIBRARIES}
    ${SIMIMAGE_LIBRARIES}
    ${FELIXCOMMON_LIBRARIES}
)
add_dependencies(${SENSORAPI_NAME}_unit_tests ${GTEST_NAME})

if (DEFINED FELIXAPI_INSTALL)
	file(COPY ${FELIXAPI_INSTALL} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
    install(FILES ${FELIXAPI_INSTALL} DESTINATION ./CI/)
endif()

add_test(${SENSORAPI_NAME} ${SENSORAPI_NAME}_unit_tests --gtest_output=xml:xunits_sensorapi.xml)
