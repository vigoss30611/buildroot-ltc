
#
# from higher level
#
set (LIB_LINKS 
    ${TAL_LIBRARIES}
    ${LINKEDLIST_LIBRARIES}
    ${IMGMMU_LIBRARIES}
    ${FELIXCOMMON_LIBRARIES}
)

#
# additional files for user mode only
#
set(FAKE_LINUX_H 
	fake_include/linux/ioctl.h
)

set(FAKE_LINUX_C
)

if (NOT CI_BUILD_KERNEL_MODULE)
	message (STATUS "    using Posix Threads")
    # done in FindFelixAPI.cmake for user-space
    # but needed here too as we do not include all user-space includes in kernel
    if(WIN32)
        #find_package(PThreadWin32 REQUIRED) # should have been done already in FindFelixAPI.cmake
        include_directories(
            ${PTHREADWIN32_INCLUDE_DIRS}
            fake_include
        )
        add_definitions(${PTHREADWIN32_DEFINITIONS})
        set(LIB_LINKS ${LIB_LINKS} ${PTHREADWIN32_LIBRARIES})
    else()
        set(LIB_LINKS ${LIB_LINKS} -lpthread)
    endif()
  
    set (FAKE_LINUX_C ${FAKE_LINUX_C}
		platform_src/sys_parallel_posix.c
		platform_src/sys_device_posix.c
		platform_src/sys_mem_tal.c
    )
    
    set (SOURCES ${SOURCES}
		kernel_src/ci_debug.c
	)
    	
	source_group(System_posix FILES ${FAKE_LINUX_C})
endif()

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/include/)
include_directories(
  ${IMGINCLUDES_INCLUDE_DIRS}
  ${FELIXDG_INCLUDE_DIRS} # empty if not used
  ${FELIXKO_INCLUDE_DIRS}
  ${CMAKE_CURRENT_BINARY_DIR}/include/
  ${TAL_INCLUDE_DIRS}
  ${FelixLib_BINARY_DIR}/user/include/ # not included in the above CMakeLists to avoid overriding the user includes when building the kernel
)
add_definitions(
  ${IMGINCLUDES_DEFINITIONS}
)

source_group(System FILES 
	${FAKE_LINUX_H} 
	include/ci_internal/sys_device.h
	include/ci_internal/sys_mem.h
	include/ci_internal/sys_parallel.h
)

# no linking because TAL does the linking - it is just bodge because Felix LIB should init MeOS while it's TAL that uses it

if (CI_DEBUG_FUNCTIONS)
	find_package(ImgLib_GZIPIO REQUIRED)
	message(STATUS "    CI Debug Functions")

	include_directories(${IMGLIB_GZIPIO_INCLUDE_DIRS})
	add_definitions(${IMGLIB_GZIPIO_DEFINITIONS}) # CI_DEBUG_FCT should be defined by FindFelixAPI
	
	if (FELIX_DUMP_COMPRESSED_REGISTERS)
		add_definitions(-DFELIX_DUMP_COMPRESSED_REGISTERS=IMG_TRUE)
	endif()

	set (LIB_LINKS ${LIB_LINKS} ${IMGLIB_GZIPIO_LIBRARIES})
endif()

if (CI_EXT_DATA_GENERATOR)
	find_package(FelixDG REQUIRED)
	set (LIB_LINKS ${LIB_LINKS})
endif()

#
# ONLY static
#
add_library(${FELIXINTERNAL_NAME} STATIC ${SOURCES} ${FAKE_LINUX_C} ${HEADERS} ${FAKE_LINUX_H})

target_link_libraries(${FELIXINTERNAL_NAME} ${LIB_LINKS})
add_dependencies (${FELIXINTERNAL_NAME} ${FELIXAPI_DEPENDENCIES})
CopyVHCOutput(${FELIXINTERNAL_NAME} ci_target.h ${CMAKE_CURRENT_BINARY_DIR}/include/ci_target.h)
