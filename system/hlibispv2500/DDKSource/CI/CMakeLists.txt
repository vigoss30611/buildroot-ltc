cmake_minimum_required(VERSION 2.8)

project(CAPTURE_INTERFACE)

# windows C and CXX flags for shared library must be set up before reaching that project
   
## Set this to avoid zlib to be installed in the system
set (SKIP_INSTALL_ALL true)

#
# project options are included when using felix/UsePackage.cmake
#

message(STATUS "== Build Capture Interface ==")

# ----------------------------------------------------------------------
# Component Libraries in CMake Path
# ----------------------------------------------------------------------
include(felix/UsePackage.cmake)
include(imglib/UsePackage.cmake)
include(target/UsePackage.cmake)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} 
    #  ${CAPTURE_INTERFACE_SOURCE_DIR}/linkedlist
  ${CAPTURE_INTERFACE_SOURCE_DIR}/dyncmd
)

PerforceFound(P4_FOUND)
if (${P4_FOUND})
	PerforceChangelist(${CMAKE_CURRENT_SOURCE_DIR} CI_CHANGELIST)
	set(CI_CHANGELIST_STR "${CI_CHANGELIST}")
	PerforceUpdateDate(${CMAKE_CURRENT_SOURCE_DIR} CI_DATE_STR)
else()
	set(CI_CHANGELIST "-1")
	set(CI_CHANGELIST_STR "unknown-changelist")
	set(CI_DATE_STR "unkown-date")
endif()

add_subdirectory(felix)
#add_subdirectory(imglib/libraries/RegDefsUtils) # does not build anything

if (NOT CI_BUILD_KERNEL_MODULE)

  add_subdirectory(imglib/libraries/gzip_fileio)
  add_subdirectory(imglib/libraries/listman)
  add_subdirectory(imglib/libraries/list_utils)
  add_subdirectory(imglib/libraries/utils)

  add_subdirectory(imgmmu)

  add_subdirectory(osa)
  add_subdirectory(target)
  add_subdirectory(transif)
else()

  #
  # dyncmd, linkedlist and TAL are built from felix_lib using the *kernel.cmake scripts
  #
endif()
