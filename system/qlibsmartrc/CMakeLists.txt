project(qlibsmartrc)
cmake_minimum_required(VERSION 2.8)

include(FindPkgConfig)

set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -Werror -Wl,--as-needed")
include_directories(
			${qlibsmartrc_SOURCE_DIR}/include
			${qlibsmartrc_SOURCE_DIR}/lib
)

set(SRC_LIST
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/smartrc.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/rc_template.cpp
)


pkg_check_modules(LIBVIDEOBOX REQUIRED videobox)
include_directories(${LIBVIDEOBOX_INCLUDE_DIRS})
link_libraries(${LIBVIDEOBOX_LIBRARIES})
link_libraries(pthread rt m)

# build SmartRC 
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib)

add_executable(smartrc main.cpp ${SRC_LIST})


# --> install
install(TARGETS smartrc DESTINATION bin)


