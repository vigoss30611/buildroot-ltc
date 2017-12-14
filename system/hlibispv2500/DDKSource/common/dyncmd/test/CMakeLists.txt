cmake_minimum_required(VERSION 2.8)

find_package (GTest REQUIRED)

add_definitions(${GTEST_DEFINITIONS})
include_directories(${GTEST_INCLUDE_DIRS} ${DYNCMD_PREFIX}/code)

set (TEST_SOURCES commandline_test.cpp)
set (TEST_HEADERS)

add_executable (${DYNCMD_NAME}_unit_tests ${TEST_SOURCES} ${TEST_HEADERS})
target_link_libraries (${DYNCMD_NAME}_unit_tests ${GTEST_LIBRARIES})
add_dependencies(${DYNCMD_NAME}_unit_tests ${GTEST_NAME})

add_test(DynamicCommandLine ${DYNCMD_NAME}_unit_tests --gtest_output=xml:xunits_dyncmd.xml)

add_executable (${DYNCMD_NAME}_doxytest dyncmd_example.c)
target_link_libraries (${DYNCMD_NAME}_doxytest ${DYNCMD_LIBRARIES})

file(COPY test_correct.txt test_longcomments.txt DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
