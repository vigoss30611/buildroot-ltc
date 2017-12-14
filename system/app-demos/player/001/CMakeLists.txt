cmake_minimum_required(VERSION 2.8)

include(FindPkgConfig)

set(CMAKE_VERBOSE_MAKEFILE on)
set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -Werror -Wl,--as-needed")

pkg_check_modules(LIBQVPLAY REQUIRED qvplay)
pkg_check_modules(LIBVIDEOBOX REQUIRED videobox)

include_directories(./ $LIBQVPLAY_INCLUDE_DIRS})
include_directories($LIBVIDEOBOX_INCLUDE_DIRS})
include_directories(../common/)

link_libraries(${LIBQVPLAY_LIBRARIES})
link_libraries(${LIBVIDEOBOX_LIBRARIES})

add_executable(demo_player_001 demo_main.cpp ../common/player_common.cpp)

install(TARGETS demo_player_001 DESTINATION bin)
