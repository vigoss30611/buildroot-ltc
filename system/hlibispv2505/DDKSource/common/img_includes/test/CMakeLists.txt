cmake_minimum_required (VERSION 2.8)

project(IMGIncludes_test)
message(STATUS "-- IMG Includes unit tests")

#
# if this test is on top level
#
if (${CMAKE_CURRENT_SOURCE_DIR} STREQUAL ${CMAKE_SOURCE_DIR})
	set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${IMGIncludes_test_SOURCE_DIR}/.. ${IMGIncludes_test_SOURCE_DIR}/kernel/)
	include(kernel/GenKernel.cmake)
endif()

find_package(ImgIncludes REQUIRED)
find_package(GTest QUIET)

if (NOT DEFINED IMGINCLUDES_TEST_USES_GTEST)
	set (IMGINCLUDES_TEST_USES_GTEST TRUE) # if gtest is found use it
endif()

include_directories(${IMGINCLUDES_INCLUDE_DIRS})
add_definitions(${IMGINCLUDES_DEFINITIONS})

if(NOT IMGINCLUDES_FORCE_KERNEL)
  message(STATUS "    build user-mode unit_test")

  add_executable(IMGIncludes_unittest unit_test.cpp)

  if (GTEST_FOUND AND IMGINCLUDES_TEST_USES_GTEST)
	include_directories(${GTEST_INCLUDE_DIRS})
	add_definitions(${GTEST_DEFINITIONS})
    add_definitions(-DUSE_GTEST)
    target_link_libraries(IMGIncludes_unittest ${GTEST_LIBRARIES})
	add_dependencies(IMGIncludes_unittest ${GTEST_NAME})
  endif()

  # if not gtest the test args are useless
  add_test(IMGIncludes IMGIncludes_unittest --gtest_output=xml:xunits_IMGincludes.xml)
  
  add_executable(IMGIncludes_compiletest compile_test.c)

  add_executable(IMGIncludes_asserttest test_assert.c)
else()
	message(STATUS "    build kernel-mode unit_test")

   if (NOT DEFINED LINUX_KERNEL_BUILD_DIR)
    execute_process(COMMAND uname -r OUTPUT_VARIABLE KERNEL_VER OUTPUT_STRIP_TRAILING_WHITESPACE)
    set (LINUX_KERNEL_BUILD_DIR "/lib/modules/${KERNEL_VER}/build")
    message ("LINUX_KERNEL_BUILD_DIR not define: using kernel ${KERNEL_VER}")
  endif()

  add_definitions(-DIMG_BUILD_KO)

  get_directory_property(KO_INCLUDES INCLUDE_DIRECTORIES)
  GenSpaceList("${KO_INCLUDES}" "-I" KO_INCLUDES)
  get_directory_property(KO_CFLAGS DEFINITIONS) # spaces and -D

  set (KO_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/km_compile)
  message("build compile test as ko in ${KO_BINARY_DIR}")

  set(KO_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/compile_test.c)
  
  GenKernel_Makefile(imgincludes_build "${KO_CFLAGS}" "${KO_SOURCES}" "${KO_INCLUDES}" ${KO_BINARY_DIR})
  GenKernel_addTarget(imgincludes_build ${KO_BINARY_DIR})

  set (KO_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/km_test)
  message("build unit test as ko in ${KO_BINARY_DIR}")

  file(COPY unit_test.cpp DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
  file(RENAME ${CMAKE_CURRENT_BINARY_DIR}/unit_test.cpp ${CMAKE_CURRENT_BINARY_DIR}/unit_test.c)
  set(KO_SOURCES ${CMAKE_CURRENT_BINARY_DIR}/unit_test.c)

  GenKernel_Makefile(imgincludes_test "${KO_CFLAGS}" "${KO_SOURCES}" "${KO_INCLUDES}" ${KO_BINARY_DIR})
  GenKernel_addTarget(imgincludes_test ${KO_BINARY_DIR})

endif()
