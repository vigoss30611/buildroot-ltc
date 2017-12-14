cmake_minimum_required (VERSION 2.8)

message (STATUS "    unit tests")

find_package(ImgIncludes REQUIRED)
find_package(GTest REQUIRED)
find_package(FelixCommon REQUIRED)

include_directories(
  ${IMGINCLUDES_INCLUDE_DIRS}
  ${GTEST_INCLUDE_DIRS}
  ${FELIXCOMMON_INCLUDE_DIRS}
)
add_definitions(
  ${IMGINCLUDES_DEFINITIONS}
  ${GTEST_DEFINITIONS}
  ${FELIXCOMMON_DEFINITIONS}
)

set (SOURCES_TEST
    ci_alloc_info_test.cpp
	tiling_test.cpp
	mosaic_test.cpp
	lshgrid_test.cpp
    main_test.cpp
)

add_executable (${FELIXCOMMON_NAME}_test ${SOURCES_TEST})
target_link_libraries (${FELIXCOMMON_NAME}_test ${GTEST_LIBRARIES} ${FELIXCOMMON_LIBRARIES})
add_dependencies (${FELIXCOMMON_NAME}_test ${GTEST_NAME})

add_test(${FELIXCOMMON_NAME} ${FELIXCOMMON_NAME}_test --gtest_output=xml:xunits_felixcommon.xml)
