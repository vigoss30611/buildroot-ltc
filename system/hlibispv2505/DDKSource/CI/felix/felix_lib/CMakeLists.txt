cmake_minimum_required(VERSION 2.8)

project(FelixLib) # used in FindDG and in driver_test

message (STATUS "-- Capture Interface driver")

find_package(GTest QUIET) # optional for unit tests

#
# the internal part is only available to the FelixAPI
#
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${felix_lib_SOURCE_DIR}/kernel/)

#
# Data gen
#
if (CI_EXT_DATA_GENERATOR)
  message (STATUS "    using Data Generator")
  find_package(FelixDG REQUIRED)

  # defined in advance
  add_definitions(-DFELIX_HAS_DG)
endif()

# user has to be first to define the location of the FelixUser project (to find includes needed for the kernel module)
add_subdirectory(user)
add_subdirectory(kernel)
if (BUILD_TEST_APPS)
	add_subdirectory(utils) # not really a test app but used with test apps
endif()

if (CI_EXT_DATA_GENERATOR)
  add_subdirectory(data_generator)
endif()

# CAPTUREINTERFACE_TESTS disabled in project.cmake when building KO
if(GTEST_FOUND AND CAPTUREINTERFACE_TESTS)
    option(CI_TEST_USES_TRANSIF "Use transif for communication between CI driver test and simulator.")
    if(${CI_TEST_USES_TRANSIF} MATCHES "ON")
        add_definitions(-DCI_TEST_USES_TRANSIF)
    endif()
    file(COPY valgrind_suppr.txt DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/test/)
    add_subdirectory(test)
endif()
