cmake_minimum_required(VERSION 2.8)

find_package(SimImage REQUIRED)
find_package(ImgIncludes REQUIRED)
find_package(FelixCommon REQUIRED) # logging

include_directories(
	${IMGINCLUDES_INCLUDE_DIRS}
	${FELIXCOMMON_INCLUDE_DIRS}
	${SIMIMAGE_INCLUDE_DIRS}
)
add_definitions(
	${IMGINCLUDES_DEFINITIONS}
	${FELIXCOMMON_DEFINITIONS}
	${SIMIMAGE_DEFINITIONS}
)

add_executable(flxconverter flxconverter.cpp)
target_link_libraries(flxconverter ${SIMIMAGE_LIBRARIES} ${FELIXCOMMON_LIBRARIES})

#install(TARGETS flxconverter DESTINATION ./utils/)
