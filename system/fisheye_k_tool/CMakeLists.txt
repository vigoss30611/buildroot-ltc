project(fisheye_k_tool)
cmake_minimum_required(VERSION 2.8)

include(FindPkgConfig)

set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -Werror -Wl,--as-needed")
include_directories(
			${fisheye_k_tool_SOURCE_DIR}/include
)

set(SRC_LIST
	 ${CMAKE_CURRENT_SOURCE_DIR}/src/fe_k_proc.cpp
	 ${CMAKE_CURRENT_SOURCE_DIR}/src/fe_k_misc.cpp
)

set(SRC_LIST ${SRC_LIST}
	${CMAKE_CURRENT_SOURCE_DIR}/view_mode/fisheye_2p360.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/view_mode/fisheye_downview.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/view_mode/fisheye_barrel.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/view_mode/fisheye_1p360.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/view_mode/fisheye_ceiling.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/view_mode/fisheye_floor.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/view_mode/fisheye_org.cpp
)

pkg_check_modules(LIBGRIDTOOLS REQUIRED libgridtools)
include_directories(${LIBGRIDTOOLS_INCLUDE_DIRS})
add_definitions(${LIBGRIDTOOLS_CFLAGS})
link_libraries(${LIBGRIDTOOLS_LIBRARIES})

pkg_check_modules(LIBVIDEOBOX REQUIRED videobox)
include_directories(${LIBVIDEOBOX_INCLUDE_DIRS})
link_libraries(${LIBVIDEOBOX_LIBRARIES})

# build fisheye_k_tool
add_executable(fisheye_k_tool main.cpp ${SRC_LIST})


# --> install
install(TARGETS fisheye_k_tool DESTINATION bin)
#install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/lib/libgridtools.so DESTINATION lib)


