cmake_minimum_required(VERSION 2.8)

message(STATUS "TAL unit tests")

find_package(TAL REQUIRED)
set(IMGINCLUDES_EXIT_ON_ASSERT FALSE)
find_package(ImgIncludes REQUIRED) # already included but we need to insure no_assert is on
find_package(GTest REQUIRED)

if(TAL_USE_MEOS)
  find_package(ImgSystem REQUIRED)
endif()

include_directories(
  ${IMGSYSTEM_INCLUDE_DIRS}
  ${GTEST_INCLUDE_DIRS}
)
add_definitions(
	${GTEST_DEFINITIONS}
	${IMGINCLUDES_DEFINITIONS}
)

set (TEST_SOURCES
	tal_test.h
	talbase.cpp
	highlevel_test.cpp
	main_test.cpp
)

if(${TAL_TYPE} MATCHES "normal")

set (TEST_SOURCES ${TEST_SOURCES}
	pdump_test.cpp
)

endif()

include_directories(
  ${GTEST_INCLUDE_DIRS}
  ${IMGINCLUDES_INCLUDE_DIRS}
  ${TAL_INCLUDE_DIRS}
)
add_definitions(
  ${GTEST_DEFINITIONS}
  ${IMGINCLUDES_DEFINITIONS}
  ${TAL_DEFINITIONS}
)

add_executable(${TAL_LIBRARIES}_unit_tests ${TEST_SOURCES})
target_link_libraries(${TAL_LIBRARIES}_unit_tests ${TAL_LIBRARIES} ${GTEST_LIBRARIES})
add_dependencies(${TAL_LIBRARIES}_unit_tests ${GTEST_NAME})

add_test(TAL_Test ${TAL_LIBRARIES}_unit_tests --gtest_output=xml:xunits_tal.xml)
