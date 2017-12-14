project(app-ittd)
cmake_minimum_required(VERSION 2.8)

include(FindPkgConfig)

set(CMAKE_VERBOSE_MAKEFILE on)
#set(CMAKE_C_FLAGS "-std=c++11 -g -Wall")
set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -Wl,--as-needed")
include_directories(${app-ittd_SOURCE_DIR})
link_libraries(pthread rt m rtsp fr videobox audiobox codecs demux pthreadcc socketcc)

set(pthreadccfiles
	pthreadcc/threadexception.cpp
	pthreadcc/mutualexclusion.cpp
	pthreadcc/semaphore.cpp
	pthreadcc/condition.cpp
	pthreadcc/threadbase.cpp
)
add_definitions(-DPLATFORM_linux)
add_library(pthreadcc STATIC ${pthreadccfiles})
link_libraries(pthreadcc)

set(socketccfiles
	socketcc/socketexception.cpp
	socketcc/ipaddress.cpp
	socketcc/socketbase.cpp
	socketcc/tcpsockets.cpp
	socketcc/udpsockets.cpp
)
add_definitions(-DPLATFORM_linux)
include_directories(${app-ittd_SOURCE_DIR}/pthreadcc)
add_library(socketcc STATIC ${socketccfiles})
link_libraries(socketcc)

pkg_check_modules(LIBAUDIOBOX REQUIRED audiobox)
include_directories(${LIBAUDIOBOX_INCLUDE_DIRS})
add_definitions(${LIBAUDIOBOX_CFLAGS})
link_libraries(${LIBAUDIOBOX_LIBRARIES})

set(ipufiles ${ipufiles}
	main.cpp
	tcpserver.cpp
	deamon.cpp	
)

if (ITTD_V4L2_ONLY STREQUAL "yes")
    add_definitions(-DITTD_V4L2_ONLY)
endif()

# build app-ittd
message(STATUS "Build app-ittd ....")
include_directories(${app-ittd_SOURCE_DIR}/pthreadcc)
include_directories(${app-ittd_SOURCE_DIR}/socketcc)
add_executable(app-ittd ${ipufiles})

# --> install
install(TARGETS app-ittd DESTINATION bin)

