project(qlibsensors)
cmake_minimum_required(VERSION 2.8)

include_directories(${qlibsensors_SOURCE_DIR} ${qlibsensors_SOURCE_DIR}/include)
link_libraries(pthread rt)

# --> files
if (COMPILE_LIGHT_SENSOR STREQUAL "yes")
set(qlsfiles ${qlsfiles}
	lu/lu.cpp
)
add_definitions(-DLUDEV="${LUDEV}")
include_directories(${qlibsensors_SOURCE_DIR}/lu)
endif()

if (COMPILE_GPS STREQUAL "yes")
set(qlsfiles ${qlsfiles}
	gps/gps.cpp gps/TinyGPS++.cpp
)
link_libraries(m)
add_definitions(-DARM)
add_definitions(-DGPSTTY="${GPSTTY}"
				-DGPSSPEED=B${GPSSPEED}
				-DGPSDATA=CS${GPSDATA}
				-DGPSSTOP=${GPSSTOP}
				)
if (GPSPARNONE STREQUAL "y")
add_definitions(-DGPSPARNONE)
endif()
if (GPSPAREVEN STREQUAL "y")
add_definitions(-DGPSPAREVEN)
endif()
if (GPSPARODD STREQUAL "y")
add_definitions(-DGPSPARODD)
endif()

include_directories(${qlibsensors_SOURCE_DIR}/gps)
endif()

add_library(sensors SHARED ${qlsfiles})
install(TARGETS sensors DESTINATION lib)

