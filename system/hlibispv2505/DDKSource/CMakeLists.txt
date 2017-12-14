cmake_minimum_required(VERSION 2.8)

message("INFOTM_LANCHOU_PROJECT: ${INFOTM_LANCHOU_PROJECT}")
if (INFOTM_LANCHOU_PROJECT STREQUAL "yes")
        add_definitions(-DINFOTM_LANCHOU_PROJECT)
        option(INFOTM_LANCHOU_PROJECT "project is LANCHOU" ON)
endif()
message("INFOTM_MNEON_SUPPORT: ${INFOTM_MNEON_SUPPORT}")
if (INFOTM_MNEON_SUPPORT STREQUAL "yes")
	add_definitions(-DUSE_MATH_NEON)
	option(MATH_NEON_ON "use math neon" ON)
endif()
message("INFOTM_HW_AWB: ${INFOTM_HW_AWB}")
if (INFOTM_HW_AWB STREQUAL "yes")
        add_definitions(-DINFOTM_HW_AWB)
        option(INFOTM_HW_AWB "project is LANCHOU" ON)
endif()

if (DEFINED CROSS_COMPILE)
    message(STATUS "CROSS_COMPILE=${CROSS_COMPILE}")

    # specify the cross compiler
    set(CMAKE_C_COMPILER "${CROSS_COMPILE}gcc")
    set(CMAKE_CXX_COMPILER "${CROSS_COMPILE}g++")

    # could specify C and CXX flags here
    #set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Os -fvisibility=hidden")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Os")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Os -fvisibility=hidden")

    # compiler flags should be modifiable in build/c_flag_overrides.cmake
	# and build/cxx_flag_overrides.cmake

    # search for lib and includes only in cross compile path
    set(CMAKE_FIND_ROOT_PATH "${CROSS_COMPILE}")

    set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
    set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
    set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
endif()

project (Felix_DDK)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/out/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/out/lib)

option(BUILD_CAPTUREINTERFACE "Build the Capture Interface (Felix Hardware Driver)" ON)
option(BUILD_ISPCONTROL "Build the ISP Control" ON)
option(BUILD_TEST_APPS "Build the Test applications" ON)
option(BUILD_GUI "Build the Felix GUIs using Qt 4 (Felix Param GUI, Felix Demo GUI, Tuning GUI) - you may need to define QT_BINARY_DIR if qmake isn't in your PATH" OFF)
option(BUILD_SENSORAPI "Build the Felix Sensor API" ON)
option(BUILD_UNIT_TESTS "Encourage libraries to build their unit tests" OFF)
option(BUILD_DONT_USE_P4 "Do not try to use perforce to get current revision" ON)
option(BUILD_SCB_DRIVER "Build the IMG SCB (I2C) driver as part of CI (needed on test platform or some HW versions)" OFF)
option(IMG_SCB_POLLING_MODE "Driver support polling mode instead of interrupt (interrupts are not supported by all HW versions!)" OFF)
option(CI_MEM_FR_ON "Build the CI memory machansim MEM FR" ON)
option(DEBUG_MODULES "Display CMake debug information" OFF)

if(NOT WIN32)
  option(ENABLE_GPROF "Enable gprof for gcc" OFF)
endif()
set(P4UTILS_BASE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

if (NOT DEFINED CMAKE_BUILD_TYPE)	
  if (WIN32)
    set(CMAKE_BUILD_TYPE "Debug")
  else()
    set(CMAKE_BUILD_TYPE "Release")
  endif()
endif()
#set(CMAKE_BUILD_TYPE "Debug")
message("CMake Build type: ${CMAKE_BUILD_TYPE}")

if (BUILD_GUI)
	# done here so that external library Qwt already has some variables defined
	include(GUI/CheckQtLocation.cmake)
endif()

set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
	${CMAKE_CURRENT_SOURCE_DIR}/bin/tools
	${CMAKE_CURRENT_SOURCE_DIR}/build
	${CMAKE_CURRENT_SOURCE_DIR}/tools/pthread_win32
)

option(ANDROID_BUILD "Build for Android (toolchain paths (C_COMPILER etc.) should be provided)." OFF)
# other androird options and checks delayed until CI information has been parsed (checking if building kernel module)

if (NOT DEFINED FELIXCOMMON_NDEBUG)
	if (CMAKE_BUILD_TYPE MATCHES "Release")
		set(FELIXCOMMON_NDEBUG ON)
	else()
		set(FELIXCOMMON_NDEBUG OFF)
	endif()
endif()
if (NOT DEFINED FELIXCOMMON_NPERF)
	if (CMAKE_BUILD_TYPE MATCHES "Release")
		set(FELIXCOMMON_NPERF ON)
	else()
		set(FELIXCOMMON_NPERF OFF)
	endif()
elseif (DEFINED ISPC_PERFLOG)
    # ensure that if ISPC_PERFLOG is on then FELIXCOMMON_NPERF is on too
    if (${ISPC_PERFLOG})
        if (${FELIXCOMMON_NPERF})
            message("Forcing FELIXCOMMON_NPERF to OFF because ISPC_PERFLOG=${ISPC_PERFLOG}")
            set(FELIXCOMMON_NPERF OFF)
        endif()
    endif()
endif()
if (DEBUG_MODULES)
	message("FELIXCOMMON_NDEBUG = ${FELIXCOMMON_NDEBUG}")
	message("FELIXCOMMON_NPERF = ${FELIXCOMMON_NPERF}")
endif()

#
# as GTEST is part of the build enable testing
#
enable_testing() # add_test can be used from now on

#
# tools are independent
# - either not compiled
# - or compiled with their own env
#
# tools are not given to the customer
#

message("CMAKE_CURRENT_SOURCE_DIR/tools = ${CMAKE_CURRENT_SOURCE_DIR}/tools")
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tools")
  set(TOOL_INSTALL_DIR ${Felix_DDK_BINARY_DIR}/tools/installed)
  set(GENERATE_TOOL_DEF2CODE ${TOOL_INSTALL_DIR}/def2code)
  set(CMAKE_PROGRAM_PATH ${CMAKE_PROGRAM_PATH} ${TOOL_INSTALL_DIR})
  add_subdirectory(tools)
endif()

include (common/UsePackage.cmake)
if (BUILD_CAPTUREINTERFACE)
	include (CI/UsePackage.cmake)
endif()
if(BUILD_SENSORAPI)
	include(sensorapi/UsePackage.cmake)
endif()
if (BUILD_ISPCONTROL)
	include(ISP_Control/UsePackage.cmake)
endif()
include (build/OverwriteFlags.cmake)
include (build/GenKernel.cmake)
include (build/RegdefFunctions.cmake)
include (build/PerforceUtils.cmake)

if(DEFINED QT_BINARY_DIR)
	message("Using QT_BINARY_DIR=${QT_BINARY_DIR}")
endif()

set(SAVE_LOCKS TRUE) # use pthread locks in sim_image common library
if(ANDROID_BUILD)
    if(${FORCE_32BIT_BUILD} MATCHES "ON")
        if(${CI_BUILD_KERNEL_MODULE})
            add_definitions(-DANDROID)
            message("This is Android build.")
        else()
            message(FATAL_ERROR "Android build is feasible only when CI_BUILD_KERNEL_MODULE option is selected!")
        endif()
    else()
        message(FATAL_ERROR "Only 32 bit build is supported for Android build! Please select FORCE_32BIT_BUILD option!")
    endif()

	if(${CI_DEVICE} STREQUAL "ANDROID_EMULATOR")
        message("Use Android emulator as a target.")
        add_definitions(-DANDROID_EMULATOR)
		# Disable SCB driver build when building for Android emulator
		set (BUILD_SCB_DRIVER OFF)
    else()
        message("Use FPGA as a target.")
    endif()
    
    set(SAVE_LOCKS FALSE)
endif()

#
# the current directory can contain some cpack configuration
#
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/cpack_sources.cmake")
  include(cpack_sources.cmake)
endif()

add_subdirectory(common)

if (BUILD_CAPTUREINTERFACE)
	add_subdirectory(CI)
endif()

if(BUILD_SENSORAPI)
	if(NOT BUILD_CAPTUREINTERFACE)
		message(FATAL_ERROR "ISP Control requires building the Capture Interface (CI) check BUILD_CAPTUREINTERFACE flag.")
	else()
	        set(SENSORAPI_UNIT_TESTS ${BUILD_UNIT_TESTS})
		add_subdirectory(sensorapi)
	endif()
endif()

if (BUILD_ISPCONTROL)
	if(NOT BUILD_CAPTUREINTERFACE)
		message(FATAL_ERROR "ISP Control requires building the Capture Interface (CI) check BUILD_CAPTUREINTERFACE flag.")
	else()
        set(ISPC_TESTS ${BUILD_UNIT_TESTS})
		add_subdirectory(ISP_Control)
	endif()
endif()

if (BUILD_GUI)
	add_subdirectory(GUI)
endif()

# documentation generation
add_subdirectory(doc)

install(FILES changelist.txt DESTINATION ./) 
