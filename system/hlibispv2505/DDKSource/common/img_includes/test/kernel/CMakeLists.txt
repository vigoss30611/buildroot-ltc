cmake_minimum_required (VERSION 2.8)

project(NewImgIncludes)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${NewImgIncludes_SOURCE_DIR}/img_includes)

if (NOT WIN32)
  option(IMGINCLUDES_FORCE_KERNEL "use kernel includes" OFF)
  include(GenKernel.cmake)
endif()

message(STATUS "img_includes tests")
add_subdirectory(img_includes/tests)
