cmake_minimum_required(VERSION 2.8)
include(FindPkgConfig)

set(CMAKE_VERBOSE_MAKEFILE on)
set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -Werror -Wl,--as-needed")

pkg_check_modules(LIBFR REQUIRED fr)
include_directories(${LIBFR_INCLUDE_DIRS})
link_libraries(${LIBFR_LIBRARIES})
add_definitions(${LIBFR_CFLAGS})

pkg_check_modules(LIBEVENT REQUIRED event)
include_directories(${LIBEVENT_INCLUDE_DIRS})
link_libraries(${LIBEVENT_LIBRARIES})
add_definitions(${LIBEVENT_CFLAGS})

pkg_check_modules(LIBAUDIOBOX REQUIRED audiobox)
include_directories(./ $LIBAUDIOBOX_INCLUDE_DIRS})
link_libraries(${LIBAUDIOBOX_LIBRARIES})

add_executable(demo_audiobox_002 demo_main.cpp)

install(TARGETS demo_audiobox_002 DESTINATION bin)
