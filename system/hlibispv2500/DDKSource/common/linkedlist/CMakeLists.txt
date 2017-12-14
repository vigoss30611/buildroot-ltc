cmake_minimum_required(VERSION 2.8)

project (LinkedList)

message (STATUS "-- Template Linked List")

find_package(ImgIncludes REQUIRED)
find_package(LinkedList REQUIRED)
find_package(GTest QUIET) # optional

if(NOT DEFINED LINKEDLIST_TESTS)
	set (LINKEDLIST_TESTS FALSE)
endif()

include_directories(${IMGINCLUDES_INCLUDE_DIRS} ${LINKEDLIST_INCLUDE_DIRS})
add_definitions(${IMGINCLUDES_DEFINITIONS} ${LINKEDLIST_DEFINITIONS})

set (SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/code/linkedlist.c)
set (HEADERS include/linkedlist.h)

add_library(${LINKEDLIST_NAME} STATIC ${SOURCES} ${HEADERS})

if (GTEST_FOUND AND LINKEDLIST_TESTS) # build unit tests

	add_subdirectory(test)
	
endif()
