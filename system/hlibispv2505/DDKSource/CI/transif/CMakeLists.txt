project(transif)

if (NOT (TRANSIF_BUILD_SLAVE OR TRANSIF_BUILD_MASTER))
	message(FATAL_ERROR "You must set at least one of TRANSIF_BUILD_SLAVE or TRANSIF_BUILD_MASTER")
endif()

if (TRANSIF_BUILD_TESTER)
	set (TRANSIF_BUILD_SLAVE yes)
endif()

find_package(ImgIncludes REQUIRED)
	
include_directories(${IMGINCLUDES_INCLUDE_DIRS})

if (TRANSIF_BUILD_MASTER)
  find_package(OSA REQUIRED)
  find_package(DeviceInterfaceTransif REQUIRED)
  find_package(Transif REQUIRED)
  
  add_subdirectory(device_interface)
  
  include_directories(
	${OSA_INCLUDE_DIRS}  
	${DEVIF_TRANSIF_INCLUDE_DIRS}
	${TRANSIF_INCLUDE_DIRS}
  )
	
  set(SOURCES
    sim_wrapper_api.cpp
    transif_adaptor.cpp
    mem_models/system_mem_alloc.cpp
    mem_models/system_mem_prealloc.cpp
    mem_models/system_memory.cpp
    mem_models/system_mem_sim.cpp
  )
  
  set(HEADERS
    sim_wrapper_api.h
    transif_adaptor.h
    mem_models/system_memory.h
    mem_models/system_mem_alloc.h
    mem_models/system_mem_prealloc.h
    mem_models/system_mem_sim.h
  )
    
  add_library(transif_master ${SOURCES} ${HEADERS})
  target_link_libraries(transif_master ${OSA_LIBRARIES} ${DEVIF_TRANSIF_LIBRARIES})

  if (ANDROID_BUILD OR NOT ${CMAKE_SYSTEM_NAME} MATCHES "Linux")
        target_link_libraries(transif_master ${IMGLIB_THREADS_LIBRARIES})
  else()
        find_library(DL_LIB dl)
        target_link_libraries(transif_master ${DL_LIB} ${IMGLIB_THREADS_LIBRARIES} )
  endif()
endif()

if (TRANSIF_BUILD_SLAVE)
	find_package(Systemc REQUIRED)
	find_package(Systemc_support REQUIRED)

	set (sources 
		sim_wrapper_api_internal_imp.cpp
	)
	
	set (headers
		sim_clock.h
		sim_clock_defs.h
		sim_wrapper_api.h
		sim_wrapper_api_internal.h
		sim_wrapper_api_internal_imp.h
		transif_scsimif_adaptor.h
	)

	include_directories(${SYSTEMC_INCLUDE_DIRS} ${SYSTEMC_SUPPORT_INCLUDE_DIRS})
	if (TRANSIF_DLL_SUPPORT_PDUMPPLAYER)
		include_directories(
			${CMAKE_SOURCE_DIR}/rogue/
			${CMAKE_SOURCE_DIR}/rogue/interface
			${CMAKE_SOURCE_DIR}/rogue/client
			)
	endif()

	add_library(transif_slave ${sources} ${headers})
	
	if (TRANSIF_NO_PDUMP_PLAYER)
		set_property(TARGET transif_slave APPEND PROPERTY COMPILE_DEFINITIONS	"NO_PDUMP_PLAYER")
	endif()
endif()

if (TRANSIF_BUILD_TESTER)
	find_package(Systemc REQUIRED)
	find_package(Systemc_support REQUIRED)
	find_package(MeOSAL REQUIRED)
	find_package(ZLIB REQUIRED)
	
	include_directories(${SYSTEMC_INCLUDE_DIRS} ${SYSTEMC_SUPPORT_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/testApp)
  include_directories(${MEOSAL_INCLUDE_DIRS})
  include_directories(${ZLIB_INCLUDE_DIRS})
  
	add_library(testDLL SHARED EXCLUDE_FROM_ALL ${CMAKE_CURRENT_SOURCE_DIR}/testApp/dll_main.cpp)
  
	#test DLL does not have built in pdump player, if the transif_slave lib was build with integrated pdump, rebuilt lib without
	set_property(TARGET testDLL APPEND PROPERTY COMPILE_DEFINITIONS "NO_PDUMP_PLAYER")
  
	if (NOT TRANSIF_NO_PDUMP_PLAYER)
		add_library(transif_slave_no_pd ${sources} ${headers})
		set_property(TARGET transif_slave_no_pd  APPEND PROPERTY COMPILE_DEFINITIONS "NO_PDUMP_PLAYER")
		target_link_libraries(testDLL transif_slave_no_pd ${ZLIB_PATH} ${SYSTEMC_LIBRARIES} ${SYSTEMC_SUPPORT_LIBRARIES} ${MEOSAL_LIBRARIES} ${ZLIB_LIBRARIES})
	else()
		target_link_libraries(testDLL transif_slave ${ZLIB_PATH} ${SYSTEMC_LIBRARIES} ${SYSTEMC_SUPPORT_LIBRARIES} ${MEOSAL_LIBRARIES} ${ZLIB_LIBRARIES})
	endif()
  
  if (TRANSIF_TIMED_MODEL)
	  set_target_properties(testDLL	PROPERTIES OUTPUT_NAME TestLibTimed)
  else()
	  set_target_properties(testDLL	PROPERTIES OUTPUT_NAME TestLibUntimed)
  endif()

	IF(UNIX AND NOT APPLE)
		set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LIBRARY_LINK_C_FLAGS} -Wl,-Bsymbolic -Wl,--no-undefined")
	ENDIF()
endif ()
