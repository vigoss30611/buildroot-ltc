cmake_minimum_required(VERSION 2.8)

project(FELIX)

#
# these libraries have no scope higher than here therefore they are not in UsePackage.cmake
#

#
# define the kernel module location (not used when building FAKE interface)
#
if (NOT DEFINED ISP_KM_BINARY_DIR)
  set (ISP_KM_BINARY_DIR ${FELIX_BINARY_DIR}/felix_lib/km)
endif()


add_subdirectory(regdefs)
add_subdirectory(felix_lib)
# system is not a library

# do not compare CI against the SIM thus this is now useless
#add_subdirectory(sim_setup)

if(BUILD_TEST_APPS) # the driver test needs the DG to work
  add_subdirectory(driver_test)
endif()
