cmake_minimum_required(VERSION 2.8)

find_package(ImgIncludes REQUIRED)
find_package(ISPC2 REQUIRED)
find_package(SimImage REQUIRED)
find_package(DynCMD REQUIRED)

include_directories(
  ${IMGINCLUDES_INCLUDE_DIRS}
  ${ISPC2_INCLUDE_DIRS}
  ${SIMIMAGE_INCLUDE_DIRS}
  ${DYNCMD_INCLUDE_DIRS}
  ${ISPC2TEST_INCLUDE_DIRS} # will be empty if building real drivers
)

add_definitions(
  ${IMGINCLUDES_DEFINITIONS}
  ${ISPC2_DEFINITIONS}
  ${SIMIMAGE_DEFINITIONS}
  ${DYNCMD_DEFINITIONS}
  ${ISPC2TEST_DEFINITIONS} # will be empty if building real drivers
)

set(LINK_LIBS ${ISPC2_LIBRARIES} ${ISPC2TEST_LIBRARIES} ${SIMIMAGE_LIBRARIES} ${DYNCMD_LIBRARIES})

if (${SYSMEM_DMABUF_IMPORT})
	find_package(DmaBuf REQUIRED)
	find_package(FelixCommon REQUIRED)
	
	message(STATUS "Test app can support DMA buffer allocator")
	
	include_directories(${DMABUF_INCLUDE_DIRS})# ${FELIXCOMMON_INCLUDE_DIRS})
	add_definitions(${DMABUF_DEFINITIONS} -DUSE_DMABUF=1)
	
	set(LINK_LIBS ${LINK_LIBS} ${DMABUF_LIBRARIES})# ${FELIXCOMMON_LIBRARIES})
endif()

if (CI_EXT_DATA_GENERATOR)
  add_definitions(-DEXT_DATAGEN)
endif()

if (NOT WIN32)
    message(STATUS "Build ISPC_loop")
    
    add_executable(ISPC_loop src/ISPC_loop.cpp)
    target_link_libraries(ISPC_loop ${LINK_LIBS})

    #install(TARGETS ISPC_loop DESTINATION ./ISPC)

    if(${CI_BUILD_KERNEL_MODULE}) # used only when using the real driver
      message(STATUS "Build ISPC_capture")
      add_executable(ISPC_capture src/ISPC_capture.cpp)
      target_link_libraries(ISPC_capture ${LINK_LIBS})
    
      #install(TARGETS ISPC_capture DESTINATION ./ISPC)
    endif()
     
     message(STATUS "Build ISPC_loop_tuning")
     add_executable(ISPC_loop_tuning src/ISPC_loop_tuning.cpp)
     target_link_libraries(ISPC_loop_tuning ${LINK_LIBS})
     #install(TARGETS ISPC_loop_tuning DESTINATION ./ISPC)

     message(STATUS "Build ISPC_loop_tuning_blc")
     add_executable(ISPC_loop_tuning_blc src/ISPC_loop_tuning_blc.cpp)
     target_link_libraries(ISPC_loop_tuning_blc ${LINK_LIBS})
     #install(TARGETS ISPC_loop_tuning_blc DESTINATION ./ISPC)
	
	# to test console raw-mode reading
	add_executable(rawmode_test src/rawmode_test.c)
	#install(TARGETS rawmode_test DESTINATION ./utils/)
endif()

message(STATUS "Build ISPC_test")
message(STATUS "    support ext-datagen ${CI_EXT_DATA_GENERATOR}")
if(NOT CI_EXT_DATA_GENERATOR)
    message("Warning: ISPC_test may not work without CI_EXT_DATA_GENERATOR on all HW version (needs internal DG)")
endif()

add_executable(ISPC_test src/ISPC_test.cpp)
target_link_libraries(ISPC_test ${LINK_LIBS})
  
#install(TARGETS ISPC_test DESTINATION ./ISPC)
if (DEFINED FELIXAPI_INSTALL)
    file(COPY ${FELIXAPI_INSTALL} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
    #install(FILES ${FELIXAPI_INSTALL} DESTINATION ./ISPC/)
endif()
