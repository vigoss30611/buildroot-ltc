cmake_minimum_required(VERSION 2.8)

set(IMGINCLUDES_FORCE_KERNEL ${CI_BUILD_KERNEL_MODULE})
find_package(ImgIncludes REQUIRED)
find_package(TAL REQUIRED)
find_package(RegDefs REQUIRED)
find_package(RegDefsUtils REQUIRED)
find_package(LinkedList REQUIRED)
find_package(FelixAPI REQUIRED)
find_package(IMGMMU REQUIRED)
#find_package(FelixCommon REQUIRED) # done when finding FelixAPI

#find_package(GenDoxyfile QUIET) # to have doxygen aliases and generate documentation
#find_package(Doxygen QUIET) # find doxygen executable


#
# log level: 0=none, 1=fatal, 2=fatal+warnings, 3=all, 4=extra (including debug messages)
#
if (NOT CI_LOG_LEVEL)
    if (CMAKE_BUILD_TYPE MATCHES "Release")
        set (CI_LOG_LEVEL 1)
    else() 
        set (CI_LOG_LEVEL 3)
    endif()
endif()

if (NOT DEFINED ANDROID_BUILD)
  set(ANDROID_BUILD FALSE)
endif()
if (NOT DEFINED PLATFORM)
    # port framework platform is not present if not using imgvideo
    # but needed for following if
    set(PLATFORM "none")
endif()

if ( ((${CI_DEVICE} STREQUAL "PCI") AND (${CI_ALLOC} STREQUAL "CARVEOUT"))
    OR (NOT ${CI_BUILD_KERNEL_MODULE})
    OR ((${CI_ALLOC} STREQUAL "IMGVIDEO") AND (${PLATFORM} STREQUAL "img_fpga") AND ${PORTFWRK_SYSMEM_CARVEOUT}) )
    # on linux PDP is available only if using FPGA with carved out memory
    # on fake mode PDP is enabled to have it in the pdumps
    if (NOT DEFINED FELIX_PDP)
        set(FELIX_PDP TRUE)
    endif()
else()
    set(FELIX_PDP FALSE)
endif()

set (FELIXINTERNAL_NAME "${FELIXAPI_NAME}_Kernel")

set (FELIXKO_INCLUDE_DIRS
    #${IMGINCLUDES_INCLUDE_DIRS} # special rule for IMG includes - see build_kernel.cmake
    ${CMAKE_CURRENT_SOURCE_DIR}/include/
    ${FelixLib_SOURCE_DIR}/user/include/ # needed for the structures definitions
    #${FelixLib_BINARY_DIR}/user/include/ # needed for the version header but if included here will override the user includes
    ${REGDEF_INCLUDE_DIRS}
    ${REGDEFSUTILS_INCLUDE_DIRS}
    ${LINKEDLIST_INCLUDE_DIRS}
    ${FELIXCOMMON_INCLUDE_DIRS}
    ${IMGMMU_INCLUDE_DIRS}
)
add_definitions(
    ${FELIXAPI_DEFINITIONS}
    ${TAL_DEFINITIONS}
    ${REGDEFSUTILS_DEFINITIONS}
    ${LINKEDLIST_DEFINITIONS}
    ${FELIXCOMMON_DEFINITIONS}
    ${IMGMMU_DEFINITIONS}
    -DCI_LOG_LEVEL=${CI_LOG_LEVEL}
    #-DFELIX_DEBUG_INTERRUPT
)

# can add -DCI_REGPOLL_TIMEOUT int
if (NOT DEFINED CI_HW_REF_CLK)
    set(CI_HW_REF_CLK 40) # reference clock of the HW in MHz
endif()

message(STATUS "Building ISP with reference clock: ${CI_HW_REF_CLK}MHz")
add_definitions(-DCI_HW_REF_CLK=${CI_HW_REF_CLK})

#
# use ${CMAKE_CURRENT_SOURCE_DIR}/ because var are used from other cmake contexts (when building kernel module)
# list is duplicated in unit tests
#
set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/platform_src/sys_mem.c
    ${CMAKE_CURRENT_SOURCE_DIR}/kernel_src/ci_internal.c
    ${CMAKE_CURRENT_SOURCE_DIR}/kernel_src/ci_config.c
    ${CMAKE_CURRENT_SOURCE_DIR}/kernel_src/ci_capture.c
    ${CMAKE_CURRENT_SOURCE_DIR}/kernel_src/ci_shot.c
    ${CMAKE_CURRENT_SOURCE_DIR}/kernel_src/ci_hwstruct.c
    ${CMAKE_CURRENT_SOURCE_DIR}/kernel_src/ci_interrupt.c
    ${CMAKE_CURRENT_SOURCE_DIR}/kernel_src/ci_connection.c
    ${CMAKE_CURRENT_SOURCE_DIR}/kernel_src/ci_ioctl_km.c
    ${CMAKE_CURRENT_SOURCE_DIR}/kernel_src/ci_mmu.c
    ${CMAKE_CURRENT_SOURCE_DIR}/kernel_src/ci_intdg_km.c
    ${CMAKE_CURRENT_SOURCE_DIR}/kernel_src/ci_debugfs.c
)

set(HEADERS
    include/ci_kernel/ci_kernel_structs.h
    include/ci_kernel/ci_kernel.h
    include/ci_kernel/ci_connection.h
    include/ci_kernel/ci_hwstruct.h
    include/ci_kernel/ci_ioctrl.h
    include/ci_kernel/ci_debug.h # included as a header but contains functions only if CI_DEBUG_FUNCTIONS and not CI_BUILD_KERNEL_MODULE
    
    include/ci_internal/sys_mem.h
    include/ci_internal/sys_parallel.h
    include/ci_internal/sys_device.h
    include/ci_internal/ci_errors.h
)

if (FELIX_PDP)
  message(STATUS "    PDP support")
    add_definitions(-DCI_PDP)
    # pdp functions should be implemented in SYS_Mem/SYS_Dev implementations
    set(HEADERS ${HEADERS}
        include/ci_internal/ci_pdp.h
    )
endif()

if(CI_MEMSET_ALLOCATED_MEMORY)
    message(STATUS "    support memset of allocated memory to 0x0")
    add_definitions(
        -DCI_MEMSET_ALLOC=0x0 # optional, to set additional memory to a specific value when allocated
    )
endif()
if(CI_MEMSET_ADDITIONAL_MEMORY)
    message(STATUS "    support memset of rounded-up memory to 0xAA")
    add_definitions(
        -DCI_MEMSET_ALIGNMENT=0xAA # optional, to set memory to a specific value when allocated
    )
endif()

if (CI_BUILD_KERNEL_MODULE)
  
  include(build_kernel.cmake)
  
else()

  include(build_library.cmake)
  
endif()
