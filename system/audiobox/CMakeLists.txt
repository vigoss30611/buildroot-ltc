project(audiobox)
cmake_minimum_required(VERSION 2.8)

include(FindPkgConfig)

set(CMAKE_VERBOSE_MAKEFILE on)
set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -Werror -Wl,--as-needed -lstdc++")
set(LIST_LIBNAME)
set(LIST_LIB "LIST_LIB-NOTFOUND")
if (COMPILE_AUDIOBOX_OPTIMIZATION STREQUAL "yes")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++")
endif()

if (COMPILE_AUDIOBOX_USE_AECV1 STREQUAL "yes")
link_libraries(dsp)
add_definitions(-DAUDIOBOX_USE_AECV1)
endif(COMPILE_AUDIOBOX_USE_AECV1)

if (COMPILE_AUDIOBOX_USE_AECV2 STREQUAL "yes")
link_libraries(dsp)
add_definitions(-DAUDIOBOX_USE_AECV2)
endif(COMPILE_AUDIOBOX_USE_AECV2)

include_directories(./include)

link_libraries(m)
link_libraries(pthread)
link_libraries(vcp7g)

function(search_slib path_slib slib_name)
    set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
    set(index 1)
    while (index LESS ${ARGC})
         message("ARGV = ${ARGV${index}}")
         set(input_slib_name ${ARGV${index}})
         find_library(static_lib_${input_slib_name}
             NAMES ${input_slib_name})
          if (static_lib_${input_slib_name})
              message("SUCESS:find static lib:${static_lib_${input_slib_name}}")
              set(result "${result}" "lib${input_slib_name}.a")
              #set(result "${result} ${static_lib_${input_slib_name}}")
              message("result:${result}")
          else (static_lib_${slib_name})
              message("WARNING:not found static lib${input_slib_name}.a,then using dynamic lib${input_slib_name}.so")
              set(result "${result}" "lib${input_slib_name}.so")
              message("result:${result}")
          endif ()
          math(EXPR index "${index} + 1")
    endwhile()
    set(${path_slib} ${result} PARENT_SCOPE)
    message("Debug: search_slib lib list-->${result}")
endfunction()

pkg_check_modules(LIBCODECS REQUIRED libcodecs)
include_directories(${LIBCODECS_INCLUDE_DIRS})
add_definitions(${LIBCODECS_CFLAGS})
set(LIST_LIBNAME ${LIST_LIBNAME} ${LIBCODECS_LIBRARIES})

pkg_check_modules(LIBFR REQUIRED fr)
include_directories(${LIBFR_INCLUDE_DIRS})
add_definitions(${LIBFR_CFLAGS})
set(LIST_LIBNAME ${LIST_LIBNAME} ${LIBFR_LIBRARIES})

pkg_check_modules(LIBEVENT REQUIRED event)
include_directories(${LIBEVENT_INCLUDE_DIRS})
add_definitions(${LIBEVENT_CFLAGS})
set(LIST_LIBNAME ${LIST_LIBNAME} ${LIBEVENT_LIBRARIES})

pkg_check_modules(LIBALSA REQUIRED alsa)
include_directories(${LIBALSA_INCLUDE_DIRS})
add_definitions(${LIBALSA_CFLAGS})
set(LIST_LIBNAME ${LIST_LIBNAME} ${LIBALSA_LIBRARIES})

#pkg_check_modules(LIBCODECS REQUIRED codecs)
#include_directories(${LIBCODECS_INCLUDE_DIRS})
#add_definitions(${LIBCODECS_CFLAGS})
#set(LIST_LIBNAME ${LIST_LIBNAME} ${LIBCODECS_LIBRARIES})


#build lib
add_library(audiobox_dynamic SHARED audio_streamops.c 
                            audio_rpc.c
                            audio_chn.c)
add_library(audiobox_static STATIC audio_streamops.c
                            audio_rpc.c
                            audio_chn.c)
set_target_properties(audiobox_dynamic PROPERTIES OUTPUT_NAME audiobox CLEAN_DIRECT_OUTPUT 1)
set_target_properties(audiobox_static PROPERTIES OUTPUT_NAME audiobox CLEAN_DIRECT_OUTPUT 1)

# build audiobox
add_executable(audiobox audiobox.c 
                        audiobox_hal.c
                        audiobox_listener.c
                        audiobox_ctl.c
                        audiobox_service.c
                        audio_rpc.c
                        audiobox_softvol.c
                        audio_streamops.c
                        audio_chn.c)
foreach(lib_var ${LIST_LIBNAME})
        message("link lib_var-->${lib_var}")
endforeach()

if (COMPILE_AUDIOBOX_OPTIMIZATION STREQUAL "yes")
    message("dynamic compile audiobox.............0")
    message("begin LIST_LIB:${LIST_LIB}")
    message("begin LIST_LIBNAME:${LIST_LIBNAME}")
    search_slib(LIST_LIB ${LIST_LIBNAME})
    foreach(lib_var ${LIST_LIB})
        message("link lib_var-->${lib_var}")
    endforeach()
    message("search_slib_result  result:${LIST_LIB}")
    foreach(lib_var ${LIST_LIB}) 
        message("link lib_var-->${lib_var}")
        target_link_libraries(audiobox ${lib_var})
    endforeach()
else()
    message("dynamic compile audiobox")
    target_link_libraries(audiobox ${LIST_LIBNAME})
endif()
message("dynamic compile audiobox.............1")
# --> install
install(TARGETS audiobox DESTINATION bin)
install(TARGETS audiobox_dynamic DESTINATION lib)
install(TARGETS audiobox_static DESTINATION lib)
