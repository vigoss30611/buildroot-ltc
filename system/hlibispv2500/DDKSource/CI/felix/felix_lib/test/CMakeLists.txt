cmake_minimum_required (VERSION 2.8)

project (FELIX_UTEST CXX)

message (STATUS "    driver unit tests")

#set (IMGINCLUDES_EXIT_ON_ASSERT FALSE)
remove_definitions(${IMGINCLUDES_DEFINITIONS})
set (IMGINCLUDES_MALLOC_TEST TRUE)
find_package (ImgIncludes REQUIRED)
# special configuration for the unit tests

find_package(GTest REQUIRED)
find_package(FelixAPI REQUIRED)
find_package(FelixCommon REQUIRED)
find_package(TAL REQUIRED)
find_package(RegDefsUtils REQUIRED)
find_package(LinkedList REQUIRED)
find_package(ImgLib_GZIPIO REQUIRED) # used in some tests
find_package(SimImage REQUIRED) # needed for the setup functions that does not link with it while it needs
find_package(IMGMMU REQUIRED)

include_directories(
  ${FELIXAPI_INCLUDE_DIRS}
  ${TAL_INCLUDE_DIRS}
  ${REGDEFSUTILS_INCLUDE_DIRS}
  ${LINKEDLIST_INCLUDE_DIRS}
  ${IMGLIB_GZIPIO_INCLUDE_DIRS}
  ${IMGMMU_INCLUDE_DIRS}
  ${FELIXCOMMON_INCLUDE_DIRS}
  ${GTEST_INCLUDE_DIRS}
  include
  ${IMGINCLUDES_INCLUDE_DIRS}
  ${FELIXSIM_INCLUDE_DIRS}
  ${SIMIMAGE_INCLUDE_DIRS}
  )
add_definitions(
  ${FELIXAPI_DEFINITIONS}
  ${TAL_DEFINITIONS}
  ${REGDEFSUTILS_DEFINITIONS}
  ${LINKEDLIST_DEFINITIONS}
  ${IMGLIB_GZIPIO_DEFINITIONS}
  ${IMGMMU_DEFINITIONS}
  #-DCI_MEMSET_ALIGNMENT=0xAA
  -DCI_LOG_LEVEL=0 # make all kernel module prints quite
  ${IMGINCLUDES_DEFINITIONS}
  ${FELIXCOMMON_DEFINITIONS}
  ${SIMIMAGE_DEFITIONS}

  ${GTEST_DEFINITIONS}
  ${FELIXSIM_DEFINITIONS}
  -DFELIX_UNIT_TESTS
  )
# FELIX_UNIT_TESTS is used to init the register with default values using extern function

set(TEST_LINKS 
  ${TAL_LIBRARIES}
  ${LINKEDLIST_LIBRARIES}
  ${IMGMMU_LIBRARIES}
  ${SIMIMAGE_LIBRARIES}
  ${FELIXCOMMON_LIBRARIES}
)

if(WIN32)
  find_package (PThreadWin32 REQUIRED)

  # so it can be run from local folder
  file(COPY ${PTHREADWIN32_INSTALL} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
  set(TEST_LINKS ${TEST_LINKS} ${PTHREADWIN32_LIBRARIES})
else()
  set (TEST_LINKS ${TEST_LINKS} -lpthread)
endif()

add_definitions(-DCI_HW_REF_CLK=40)

if (CI_CODE_COVERAGE)
  message (STATUS "enable code coverage for ${FELIXAPI_NAME}_unit_tests")
  add_definitions(-fprofile-arcs -ftest-coverage -DNDEBUG) # NDEBUG remove asserts
  set (TEST_LINKS ${TEST_LINKS} -lgcov)
  if(NOT CMAKE_COMPILER_IS_GNUCC)
    message ("Warning: code coverage is enabled for ${FELIXAPI_NAME}_unit_tests but your compiler is not GCC.")
  endif()
endif()

include_directories (
  ${FelixLib_SOURCE_DIR}/data_generator
  )

#
# the sources of the library are included in the unit test
# because they use an extern variable that allows the unit
# test to make a malloc call to fail (return NULL)
#
set (SOURCES
  code/ci_internal_test.cpp
  code/ci_capture_test.cpp
  code/ci_config_test.cpp
  code/ci_pipeline_test.cpp
  code/ci_shot_test.cpp
  code/ci_api_test.cpp
  code/ci_mem_test.cpp
  code/ci_hwstruct_test.cpp
  code/sys_device_test.cpp
  #code/felix.cpp
  code/main_test.cpp
  code/ci_parallel_test.cpp
  code/mc_test.cpp
  code/ci_mmu_test.cpp
  code/ci_intdg_test.cpp
  )

set(FELIXTEST_LIB_SRC
  # should add all the sources from felix here as well
  # because they are recompiled with -DFELIX_UNIT_TESTS to have slighly different behaviours

  # ci user
  ${FelixLib_SOURCE_DIR}/user/source/ci_api.c
  ${FelixLib_SOURCE_DIR}/user/source/ci_pipeline.c
  ${FelixLib_SOURCE_DIR}/user/source/ci_intdg.c
  ${FelixLib_SOURCE_DIR}/user/source/ci_gasket.c
  ${FelixLib_SOURCE_DIR}/user/source/ci_modulesdefaults.c
  ${FelixLib_SOURCE_DIR}/user/source/sys_userio_fake.c
  # mc
  ${FelixLib_SOURCE_DIR}/user/source/mc_config.c
  ${FelixLib_SOURCE_DIR}/user/source/mc_convert.c
  ${FelixLib_SOURCE_DIR}/user/source/mc_pipeline.c
  ${FelixLib_SOURCE_DIR}/user/source/mc_extract.c

  # ci kernel
  ${FelixLib_SOURCE_DIR}/kernel/platform_src/sys_mem.c
  ${FelixLib_SOURCE_DIR}/kernel/kernel_src/ci_internal.c
  ${FelixLib_SOURCE_DIR}/kernel/kernel_src/ci_config.c
  ${FelixLib_SOURCE_DIR}/kernel/kernel_src/ci_capture.c
  ${FelixLib_SOURCE_DIR}/kernel/kernel_src/ci_shot.c
  ${FelixLib_SOURCE_DIR}/kernel/kernel_src/ci_hwstruct.c
  ${FelixLib_SOURCE_DIR}/kernel/kernel_src/ci_interrupt.c
  ${FelixLib_SOURCE_DIR}/kernel/kernel_src/ci_connection.c
  ${FelixLib_SOURCE_DIR}/kernel/kernel_src/ci_ioctl_km.c
  ${FelixLib_SOURCE_DIR}/kernel/kernel_src/ci_mmu.c
  ${FelixLib_SOURCE_DIR}/kernel/kernel_src/ci_intdg_km.c
  ${FelixLib_SOURCE_DIR}/kernel/kernel_src/ci_debug.c

  # platform
  ${FelixLib_SOURCE_DIR}/kernel/platform_src/sys_parallel_posix.c
  ${FelixLib_SOURCE_DIR}/kernel/platform_src/sys_device_posix.c
  ${FelixLib_SOURCE_DIR}/kernel/platform_src/sys_mem_tal.c
  )
set(FELIXTEST_SRC
  code/felix.cpp

  include/felix.h
  include/unit_tests.h

  ${FELIXTEST_LIB_SRC}
  )
source_group(CI FILES ${FELIXTEST_LIB_SRC})


if (CI_DEBUG_FUNCTIONS)
  set (SOURCES ${SOURCES}
    code/debug_test.cpp
    )
endif()

set (HEADERS
  #include/unit_tests.h
  #include/felix.h
  )

if (CI_EXT_DATA_GENERATOR)
  find_package(FelixDG REQUIRED)

  include_directories(${FELIXDG_INCLUDE_DIRS})
  add_definitions(${FELIXDG_DEFINITIONS} -DTEST_USES_DATAGEN)
  set (DG_USER_SRC ${FelixLib_SOURCE_DIR}/data_generator/user)
  set (DG_KERNEL_SRC ${FelixLib_SOURCE_DIR}/data_generator/kernel)

  set (DG_LIB_SRC
    ${DG_USER_SRC}/dg_api.c
    ${DG_USER_SRC}/dg_converter.c
    ${DG_KERNEL_SRC}/dg_camera.c
    ${DG_KERNEL_SRC}/dg_connection.c
    #${DG_KERNEL_SRC}/dg_debug.c
    ${DG_KERNEL_SRC}/dg_internal.c
    ${DG_KERNEL_SRC}/dg_io_km.c
    )

  # add sources for the DG
  set (SOURCES ${SOURCES}
    code/dg_datagen_test.cpp
    code/dg_converter_test.cpp
    code/dg_pgm_test.cpp

    # add the DG sources as well
    ${DG_LIB_SRC}
    )

  source_group(DatagenLib FILES ${DG_LIB_SRC})

  # set (HEADERS ${HEADERS} ${DG_HEADERS})
  set (TEST_LINKS ${TEST_LINKS} ${FELIXDG_LIBRARIES})
endif()

# foreach(lib ${FELIXLIB_LIBRARIES})
# if (NOT ${lib} MATCHES ${FELIXAPI_NAME})
# set (LINK_WITH ${LINK_WITH} ${lib})
# endif()
# endforeach()

if (DEBUG_MODULES)
  message (" unit tests linking with ${LINK_WITH}")
endif()

# also used by ISP unit tests
add_library(${CITEST_NAME} ${FELIXTEST_SRC})
target_link_libraries(${CITEST_NAME} ${TEST_LINKS})
add_dependencies(${CITEST_NAME} ${FELIXAPI_DEPENDENCIES} ${GTEST_NAME})

add_executable (${FELIXAPI_NAME}_unit_tests ${SOURCES} ${HEADERS})
target_link_libraries (${FELIXAPI_NAME}_unit_tests ${GTEST_LIBRARIES} ${CITEST_NAME})
add_dependencies (${FELIXAPI_NAME}_unit_tests ${FELIXAPI_DEPENDENCIES} ${GTEST_NAME})

#file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${FELIX_PDUMP_FOLDER})

add_test(CaptureInterface ${FELIXAPI_NAME}_unit_tests --gtest_output=xml:xunits_captureinterface.xml)
