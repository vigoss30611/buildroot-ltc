cmake_minimum_required(VERSION 2.8)

# Callback Manager Test project base 
project(IMGLIB_list_test)

# if this test is on top level
if (${CMAKE_CURRENT_SOURCE_DIR} MATCHES ${CMAKE_SOURCE_DIR})
	set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${IMGLIB_list_test_SOURCE_DIR}/.. ${IMGINCLUDES_PREFIX})
	add_subdirectory(.. list_utils)
endif()

find_package(ImgIncludes REQUIRED)
find_package(ImgLib_ListUtils REQUIRED)
find_package(GTest QUIET)



if (NOT DEFINED IMGLIB_LISTUTILS_TEST_USES_GTEST)
	set (IMGLIB_LISTUTILS_TEST_USES_GTEST FALSE)
endif()

include_directories(${IMGINCLUDES_INCLUDE_DIRS})
include_directories(${IMGLIB_LISTUTILS_INCLUDE_DIRS})
add_definitions(${IMGINCLUDES_DEFINITIONS})
add_definitions(${IMGLIB_LISTUTILS_DEFINITIONS})



#Just testing build, further unit testing required
add_executable(imglib_listutils_compiletest compile_test.c)

target_link_libraries(imglib_listutils_compiletest ${IMGLIB_LISTUTILS_LIBRARIES})
