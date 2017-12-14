cmake_minimum_required(VERSION 2.8)

set(IMGINCLUDES_MALLOC_TEST TRUE)
set(IMGINCLUDES_EXIT_ON_ASSERT FALSE)
find_package(ImgIncludes REQUIRED) # already included but we need to insure malloc test is on and no_assert is on

find_package(GTest REQUIRED)

include_directories(
	${GTEST_INCLUDE_DIRS}
)
add_definitions(
	${GTEST_DEFINITIONS}
	${IMGINCLUDES_DEFINITIONS}
)

if (IMGMMU_TALHEAP)
	add_definitions(-DUSES_TALHEAP)
endif()

if(NOT WIN32 AND TAL_USE_MEOS)
  find_package(MeOSAL REQUIRED)
  include_directories(${MEOSAL_INCLUDE_DIRS})
  add_definitions(
    ${MEOSAL_DEFINITIONS}
    -D__TAL_USE_MEOS__
	#-DUSE_TAL_CFG # can be added to use TAL config file instead of config structure for the tests
    )  
endif()

set(SOURCES
	tal_config.h
	imgmmu_test.cpp
	main_test.cpp
)

if (IMGMMU_TALHEAP)
	set(SOURCES ${SOURCES}
		talheap_test.cpp
		talalloc_test.cpp
	)
endif()

add_executable(${IMGMMU_NAME}_unit_tests ${SOURCES})
target_link_libraries(${IMGMMU_NAME}_unit_tests ${GTEST_LIBRARIES})
add_dependencies(${IMGMMU_NAME}_unit_tests ${GTEST_NAME})

if (DEFINED LINKWITH) # defined in the above directory
	target_link_libraries(${IMGMMU_NAME}_unit_tests ${LINKWITH})
endif()

add_test(MMULib ${IMGMMU_NAME}_unit_tests --gtest_output=xml:xunits_mmulib.xml)

# copy files needed for the tests
# only used if code is compiled with USE_TAL_CFG
file(COPY MMU_TAL_config.txt DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
