cmake_minimum_required(VERSION 2.8)
include(FindPkgConfig)

set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -Werror -Wl,--as-needed")

pkg_check_modules(LIBEVENT REQUIRED event)
include_directories(${LIBEVENT_INCLUDE_DIRS})
link_libraries(${LIBEVENT_LIBRARIES})
add_definitions(${LIBEVENT_CFLAGS})

link_libraries(pthread)
pkg_check_modules(LIBVIDEOBOX REQUIRED videobox)
include_directories(./ $LIBVIDEOBOX_INCLUDE_DIRS})
link_libraries(${LIBVIDEOBOX_LIBRARIES})

add_executable(demo_camera_010 demo_main.cpp)

install(TARGETS demo_camera_010 DESTINATION bin)
