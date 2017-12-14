cmake_minimum_required(VERSION 2.8)

find_package(SimImage REQUIRED)
find_package(ImgIncludes REQUIRED)
find_package(FelixCommon REQUIRED)

set(LIB_LINKS) # empty
if(WIN32)
	#find_package(PThreadWin32 REQUIRED) # done in find SimImage
	include_directories(${PTHREADWIN32_INCLUDE_DIRS})
	add_definitions(${PTHREADWIN32_DEFINITIONS})
	set (LIB_LINKS ${LIB_LINKS} ${PTHREADWIN32_LIBRARIES})
elseif(ANDROID_BUILD)
    #set (LIB_LINKS) # no threads - define in FindSimImage
	#add_definitions(-DANDROID)
else()
    set (LIB_LINKS ${LIB_LINKS} -lpthread)
endif()

project (sim_image CXX)

include_directories(
    ${IMGINCLUDES_INCLUDE_DIRS}
    ${FELIXCOMMON_INCLUDE_DIRS}
)
add_definitions(
    ${IMGINCLUDES_DEFINITIONS}
    ${FELIXCOMMON_DEFINITIONS}
    ${SIMIMAGE_DEFINITIONS}
)

set (HEADERS image.h sim_image.h savefile.h)
set (SOURCES image.cpp sim_image.cpp savefile.c)

add_library(${SIMIMAGE_NAME} ${SOURCES} ${HEADERS})
target_link_libraries(${SIMIMAGE_NAME} ${FELIXCOMMON_LIBRARIES})
