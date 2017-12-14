cmake_minimum_required(VERSION 2.8)
include(FindPkgConfig)

set(CMAKE_VERBOSE_MAKEFILE on)
set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -Werror -Wl,--as-needed")

link_libraries(pthread)

pkg_check_modules(LIBSYSTEM REQUIRED qlibsys)
include_directories(${LIBSYSTEM_INCLUDE_DIRS})
link_libraries(${LIBSYSTEM_LIBRARIES})
add_definitions(${LIBSYSTEM_CFLAGS})

pkg_check_modules(LIBEVENT REQUIRED event)
include_directories(${LIBEVENT_INCLUDE_DIRS})
link_libraries(${LIBEVENT_LIBRARIES})
add_definitions(${LIBEVENT_CFLAGS})

pkg_check_modules(LIBWIFI REQUIRED wifi)
include_directories(./ ${LIBWIFI_INCLUDE_DIRS})
link_libraries(${LIBWIFI_LIBRARIES})
add_definitions(${LIBWIFI_CFLAGS})

add_executable(demo_wi-fi_002 demo_main.c)

install(TARGETS demo_wi-fi_002 DESTINATION bin)
