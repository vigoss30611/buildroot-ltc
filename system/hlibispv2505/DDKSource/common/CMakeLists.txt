cmake_minimum_required(VERSION 2.8)

set (DYNCMD_TESTS ${BUILD_UNIT_TESTS})
set (LINKEDLIST_TESTS ${BUILD_UNIT_TESTS})
set (FELIXCOMMON_TESTS ${BUILD_UNIT_TESTS})
set (SIMIMAGE_TESTS ${BUILD_UNIT_TESTS})

message(STATUS "== Common libraries ==")

if(BUILD_UNIT_TESTS)
  include(external/gtest/gtest.cmake)
endif()

#${BUILD_CAPTUREINTERFACE} AND
if (NOT ${CI_BUILD_KERNEL_MODULE}) # need the TAL and ZLib when using TAL normal
  include(external/mxml/mxml.cmake)
  include(external/zlib/zlib.cmake)
endif()

if (BUILD_GUI)
  include(external/qwt/qwt.cmake)
endif()

add_subdirectory(felixcommon)
if ( BUILD_TEST_APPS OR BUILD_GUI )
  add_subdirectory(paramsocket2)
endif()
add_subdirectory(dyncmd)
add_subdirectory(sim_image)
add_subdirectory(linkedlist)

if (BUILD_TEST_APPS)
    include (HDRLibs/HDRLibs.cmake)
endif()

# img include is not a library - but it has unit tests
if(BUILD_UNIT_TESTS)
  add_subdirectory(img_includes/test)
endif()

add_subdirectory(dmabuf)
