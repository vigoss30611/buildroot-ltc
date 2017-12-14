cmake_minimum_required(VERSION 2.8)

find_package (GTest REQUIRED)

add_definitions(${GTEST_DEFINITIONS})
include_directories(${GTEST_INCLUDE_DIRS})

set (TEST_SOURCES
	main_test.cpp
	simimagein_test.cpp
)
set (TEST_HEADERS)

add_executable (${SIMIMAGE_NAME}_unit_tests ${TEST_SOURCES} ${TEST_HEADERS})
target_link_libraries (${SIMIMAGE_NAME}_unit_tests ${GTEST_LIBRARIES} ${SIMIMAGE_NAME})
add_dependencies(${SIMIMAGE_NAME}_unit_tests ${GTEST_NAME})

add_test(SimImage ${SIMIMAGE_NAME}_unit_tests --gtest_output=xml:xunits_simimage.xml)

file(COPY testdata DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
