cmake_minimum_required(VERSION 2.8)

# ----------------------------------------------------------------------
# Cmake file for "normal" tal - included from target's CMakeLists
# ----------------------------------------------------------------------

# ----------------------------------------------------------------------
# List of headers and sources added to the common ones
# ----------------------------------------------------------------------
find_package(ImgLib_GZIPIO QUIET)
find_package(DEVIF REQUIRED)
include_directories(${IMGLIB_GZIPIO_INCLUDE_DIRS}
					${DEVIF_INCLUDE_DIRS})
add_definitions(${IMGLIB_GZIPIO_DEFINITIONS}
				${DEVIF_DEFINITIONS})

set(TAL_LINK_LIB ${TAL_LINK_LIB} ${DEVIF_LIBRARIES})

set(DEVIF_DEFINE_DEVIF1_FUNCS ${TAL_DEFINE_DEVIF1_FUNCS})
#set(DEVIF_DEBUG true)
set(DEVIF_USE_DEVICEINTERFACE ${TAL_USING_DEVIF})
set(DEVIF_EXCLUDE ${TAL_EXCLUDE_DEVIF})
set(DEVIF_PCI_DRIVER_HEADERS ${TAL_PREFIX}/imgpcidd/linux/code ${TAL_PREFIX}/imgpcidd/include)
set(DEVIF_USE_OSA ${TAL_USE_OSA})
set(DEVIF_USE_MEOS ${TAL_USE_MEOS})
add_subdirectory(libraries/devif)

# ----------------------------------------------------------------------
# Setup Device_interface if requested
# ----------------------------------------------------------------------
if(TAL_USING_DEVIF)
	list(FIND TAL_EXCLUDE_DEVIF socket soc_index)
	list(FIND TAL_EXCLUDE_DEVIF direct dir_index)
	if ((${soc_index} EQUAL -1) OR (${dir_index} EQUAL -1)) # Check socket and direct are not all in the exclude list
		add_subdirectory(device_interface)
	endif()
else()
	list(APPEND TAL_EXCLUDE_DEVIF socket direct)
endif()

# No PCI support in MacOs at the moment
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	list(APPEND TAL_EXCLUDE_DEVIF pci)
endif()

if(TAL_DEINIT_ON_EXIT)
    add_definitions(-DTAL_DEINIT_ON_EXIT)
endif()

if(NOT DEFINED TAL_DEBUG_DEVIF)
    set(TAL_DEBUG_DEVIF FALSE)
endif()
if(${TAL_DEBUG_DEVIF})
    add_definitions(-DTAL_DEBUG_DEVIF)
endif()


set (EXTRA ${EXTRA}
	normal.cmake
)

set(HEADERS ${HEADERS}
	include/pagemem.h
	include/pdump_cmds.h
	include/sgxmmu.h
)

set(SOURCES ${SOURCES}
	libraries/tal/normal/code/tal.c
	
	libraries/utils/code/hash.c
	libraries/utils/code/mmu.c
	libraries/utils/code/pool.c
	libraries/utils/code/ra.c
	libraries/utils/code/trace.c
	
	libraries/pdump/common/code/pdump_cmds.c

	libraries/tal/common/code/tal_interrupt.c
	libraries/tal/common/code/tal_test.c
	libraries/tal/common/code/tal_memspace.c
	
	libraries/addr_alloc/code/addr_alloc.c
	libraries/addr_alloc/code/pagemem.c
	libraries/addr_alloc/code/sgxmmu.c
)

if(TAL_CALLBACK_SUPPORT)
	set(HEADERS ${HEADERS}
		include/tal_callback.h
	)
	set(SOURCES ${SOURCES} 
		libraries/tal/normal/code/tal_callback.c
	)
endif()


# ----------------------------------------------------------------------
# Build the library
# ----------------------------------------------------------------------
add_library(${TAL_LIBRARIES} STATIC ${SOURCES} ${HEADERS} ${EXTRA})

