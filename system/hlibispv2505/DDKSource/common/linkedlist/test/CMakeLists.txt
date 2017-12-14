cmake_minimum_required(VERSION 2.8)

message (STATUS "    unit tests")

remove_definitions(${IMGINCLUDES_DEFINITIONS})

set (IMGINCLUDES_MALLOC_TEST TRUE)
find_package(ImgIncludes REQUIRED) # reconfigure ImgIncludes to have the malloc test

add_definitions(
	${IMGINCLUDES_DEFINITIONS}
	${GTEST_DEFINITIONS}
)
include_directories(
	${LinkedList_SOURCE_DIR}/code # to be able to include the source file into the test
	${GTEST_INCLUDE_DIRS}
	${LINKEDLIST_INCLUDE_DIRS}
)

set (SOURCES linkedlist_test.cpp)

add_executable(${LINKEDLIST_NAME}_unit_tests ${SOURCES})
target_link_libraries(${LINKEDLIST_NAME}_unit_tests ${GTEST_LIBRARIES})
add_dependencies(${LINKEDLIST_NAME}_unit_tests ${GTEST_NAME})

add_test(LinkedList ${LINKEDLIST_NAME}_unit_tests --gtest_output=xml:xunits_linkedlist.xml)
