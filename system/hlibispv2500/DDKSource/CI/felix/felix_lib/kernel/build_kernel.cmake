find_package(PortFramework QUIET)

# ISP_KM_BINARY_DIR defined in felix/ folder

if (DEFINED IMGINCLUDES_BUILD_KO)
  include(${IMGINCLUDES_BUILD_KO})
else()
  message (FATAL_ERROR "no ImgIncludes kernel sources")
endif()

if (DEFINED TAL_BUILD_KO)
  include(${TAL_BUILD_KO})
else()
  message (FATAL_ERROR "no TAL kernel sources")
endif()

if (DEFINED LINKEDLIST_BUILD_KO)
  include(${LINKEDLIST_BUILD_KO})
else()
  message (FATAL_ERROR "no Linked list kernel source")
endif()

if (DEFINED FELIXCOMMON_BUILD_KO)
  include(${FELIXCOMMON_BUILD_KO})
else()
  message (FATAL_ERROR "no Felix Common kernel source")
endif()

if (DEFINED FELIXDG_BUILD_KO)
  include(${FELIXDG_BUILD_KO})
elseif(CI_EXT_DATA_GENERATOR)
  message(FATAL_ERROR "no FelixDG sources")
endif()
# if CI_EXT_DATA_GENERATOR is FALSE the FELIXDG_ variables are empty but it is ok because cmake does not care

if (DEFINED IMGMMU_BUILD_KO)
  include(${IMGMMU_BUILD_KO})
else()
  message (FATAL_ERROR "no IMG MMU kernel source")
endif()

if (DEFINED PORTFWRK_BUILD_KO)
  include(${PORTFWRK_BUILD_KO})
  # no error when missing because IMGVIDEO is optional
endif()

if (${CI_DEVICE} STREQUAL "ANDROID_EMULATOR") # Use transif to connect emulator with simulator
  set (KO_SYS_DEVICE_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/platform_src/sys_device_transif_kernel.c)
  if(${BUILD_SCB_DRIVER})
    # should be checked for at top level CMakeLists
    message(FATAL_ERROR "Cannot support SCB driver on device ${CI_DEVICE}")
  endif()
elseif(${CI_DEVICE} STREQUAL "PCI")

  set (KO_SYS_DEVICE_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/platform_src/sys_device_pci_kernel.c
  )

  if (FPGA_BUS_MASTERING)
    add_definitions(-DFPGA_BUS_MASTERING)
  endif()

  if (DEFINED FPGA_INSMOD_RECLOCK)
    message(STATUS "    PCI driver with reclock at insmod to ${CI_HW_REF_CLK}MHz is ${FPGA_INSMOD_RECLOCK}")
    if(FPGA_INSMOD_RECLOCK)
      add_definitions(-DFPGA_INSMOD_RECLOCK=${CI_HW_REF_CLK})
    endif()
  endif()
  
elseif(${CI_DEVICE} STREQUAL "MEMMAPPED")
  set (KO_SYS_DEVICE_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/platform_src/sys_device_memmapped.c)
elseif(${CI_DEVICE} STREQUAL "IMGVIDEO")
    message(FATAL_ERROR "IMGVIDEO device is obsolete - IMGVIDEO is only valid as an allocator now")
else()
  message (FATAL_ERROR "no device selected - is CI_DEVICE defined?")
endif()

if (${BUILD_SCB_DRIVER})
    set (KO_SYS_DEVICE_SOURCES ${KO_SYS_DEVICE_SOURCES}
      ${CMAKE_CURRENT_SOURCE_DIR}/platform_src/i2c-img.c)
    add_definitions(-DIMG_SCB_DRIVER)
    # SCB driver has additional clock define because there should be
    # no clock programming in FPGA board when FPGA_REF_CLK is not defined
    add_definitions(-DSCB_REF_CLK=${CI_HW_REF_CLK})
    message(STATUS "    support SCB device")
endif()

if (NOT DEFINED KO_MODE)
  set (KO_MODE "m")
endif()

set (KO_SOURCES ${SOURCES}
  ${CMAKE_CURRENT_SOURCE_DIR}/platform_src/sys_parallel_kernel.c
  ${CMAKE_CURRENT_SOURCE_DIR}/kernel_src/ci_init_km.c
  ${KO_SYS_DEVICE_SOURCES}
  ${TAL_KO_SOURCES}
  ${LINKEDLIST_KO_SOURCES}
  ${FELIXCOMMON_KO_SOURCES}
  ${FELIXDG_KO_SOURCES}
  ${IMGMMU_KO_SOURCES}
  ${IMGINCLUDES_KO_SOURCES}
  ${PORTFWRK_KO_SOURCES} # if not building with IMGVIDEO this is empty

  ${FELIXKO_INCLUDE_DIRS}
  ${FELIXDG_INCLUDE_DIRS} # empty if no DG
  ${TAL_KO_INCLUDE_DIRS}
  ${FELIXDG_KO_INCLUDE_DIRS}
  ${LINKEDLIST_KO_INCLUDE_DIRS}
  ${IMGMMU_KO_INCLUDE_DIRS}
  ${IMGINCLUDES_KO_INCLUDE_DIRS}
  ${PORTFWRK_KO_INCLUDE_DIRS} # if not building with IMGVIDEO this is empty
  )

if (CI_ALLOC STREQUAL "ION")
  message(FATAL_ERROR "CI_ALLOC=ION option is obsolete! ION is automatically used when ANDROID_BUILD is defined and CI_ALLOC choice changes its implementation")
endif()

if (ANDROID_BUILD)

  # two cases exist : 
  # 1. kernel is build in it's own source tree, (no O= parameter provided) - headers can be found in LINUX_KERNEL_BUILD_DIR
  # 2. kernel objects are stored outside source tree - headers cannot be found in LINUX_KERNEL_BUILD_DIR
  # therefore the path to the kernel sources must be provided in LINUX_KERNEL_SOURCE_DIR
  if(DEFINED LINUX_KERNEL_SOURCE_DIR)
    set(ION_INCLUDES_ROOT ${LINUX_KERNEL_SOURCE_DIR})
  else()
    set(ION_INCLUDES_ROOT ${LINUX_KERNEL_BUILD_DIR})
  endif()
  if (EXISTS ${ION_INCLUDES_ROOT}/drivers/staging/android/ion/ion.h)
    add_definitions(-DIMG_KERNEL_ION_HEADER="<${ION_INCLUDES_ROOT}/drivers/staging/android/ion/ion.h>")
    add_definitions(-DIMG_KERNEL_ION_PRIV_HEADER="<${ION_INCLUDES_ROOT}/drivers/staging/android/ion/ion_priv.h>")
  elseif (EXISTS ${ION_INCLUDES_ROOT}/include/linux/ion.h)
    add_definitions(-DIMG_KERNEL_ION_HEADER='<linux/ion.h>')
    add_definitions(-DIMG_KERNEL_ION_PRIV_HEADER="<${ION_INCLUDES_ROOT}/drivers/gpu/ion/ion_priv.h>")
  else()
    message(FATAL_ERROR "Couldn't find ion.h under ${ION_INCLUDES_ROOT}. Try to define LINUX_KERNEL_SOURCE_DIR if kernel is built with O= make parameter")
  endif()

  # In Android builds use ION allocator
  add_definitions(-fno-pic)
  
  if (CI_ALLOC STREQUAL "CARVEOUT")
    add_definitions(-DALLOC_CARVEOUT_MEM)
  elseif (CI_ALLOC STREQUAL "PAGEALLOC")
    add_definitions(-DALLOC_SYSTEM_MEM)
  endif()
  
  set(KO_SOURCES ${KO_SOURCES}
    ${CMAKE_CURRENT_SOURCE_DIR}/platform_src/sys_mem_ion.c
  )
    
elseif (CI_ALLOC STREQUAL "CARVEOUT")
  set(KO_SOURCES ${KO_SOURCES}
    ${CMAKE_CURRENT_SOURCE_DIR}/platform_src/sys_mem_carveout.c)
elseif (CI_ALLOC STREQUAL "MEMFR")
  set(KO_SOURCES ${KO_SOURCES}
    ${CMAKE_CURRENT_SOURCE_DIR}/platform_src/sys_mem_fr.c)    
elseif (CI_ALLOC STREQUAL "IMGVIDEO")

  set(KO_SOURCES ${KO_SOURCES}
    ${CMAKE_CURRENT_SOURCE_DIR}/platform_src/sys_mem_imgvideo.c
  )
  
  add_definitions(-DIMGVIDEO_ALLOC) # used in sys device to know we are using IMGVIDEO
  
  if(NOT DEFINED PORTFWRK_BUILD_KO)
    message(FATAL_ERROR "Portability Framework not found! Required for IMGVIDEO support.")
  endif()

  if(NOT DEFINED ION_BUFFERS)
    set(ION_BUFFERS FALSE)
  endif()
  
  message(STATUS "    IMGVideo support ION: ${ION_BUFFERS}")
  
  if(${ION_BUFFERS})
    add_definitions(-DIMGVIDEO_ION)
  endif()

  if(NOT DEFINED PORTFWRK_MEMALLOC_UNIFIED_VMALLOC)
    set(PORTFWRK_MEMALLOC_UNIFIED_VMALLOC FALSE)
  endif()
  
  message(STATUS "    IMGVideo unified vmalloc: :${PORTFWRK_MEMALLOC_UNIFIED_VMALLOC}")
  
  if(${PORTFWRK_MEMALLOC_UNIFIED_VMALLOC})
    # similar to PAGEALLOC
    # when using ION imgvideo uses the system heap if this is defined
    add_definitions(-DSYSMEM_UNIFIED_VMALLOC)
  endif()

elseif (CI_ALLOC STREQUAL "PAGEALLOC")
  set(KO_SOURCES ${KO_SOURCES}
    ${CMAKE_CURRENT_SOURCE_DIR}/platform_src/sys_mem_pagealloc.c)
else()
  message (FATAL_ERROR "no allocator selected - is CI_ALLOC defined?")

endif()

message(STATUS "    use CI_DEVICE=${CI_DEVICE}")
message(STATUS "    use CI_ALLOC=${CI_ALLOC}")

add_definitions(
  -DIMG_KERNEL_MODULE
  ${TAL_KO_DEFINITIONS}
  ${LINKEDLIST_KO_DEFINITIONS}
  ${FELIXDG_KO_DEFINITIONS}
  ${IMGMMU_KO_DEFINITIONS}
  ${IMGINCLUDES_KO_DEFINITIONS}
  ${PORTFWRK_KO_DEFINTIONS}
  )

if (CI_DEBUG_FUNCTIONS)
  message(STATUS "    enable debugfs and debug functions")
  add_definitions(-DCI_DEBUGFS)
  add_definitions(-DCI_DEBUG_FCT)
endif()

set(KO_INCLUDES) # empty
#get_directory_property(KO_INCLUDES INCLUDE_DIRECTORIES)
#GenSpaceList("${KO_INCLUDES}" "-I" KO_INCLUDES)
get_directory_property(KO_CFLAGS DEFINITIONS) # spaces and -D

file(REMOVE_RECURSE ${ISP_KM_BINARY_DIR})

if (DEFINED REGDEF_BINARY_INCLUDE) # if generating VHC files we need to copy the cmake output
  set(KO_INCLUDES
    ${REGDEF_BINARY_INCLUDE}
    ${REGDEF_VHC_OUT}
    ${FelixLib_BINARY_DIR}/user/include
    ${LINUX_KERNEL_BUILD_DIR}/arch/arm/mach-apollo/include
    )

  # remove binary include folder otherwise it gets over-written when creating the links (removes the sources ci include folder)
  list(REMOVE_ITEM KO_SOURCES ${FelixLib_BINARY_DIR}/user/include)
endif()

list(REMOVE_DUPLICATES KO_CFLAGS) # because some cflags are duplicated (like TAL_LIGHT)
#list(REMOVE_DUPLICATES KO_INCLUDES) # should not be needed!

if(NOT DEFINED KO_MODULE_NAME)
    set(KO_MODULE_NAME "Felix")
endif()

if (DEBUG_MODULES)
  message ("KO_INCLUDES=${KO_INCLUDES}")
  message ("KO_CFLAGS=${KO_CFLAGS}")
endif()
GenSpaceList("${KO_INCLUDES}" "-I" KO_INCLUDES)

GenKernel_Makefile(${KO_MODULE_NAME} "${KO_CFLAGS}" "${KO_SOURCES}" "${KO_INCLUDES}" ${ISP_KM_BINARY_DIR} ${KO_MODE})
file(COPY
  readme.txt MIT_COPYING GPLHEADER
  DESTINATION ${ISP_KM_BINARY_DIR})

# compile with sparse if found
if (${KO_MODE} MATCHES "m")
  if (NOT DEFINED PORTFWRK_BUILD_KO)
    set(FOUND_SPARSE)
    find_program(FOUND_SPARSE sparse)

    if (NOT ${FOUND_SPARSE} MATCHES "FOUND_SPARSE-NOTFOUND")
      # use sparse to generate more warnings
      message(STATUS "    Using sparse to generate more warnings")
      set(KO_OPT "${KO_OPT} C=1")
    endif()
  
    if (DEFINED CROSS_COMPILE)
      message(STATUS "    Using cross compile path ${CROSS_COMPILE}")
      set(KO_OPT ${KO_OPT} CROSS_COMPILE=${CROSS_COMPILE})
    endif()

    GenKernel_addSpecialTarget(${FELIXINTERNAL_NAME}_KO "${KO_OPT}" ${ISP_KM_BINARY_DIR})

    #set (MODULE TRUE)
    #set (BRIEF "Capture Interface - felix low level driver")
    #set (DEFAULT FALSE)
    #set (HELP "CI API is used to control the felix hardware.")
    #set (DEPENDS "")
    #GenKernel_Kconfig(${FELIXINTERNAL_NAME} ${MODULE} ${BRIEF} ${DEFAULT} ${HELP} ${ISP_KM_BINARY_DIR} ${DEPENDS})

    if (NOT DEFINED QUICK_BUILD_ISPDDK)
        add_custom_command(TARGET ${FELIXINTERNAL_NAME}_KO
          PRE_BUILD
          COMMAND ${CROSS_COMPILE}strip -d ${ISP_KM_BINARY_DIR}/Felix.ko
        )

        add_custom_command(TARGET ${FELIXINTERNAL_NAME}_KO
          PRE_BUILD
          COMMAND ${CMAKE_MAKE_PROGRAM} -C ${LINUX_KERNEL_BUILD_DIR} ${KO_OPT} M=${ISP_KM_BINARY_DIR} INSTALL_MOD_PATH=$ENV{TARGET_DIR} modules_install
        )
    endif()

    add_dependencies (${FELIXINTERNAL_NAME}_KO ${FELIXAPI_DEPENDENCIES})

  endif()

  install(FILES ${ISP_KM_BINARY_DIR}/Felix.ko DESTINATION ./km)
endif()
