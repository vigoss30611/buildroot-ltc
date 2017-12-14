project(deom_uvc002)
cmake_minimum_required(VERSION 2.8)

include(FindPkgConfig)

set(CMAKE_VERBOSE_MAKEFILE on)
set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -Werror -Wl,--as-needed")

pkg_check_modules(LIBVIDEOBOX REQUIRED videobox)
pkg_check_modules(HLIBGUVC REQUIRED hlibguvc)
include_directories(./ ${LIBVIDEOBOX_INCLUDE_DIRS})
include_directories(${HLIBGUVC_INCLUDE_DIRS})
link_libraries(${LIBVIDEOBOX_LIBRARIES})
link_libraries(${HLIBGUVC_LIBRARIES})
link_libraries(pthread)

add_executable(demo_uvc_001 demo_main.c)

install(TARGETS demo_uvc_001 DESTINATION bin)
