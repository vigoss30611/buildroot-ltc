cmake_minimum_required(VERSION 2.8)

project (ISPC_tcp3)

find_package(ImgIncludes REQUIRED)
find_package(ParamSocket2 REQUIRED)
find_package(ISPC2 REQUIRED)
find_package(DynCMD REQUIRED)

message(STATUS "Build ISPC_tcp v3")
set(EXE_NAME ISPC_tcp3)

if(WIN32)
  # win32 not supported at the moment but linking is ready in case we support it with simulator in the future
  find_package(PThreadWin32 REQUIRED)
  include_directories(${PTHREADWIN32_INCLUDE_DIRS})
  add_definitions(${PTHREADWIN32_DEFINITIONS})
  set(PTHREAD_LIBRARIES ${PTHREADWIN32_LIBRARIES})
  #set(PTHREAD_INSTALL ${PTHREADWIN32_INSTALL})
else()
  set(PTHREAD_LIBRARIES -lpthread)
endif()

include_directories(
	${IMGINCLUDES_INCLUDE_DIRS}
	${PARAMSOCKET2_INCLUDE_DIRS}
	${ISPC2_INCLUDE_DIRS}
	${DYNCMD_INCLUDE_DIRS}
	${ISPC2TEST_INCLUDE_DIRS} # will be empty if building real drivers
)

add_definitions(
	${IMGINLCUDES_DEFINITIONS}
	${PARAMSOCKET2_DEFINITIONS}
	${ISPC2_DEFINITIONS}
	${DYNCMD_DEFINITIONS}
	${ISPC2TEST_DEFINITIONS} # will be empty if building real drivers
)

set(SOURCE
  main.cpp
  ISPC_tcp_mk3.hpp
  ISPC_tcp_mk3.cpp
)

add_executable(${EXE_NAME} ${SOURCE})
target_link_libraries(${EXE_NAME} ${PARAMSOCKET2_LIBRARIES} ${ISPC2_LIBRARIES} ${PTHREAD_LIBRARIES} ${DYNCMD_LIBRARIES} ${ISPC2TEST_LIBRARIES})

if(CI_EXT_DATA_GENERATOR)
  add_definitions(-DEXT_DATAGEN)
endif()

if (DEFINED FELIXAPI_INSTALL) # this contains the pthread dll ISPC_tcp2 needs
  file(COPY ${FELIXAPI_INSTALL} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
  #install(FILES ${FELIXAPI_INSTALL} DESTINATION ./ISPC) # done in test_apps
endif()

if(NOT WIN32)
    # modify that when ISPC_tcp2 can use the simulator
    #install(TARGETS ${EXE_NAME} DESTINATION ./ISPC)
endif()