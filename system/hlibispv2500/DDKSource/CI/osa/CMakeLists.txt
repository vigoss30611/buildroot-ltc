cmake_minimum_required(VERSION 2.8)

project(OSA)

message(STATUS "OS Abstraction")

# ----------------------------------------------------------------------
# Find this library
# ----------------------------------------------------------------------
find_package(OSA REQUIRED)
include_directories(${OSA_INCLUDE_DIRS})
add_definitions(${OSA_DEFINITIONS})

# ----------------------------------------------------------------------
# External dependencies
# ----------------------------------------------------------------------
find_package(ImgIncludes REQUIRED)
include_directories(${IMGINCLUDES_INCLUDE_DIRS})
add_definitions(${IMGINCLUDES_DEFINITIONS})
	
find_package(ImgLib_ListUtils REQUIRED)
include_directories(${IMGLIB_LISTUTILS_INCLUDE_DIRS})
add_definitions(${IMGLIB_LISTUTILS_DEFINITIONS})

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	find_package(Threads REQUIRED)
	include_directories(${THREADS_INCLUDE_DIRS})
	add_definitions(${THREADS_DEFINITIONS})
endif()

# ----------------------------------------------------------------------
# List of sources
# ----------------------------------------------------------------------
set(HEADERS
	include/osa.h
	include/osa_utils.h 
        include/osa_rman.h 
        include/osa_idgen.h    
)

set(EXTRA
	FindOSA.cmake
)

if(${OSA_SYSTEMC})
	find_package(Systemc REQUIRED)
	include_directories(${SYSTEMC_INCLUDE_DIRS})
	add_definitions(${SYSTEMC_DEFINITIONS})
	
	set(SOURCES ${SOURCES} src/sysc/syscosa.cpp)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	set(SOURCES ${SOURCES} src/darwin/osa.c)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	set(SOURCES ${SOURCES} src/linux/linosa.c)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	set(SOURCES ${SOURCES} src/win32/winosa.c)
endif()

set(SOURCES ${SOURCES}
	src/utils/osa_utils.c
	src/utils/osa_rman.c
	src/utils/osa_idgen.c
)

# ----------------------------------------------------------------------
# Add test application
# ----------------------------------------------------------------------
if(OSA_TESTS)
	add_subdirectory(test)
endif()

# ----------------------------------------------------------------------
# Build the library
# ----------------------------------------------------------------------
add_library(OSA STATIC ${SOURCES} ${HEADERS})
#target_link_libraries(OSA ${IMG_LIBRARIES})

target_link_libraries(OSA ${IMGLIB_LISTUTILS_LIBRARIES})

if(${OSA_SYSTEMC})
	target_link_libraries(OSA ${SYSTEMC_LIBRARIES})
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	add_definitions(-D_XOPEN_SOURCE=500)
    target_link_libraries(OSA ${CMAKE_THREAD_LIBS_INIT} ${SYS_LIBS})
endif()
