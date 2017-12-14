cmake_minimum_required(VERSION 2.8)

project(TAL)

message(STATUS "Target Abstraction Layer")

find_package(TAL REQUIRED) # trick to avoid duplication
find_package(GTest QUIET) # optional, to know if we build tests

# ----------------------------------------------------------------------
# External dependencies
# ----------------------------------------------------------------------
# because we don't want to die every time the TAL normal assert on bad return code
# we want the return code!
if(${TAL_TYPE} MATCHES "normal")
    set(IMGINCLUDES_EXIT_ON_ASSERT FALSE)
endif()
find_package(ImgIncludes REQUIRED)
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux" OR ${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    find_package(Threads REQUIRED)
endif()

if (NOT IMG_KERNEL_MODULE)
	if (NOT ${IMGLIB_GZIPIO_FOUND})
		message(FATAL_ERROR "GZIP FileIO library is needed by the TAL")
	endif()
endif()

include_directories(
	${IMGINCLUDES_INCLUDE_DIRS}
)

add_definitions(
	${IMGINCLUDES_DEFINITIONS}
)



if(TAL_CALLBACK_SUPPORT)
	message(STATUS "    TAL callback")
	
    find_package(ImgLib_CBMAN REQUIRED)
    find_package(ImgLib_LISTMAN REQUIRED)
	
    include_directories(
		${IMGLIB_CBMAN_INCLUDE_DIRS}
		${IMGLIB_LISTMAN_INCLUDE_DIRS}
	)
	add_definitions(
		${IMGLIB_CBMAN_DEFINITIONS}
		${IMGLIB_LISTMAN_DEFINITIONS}
	)
	
	set(TAL_LINK_LIB ${TAL_LINK_LIB}
		${IMGLIB_CBMAN_LIBRARIES}
		${IMGLIB_LISTMAN_LIBRARIES}
	)
endif()



# ----------------------------------------------------------------------
# Internal definitions
# ----------------------------------------------------------------------
add_definitions(
	${TAL_DEFINITIONS}
	-DPDUMP1_NO_RELATIVE_OFFSET
)


# ----------------------------------------------------------------------
# Common header files
# ----------------------------------------------------------------------
include_directories(${TAL_INCLUDE_DIRS})
include_directories(libraries/tal/include)

set (EXTRA
	FindTAL.cmake
)

set(INCLUDES
	include/tal.h
	include/tal_mem.h
	include/tal_debug.h
	include/tal_defs.h
	include/tal_intvar.h
	include/tal_pdump.h
	include/tal_reg.h
	include/tal_setup.h
	include/tal_vmem.h
	libraries/tal/include/tal_internal.h
	libraries/tal/include/tal_intdefs.h
	include/target.h
)

#
# each version of the TAL (light, normal, portability framework) defines the ${TAL_LIBRARIES} library
# it also links with the specific libraries it found
# common linking is done bellow
#

if(${TAL_TYPE} MATCHES "light")
    include(light)
	# TAL light needs no linking
else()
  find_package(MXML REQUIRED)
  find_package(ImgLib_ListUtils REQUIRED)
  
  include_directories(
	${MXML_INCLUDE_DIRS}
	${IMGLIB_LISTUTILS_INCLUDE_DIRS}
	)
  add_definitions(
	${MXML_DEFINITIONS}
	${IMGLIB_LISTUTILS_DEFINITIONS}
	)
  
	# ----------------------------------------------------------------------
	# TAL Thread Locking Selection
	# ----------------------------------------------------------------------

	if(DEFINED TAL_MAKE_THREADSAFE OR DEFINED TAL_NO_OSAL)
		message(FATAL_ERROR "TAL_MAKE_THREADSAFE and TAL_NO_OSAL are depecrated use TAL_USE_MEOS or TAL_USE_OSA for TAL to be threadsafe")
	endif()

	if (TAL_USE_MEOS)
		find_package(MeOSAL REQUIRED)
		include_directories(${MEOSAL_INCLUDE_DIRS})
		add_definitions(
			${MEOSAL_DEFINITIONS}
			-D__TAL_USE_MEOS__
		)
		message(STATUS "    TAL uses MeOS")
		set(TAL_LINK_LIB ${TAL_LINK_LIB} ${MEOSAL_LIBRARIES})
	elseif(TAL_USE_OSA)
		find_package(OSA REQUIRED)
		include_directories(${OSA_INCLUDE_DIRS})
		add_definitions(
			${OSA_DEFINITIONS}
			-D__TAL_USE_OSA__
		)
		message(STATUS "    TAL uses OSA")
		set(TAL_LINK_LIB ${TAL_LINK_LIB} ${OSA_LIBRARIES})
	else()
		add_definitions(-D__TAL_NOT_THREADSAFE__)
		message(STATUS "    TAL is not threadsafe")
	endif()



	# Common files for port framework and normal TAL
	include_directories(libraries/utils/code)
	
	set(HEADERS ${HEADERS}
		include/addr_alloc.h
		include/hash.h
		include/mmu.h
		include/pool.h
		include/ra.h
		include/tconf.h
        include/tconf_int.h
        include/tconf_txt.h
        include/tconf_xml.h
		include/trace.h
		
		libraries/utils/code/mmu_definition.h
	)
	
	set(SOURCES 
		libraries/target_config/code/tconf.c
        libraries/target_config/code/tconf_int.c
        libraries/target_config/code/tconf_txt.c
        libraries/target_config/code/tconf_xml.c
		libraries/target_config/code/target.c
	)

	if(TAL_CALLBACK_SUPPORT)
		set(HEADERS ${HEADERS}
			include/tal_callback.h
		)
	endif()

	if(${TAL_TYPE} MATCHES "portfwrk") 
		include(portfwrk)
	elseif(${TAL_TYPE} MATCHES "normal")
		include(normal)
	else()
	    message(FATAL_ERROR "TAL_TYPE=${TAL_TYPE} not supported!")	
	endif()
		
	#
	# common linking with normal and portbility framework
	#
	
	target_link_libraries(${TAL_LIBRARIES} ${IMGLIB_LISTUTILS_LIBRARIES})
	add_dependencies(${TAL_LIBRARIES} ${MXML_NAME}) # to insure MXML was built and installed before building the TAL
	
	if (NOT IMG_KERNEL_MODULE)
		target_link_libraries(${TAL_LIBRARIES} ${IMGLIB_GZIPIO_LIBRARIES} ${MXML_LIBRARIES})
	endif()
	
	if(${Threads_FOUND}) # should be found only in Linux
		target_link_libraries(${TAL_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
	endif()
	
	if (DEFINED TAL_LINK_LIB)
		target_link_libraries(${TAL_LIBRARIES} ${TAL_LINK_LIB})
	endif()
	
endif() # TAL light

if (GTEST_FOUND AND TAL_TESTS)

	add_subdirectory(test)

endif()
