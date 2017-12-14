cmake_minimum_required(VERSION 2.8)

project(ISPCONTROL)

message(STATUS "== ISP Control ==")

add_subdirectory(ISPC_lib)

if (BUILD_TEST_APPS)
    add_subdirectory(test_apps)
    
    if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/ISPC_tcp")
        add_subdirectory(ISPC_tcp)
    endif()

    add_subdirectory(ISPC_tcp2)

    if(${CI_BUILD_KERNEL_MODULE})
      add_subdirectory(ISPC_hdr)
    endif()
    
endif()

