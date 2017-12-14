cmake_minimum_required(VERSION 2.8)

# ----------------------------------------------------------------------
# CMake file for "portfwrk" tal - included from target's CMakeLists
# ----------------------------------------------------------------------

find_package(PortFramework REQUIRED)
include_directories(${PORTFWRK_INCLUDE_DIRS})
add_definitions(${PORTFWRK_DEFINITIONS})
	
find_package(OSA REQUIRED)
include_directories(${OSA_INCLUDE_DIRS})
add_definitions(${OSA_DEFINITIONS})

if(NOT IMG_KERNEL_MODULE)
	find_package(ImgLib_GZIPIO REQUIRED)
	include_directories(${IMGLIB_GZIPIO_INCLUDE_DIRS})
	add_definitions(${IMGLIB_GZIPIO_DEFINITIONS})
endif()

find_package(ImgSystem REQUIRED)
include_directories(${IMGSYSTEM_INCLUDE_DIRS})
add_definitions(${IMGSYSTEM_DEFINITIONS})

if(TAL_BRIDGING)
	find_package(RPCSysBrg REQUIRED)
	
	include_directories(${RPCSYSBRG_INCLUDE_DIRS})
	add_definitions(${RPCSYSBRG_DEFINITIONS})
	
else()
	find_package(ImgSystem REQUIRED)
	
	include_directories(
		${IMGSYSTEM_INCLUDE_DIRS}
	)
	add_definitions(
		${IMGSYSTEM_DEFINITIONS}
	)
endif()

# ----------------------------------------------------------------------
# List of common header
# ----------------------------------------------------------------------
set (EXTRA ${EXTRA}
	portfwrk.cmake
)

set(HEADERS ${HEADERS}
	include/pftal_api.h
	include/tal_callback.h
)
	
# ----------------------------------------------------------------------
# List of headers and sources added to the common ones
# ----------------------------------------------------------------------
#
# SOURCES are user mode sources
#

set(KM_SOURCES ${KM_SOURCES}
	libraries/addr_alloc/code/addr_alloc.c
	libraries/utils/code/hash.c
	libraries/utils/code/pool.c
	libraries/utils/code/ra.c
	libraries/utils/code/trace.c
	libraries/tal/common/code/tal_test.c
	libraries/tal/port_fwrk/code/tal.c
)

if (TAL_BRIDGING)
	IMG_DEFINE_RPC_HEADERS(include/pftal_api.h)
	set(SOURCES ${SOURCES} ${IMG_RPC_CLIENT_FILES})
endif()

if(NOT IMG_KERNEL_MODULE)
	# ----------------------------------------------------------------------
	# Build the library
	# ----------------------------------------------------------------------
	if(TAL_BRIDGING)
		add_library(TAL STATIC ${SOURCES} ${IMG_RPC_CLIENT_FILES} ${HEADERS} ${EXTRA})
		
		set_source_files_properties(${IMG_RPC_CLIENT_FILES} PROPERTIES GENERATED 1)
		add_dependencies(TAL ${RPCSYSBRG_GEN_TARGET})
	else()
		add_library(TAL STATIC ${SOURCES} ${KM_SOURCES} ${HEADERS} ${EXTRA})
	endif()
	#target_link_libraries(TAL ${IMG_LIBRARIES})

	target_link_libraries(${TAL_LIBRARIES} ${PORTFWRK_LIBRARIES})

else()
	# ----------------------------------------------------------------------
	# Add information needed for KM build
	# ----------------------------------------------------------------------
	IMG_KM_ADD_INCLUDE_DIRS()
	IMG_KM_ADD_DEFINITIONS()
	IMG_KM_ADD_SOURCE("${KM_SOURCES}")
	IMG_KM_ADD_RPC_SERVER_FILES("${IMG_RPC_SERVER_FILES}")
endif()
