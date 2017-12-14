cmake_minimum_required(VERSION 2.8)

if (NOT CMAKE_BUILD_TYPE)
  if (WIN32)
    set(CMAKE_BUILD_TYPE "Debug")
  else()
    set(CMAKE_BUILD_TYPE "Release")
  endif()
endif()

#option (FELIX_MEM_CHECK "Enable memory checking for felix" OFF)
option (CI_EXT_DATA_GENERATOR "use the data generator in felix" OFF)
if (NOT DEFINED CI_EXT_DG_PLL_WRITE)
    set(CI_EXT_DG_PLL_WRITE TRUE)
endif()
option (CI_DEBUG_FUNCTIONS "enable debugFS for CI - for fake driver enable use of debug functions" ON)
option (CI_MEMSET_ADDITIONAL_MEMORY "enable memset to 0xAA of additional device memory rounding up allocations to CPU page size" ON)
option (CI_MEMSET_ALLOCATED_MEMORY "enable memset of device memory to 0x0" OFF)

if (WIN32)
  set (FELIX_USE_LOCAL_ZLIB ON)
	
  set(CI_BUILD_KERNEL_MODULE FALSE)
  
  #if (NOT CI_EXT_DATA_GENERATOR)
  #  message("Warning: Data-generator OFF - may not be able to run if HW does not support internal DG")
  #  set(CI_EXT_DATA_GENERATOR ON)
  #endif()
  
else(WIN32)
  #option (FELIX_USE_LOCAL_ZLIB "Build Felix using the local ZLib" OFF)
  option (CI_BUILD_KERNEL_MODULE "Build Felix API as a kernel module" ON)

  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -ldl -Wl,-rpath-link -Wl,")

  option(FORCE_32BIT_BUILD "Build a 32bit binary on a 64bit system" OFF)
  if(${FORCE_32BIT_BUILD} MATCHES "ON")
      if (NOT ANDROID_BUILD) # Arm compilers do not recognise -m32        
        add_definitions(-m32)
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -m32")
        set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -m32")
        set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -m32")
    endif()
  endif()
  
  option (CI_CODE_COVERAGE "Use GCovr to compute code coverage (use with GCC)" OFF)
endif()

if (NOT DEFINED CI_DEVICE)
	set(CI_DEVICE "MEMMAPPED")
    if (CI_BUILD_KERNEL_MODULE) # when building fake driver the choice does not matter
        message("Selecting default device ${CI_DEVICE}")
    endif()
endif()

if (${CI_DEVICE} STREQUAL "ANDROID_EMULATOR")
	set(ANDROID_BUILD ON)
endif()

if (NOT DEFINED CI_ALLOC)
	if (${CI_DEVICE} STREQUAL "ANDROID_EMULATOR")
		set(CI_ALLOC "PAGEALLOC")
	else()
		if (${CI_MEM_FR_ON} MATCHES "ON")
			set(CI_ALLOC "MEMFR")
			add_definitions(-DINFOTM_CI_MEM_FR_ON)			
		else()	
			set(CI_ALLOC "CARVEOUT")
		endif()	
	endif()
    if (CI_BUILD_KERNEL_MODULE) # when building fake driver the chocie does not matter
        message("Selecting default allocator ${CI_ALLOC}")
    endif()
endif()


if (NOT DEFINED PLATFORM)
    if (${CI_ALLOC} STREQUAL "IMGVIDEO")
        message(FATAL_ERROR "IMGVIDEO needs a platform to be selected")
    endif()
    set(PLATFORM "unknown") # just to be able to pass STREQUAL tests
endif()
if (NOT DEFINED PORTFWRK_MEMALLOC_UNIFIED_VMALLOC)
    set(PORTFWRK_MEMALLOC_UNIFIED_VMALLOC FALSE)
endif()
if (NOT DEFINED PORTFWRK_SYSMEM_CARVEOUT)
    set(PORTFWRK_SYSMEM_CARVEOUT FALSE)
endif()

# Turn on Bus Master mode (FPGA access system memory instead of carveout)
# automatically when PCI device is selected and allocator is PAGEALLOC
# or using IMGVIDEO with IMG FPGA and page alloc
if (${CI_DEVICE} STREQUAL "PCI" AND
    (
        (${CI_ALLOC} STREQUAL "PAGEALLOC")
        OR ((${CI_ALLOC} STREQUAL "IMGVIDEO") AND (${PLATFORM} STREQUAL "img_fpga") AND ${PORTFWRK_MEMALLOC_UNIFIED_VMALLOC})
    )
   )
    
    set(FPGA_BUS_MASTERING ON)
else()
	set(FPGA_BUS_MASTERING OFF)
endif()


if (NOT DEFINED SYSMEM_DMABUF_IMPORT)
    set(SYSMEM_DMABUF_IMPORT FALSE)
endif()
if (NOT ${CI_ALLOC} STREQUAL "IMGVIDEO")
    if(${SYSMEM_DMABUF_IMPORT})
        message("SYSMEM_DMABUF_IMPORT disabled because not using CI_ALLOC=IMGVIDEO")
        set(SYSMEM_DMABUF_IMPORT FALSE)
    endif()
endif()

if (CI_BUILD_KERNEL_MODULE)
  
  if (NOT DEFINED LINUX_KERNEL_BUILD_DIR)
    execute_process(COMMAND uname -r OUTPUT_VARIABLE KERNEL_VER OUTPUT_STRIP_TRAILING_WHITESPACE)
    set (LINUX_KERNEL_BUILD_DIR "/lib/modules/${KERNEL_VER}/build")
    message ("LINUX_KERNEL_BUILD_DIR not define: using kernel ${KERNEL_VER}")
  else()
	message ("Building with linux kernel '${LINUX_KERNEL_BUILD_DIR}'")
  endif()

endif(CI_BUILD_KERNEL_MODULE)

# ----------------------------------------------------------------------
# Configure ImgIncludes
# ----------------------------------------------------------------------
if(CMAKE_BUILD_TYPE MATCHES "Release")
    set(IMGINCLUDES_EXIT_ON_ASSERT TRUE)
else()
    set(IMGINCLUDES_EXIT_ON_ASSERT FALSE)
endif()

# ----------------------------------------------------------------------
# Configure ImgLib
# ----------------------------------------------------------------------

if (CI_BUILD_KERNEL_MODULE)
  if (BUILD_UNIT_TESTS)
    message("Disabling CI unit tests because building KO")
  endif()
  set (IMGLIB_GZIPIO_TESTS OFF)
  set (CAPTUREINTERFACE_TESTS OFF)
  set (IMGMMU_TESTS OFF)
  set (TAL_TESTS OFF)
else()
set (IMGLIB_GZIPIO_TESTS ${BUILD_UNIT_TESTS})
set (CAPTUREINTERFACE_TESTS ${BUILD_UNIT_TESTS})
set (IMGMMU_TESTS ${BUILD_UNIT_TESTS})
set (TAL_TESTS ${BUILD_UNIT_TESTS})
endif()


# ----------------------------------------------------------------------
# Configure TAL
# ----------------------------------------------------------------------
set(TAL_MALLOC_MEM_CBS              TRUE)
set(TAL_EXCLUDE_MEM_API             TRUE)
set(TAL_USING_DEVIF                 TRUE)
if (NOT CI_BUILD_KERNEL_MODULE)
  set(DEVIF_USE_MEOS FALSE)
  set(DEVIF_USE_OSA TRUE)
  set(TAL_USE_OSA TRUE)
  set(TAL_TYPE "normal")
  set(TAL_EXCLUDE_DEVIF pci bmem dash pdump1 posted dummy) # direct or transif?
else()
  set(TAL_TYPE "light")
  set(TAL_TARGET_NAME "felix")
endif()
set(TAL_MEMORY_BITMAP               TRUE)
set(TAL_DEINIT_ON_EXIT              FALSE)

# ----------------------------------------------------------------------
# Other configurations
# ----------------------------------------------------------------------
# MMU size
set(IMGMMU_SIZE 32) # use 32b physical size
set(IMGMMU_TALMEM_MMU_NAME "MMU_PAGE") # optional - name of the memory block in pdump
#IMGMMU_TALMEM_NAME not use (default is BLOCK_X)

include(TestBigEndian)

TEST_BIG_ENDIAN(FELIX_SYSTEM_BIGENDIAN)
