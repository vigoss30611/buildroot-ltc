project(videobox)
cmake_minimum_required(VERSION 2.8)

include(FindPkgConfig)

set(CMAKE_VERBOSE_MAKEFILE on)
set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -Werror -Wl,--as-needed")
set(LIST_LIBNAME)
set(LIST_LIB "LIST_LIB-NOTFOUND")
if (COMPILE_VIDEOBOX_OPTIMIZATION STREQUAL "yes")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++")
endif()
include_directories(
			${videobox_SOURCE_DIR}/Include
			${videobox_SOURCE_DIR}/lib/include)
set(LIST_LIBNAME ${LIST_LIBNAME} m)
set(LIST_LIBNAME ${LIST_LIBNAME} rt)
link_libraries(pthread)
# --> core files
set(corefiles
	Core/System.cpp
	Core/FRBuffer.cpp
	Core/Path.cpp
	Core/IPU.cpp
	Core/Port.cpp
    Core/Json.cpp
)

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

add_library(Core STATIC ${corefiles})
link_libraries(Core)

pkg_check_modules(LIBITEMS REQUIRED items)
include_directories($LIBITEMS_INCLUDE_DIRS})
add_definitions(${LIBITEMS_CFLAGS})
set(LIST_LIBNAME ${LIST_LIBNAME} ${LIBITEMS_LIBRARIES})
message("LIST_LIBNAME:${LIST_LIBNAME}")

pkg_check_modules(LIBFR REQUIRED fr)
include_directories(${LIBFR_INCLUDE_DIRS})
add_definitions(${LIBFR_CFLAGS})
set(LIST_LIBNAME ${LIST_LIBNAME} ${LIBFR_LIBRARIES})

pkg_check_modules(LIBEVENT REQUIRED event)
include_directories(${LIBEVENT_INCLUDE_DIRS})
add_definitions(${LIBEVENT_CFLAGS})
set(LIST_LIBNAME ${LIST_LIBNAME} ${LIBEVENT_LIBRARIES})

pkg_check_modules(QLIBSYS REQUIRED qlibsys)
include_directories(${QLIBSYSINCLUDE_DIRS})
add_definitions(${QLIBSYS_CFLAGS})
set(LIST_LIBNAME ${LIST_LIBNAME} ${QLIBSYS_LIBRARIES})

# --> IPU files
# IPU DGFrame
if (COMPILE_IPU_DGFRAME STREQUAL "yes")
add_definitions(-DCOMPILE_IPU_DGFRAME)
set(ipufiles ${ipufiles}
	IPU/DGFrame/DGFrame.cpp
)
include_directories(${videobox_SOURCE_DIR}/IPU/DGFrame)
endif()

# IPU DGPixel
if (COMPILE_IPU_DGPIXEL STREQUAL "yes")
add_definitions(-DCOMPILE_IPU_DGPIXEL)
set(ipufiles ${ipufiles}
	IPU/DGPixel/DGPixel.cpp
	IPU/DGPixel/font_7x14.cpp
)
include_directories(${videobox_SOURCE_DIR}/IPU/DGPixel)
endif()

# IPU DGMath
if (COMPILE_IPU_DGMATH STREQUAL "yes")
add_definitions(-DCOMPILE_IPU_DGMATH)
set(ipufiles ${ipufiles}
	IPU/DGMath/DGMath.cpp
)
include_directories(${videobox_SOURCE_DIR}/IPU/DGMath)
set(LIST_LIBNAME ${LIST_LIBNAME} m)
endif()

# IPU DGOV
if (COMPILE_IPU_DGOV STREQUAL "yes")
add_definitions(-DCOMPILE_IPU_DGOV)
set(ipufiles ${ipufiles}
	IPU/DGOV/DGOV.cpp
)
include_directories(${videobox_SOURCE_DIR}/IPU/DGOV)
set(LIST_LIBNAME ${LIST_LIBNAME} m)
endif()

# IPU Marker
if (COMPILE_IPU_MARKER STREQUAL "yes")
add_definitions(-DCOMPILE_IPU_MARKER)
set(LIST_LIBNAME ${LIST_LIBNAME} iconv)
set(ipufiles ${ipufiles}
	IPU/Marker/Marker.cpp
)
include_directories(${videobox_SOURCE_DIR}/IPU/Marker)
endif()

if (COMPILE_LIB_FREETYPE STREQUAL "yes")
add_definitions(-DCOMPILE_LIB_FREETYPE)
pkg_check_modules(FREETYPE REQUIRED freetype2)
include_directories(${FREETYPE_INCLUDE_DIRS})
add_definitions(${FREETYPE_CFLAGS})
set(LIST_LIBNAME ${LIST_LIBNAME} ${FREETYPE_LIBRARIES})
endif()


# IPU SoftLayer
if (COMPILE_IPU_SOFTLAYER STREQUAL "yes")
add_definitions(-DCOMPILE_IPU_SOFTLAYER)
set(ipufiles ${ipufiles}
	IPU/SoftLayer/SoftLayer.cpp
)
include_directories(${videobox_SOURCE_DIR}/IPU/SoftLayer)
endif()


# IPU IDS
if (COMPILE_IPU_IDS STREQUAL "yes")
add_definitions(-DCOMPILE_IPU_IDS)
set(ipufiles ${ipufiles}
	IPU/IDS/IDS.cpp
)
include_directories(${videobox_SOURCE_DIR}/IPU/IDS)
endif()

# IPU IDS2
if (COMPILE_IPU_IDS2 STREQUAL "yes")
add_definitions(-DCOMPILE_IPU_IDS2)
set(ipufiles ${ipufiles}
	IPU/IDS2/IDS.cpp
)
include_directories(${videobox_SOURCE_DIR}/IPU/IDS2)
endif()

# IPU FileSink
if (COMPILE_IPU_FILESINK STREQUAL "yes")
add_definitions(-DCOMPILE_IPU_FILESINK)
set(ipufiles ${ipufiles}
	IPU/FileSink/FileSink.cpp
)
include_directories(${videobox_SOURCE_DIR}/IPU/FileSink)
endif()

# IPU FileSource
if (COMPILE_IPU_FILESOURCE STREQUAL "yes")
add_definitions(-DCOMPILE_IPU_FILESOURCE)
set(ipufiles ${ipufiles}
	IPU/FileSource/FileSource.cpp
)
include_directories(${videobox_SOURCE_DIR}/IPU/FileSource)
endif()

# IPU H1JPEG
if (COMPILE_IPU_H1JPEG STREQUAL "yes")
    add_definitions(-DCOMPILE_IPU_H1JPEG)
    pkg_check_modules(LIBH1 REQUIRED hlibh1v6)
    include_directories(${LIBH1_INCLUDE_DIRS})
    add_definitions(${LIBH1_CFLAGS})
    set(LIST_LIBNAME ${LIST_LIBNAME} ${LIBH1_LIBRARIES})
    set(ipufiles ${ipufiles}
        IPU/H1JPEG/H1JPEG.cpp
        IPU/H1JPEG/H1JPEGUnitControl.cpp
        )
    include_directories(${videobox_SOURCE_DIR}/IPU/H1JPEG)
    if(COMPILE_H1JPEG_SWSCALE STREQUAL "yes")
        message(WARNING "---->H1JPEG SWSCALE ON")
        add_definitions(-DMACRO_H1JPEG_SWSCALE)
        pkg_check_modules(LIBMEDIAUTILS REQUIRED qlibmediautils)
        include_directories(${LIBMEDIAUTILS_INCLUDE_DIRS})
        add_definitions(${LIBMEDIAUTILS_CFLAGS})
        set(LIST_LIBNAME ${LIST_LIBNAME} ${LIBMEDIAUTILS_LIBRARIES})
    else()
        message(WARNING "---->H1JPEG SWSCALE OFF")
    endif()
    if(COMPILE_H1JPEG_CROP STREQUAL "yes")
        message(WARNING "---->H1JPEG CROP ON")
        add_definitions(-DMACRO_H1JPEG_CROP)
        pkg_check_modules(LIBMEDIAUTILS REQUIRED qlibmediautils)
        include_directories(${LIBMEDIAUTILS_INCLUDE_DIRS})
        add_definitions(${LIBMEDIAUTILS_CFLAGS})
        set(LIST_LIBNAME ${LIST_LIBNAME} ${LIBMEDIAUTILS_LIBRARIES})
    else()
        message(WARNING "---->H1JPEG CROP OFF")
    endif()
endif()

# IPU H1264
if (COMPILE_IPU_H1264 STREQUAL "yes")
add_definitions(-DCOMPILE_IPU_H1264)
pkg_check_modules(LIBH1 REQUIRED hlibh1v6)
include_directories(${LIBH1_INCLUDE_DIRS})
add_definitions(${LIBH1_CFLAGS})
set(LIST_LIBNAME ${LIST_LIBNAME} ${LIBH1_LIBRARIES})
set(ipufiles ${ipufiles}
	IPU/H1264/H1264.cpp IPU/H1264/H1264UnitControl.cpp
)
include_directories(${videobox_SOURCE_DIR}/IPU/H1264)
endif()

# IPU JENC
if (COMPILE_IPU_JENC STREQUAL "yes")
    add_definitions(-DCOMPILE_IPU_JENC)
    pkg_check_modules(LIBJENC REQUIRED hlibjenc)
    include_directories(${LIBJENC_INCLUDE_DIRS})
    add_definitions(${LIBJENC_CFLAGS})
    set(LIST_LIBNAME ${LIST_LIBNAME} ${LIBJENC_LIBRARIES})
    set(ipufiles ${ipufiles}
        IPU/Jenc/Jenc.cpp
        IPU/Jenc/JencUnitControl.cpp
    )
    include_directories(${videobox_SOURCE_DIR}/IPU/Jenc)
    if(COMPILE_JENC_SWSCALE STREQUAL "yes")
        message(WARNING "---->Jenc SWSCALE ON")
        add_definitions(-DMACRO_JENC_SWSCALE)
        pkg_check_modules(LIBMEDIAUTILS REQUIRED qlibmediautils)
        include_directories(${LIBMEDIAUTILS_INCLUDE_DIRS})
        add_definitions(${LIBMEDIAUTILS_CFLAGS})
        set(LIST_LIBNAME ${LIST_LIBNAME} ${LIBMEDIAUTILS_LIBRARIES})
    else()
        message(WARNING "---->Jenc SWSCALE OFF")
    endif()
endif()

# IPU H2
if (COMPILE_IPU_H2 STREQUAL "yes")
add_definitions(-DCOMPILE_IPU_H2)
pkg_check_modules(LIBH2 REQUIRED hlibh2v1)
include_directories(${LIBH2_INCLUDE_DIRS})
add_definitions(${LIBH2_CFLAGS})
set(LIST_LIBNAME ${LIST_LIBNAME} ${LIBH2_LIBRARIES})
set(ipufiles ${ipufiles}
	IPU/H2/H2.cpp IPU/H2/H2UnitControl.cpp
)
include_directories(${videobox_SOURCE_DIR}/IPU/H2)
endif()

# IPU H2V4
if (COMPILE_IPU_H2V4 STREQUAL "yes")
add_definitions(-DCOMPILE_IPU_H2V4)
pkg_check_modules(LIBH2V4 REQUIRED hlibh2v4)
include_directories(${LIBH2V4_INCLUDE_DIRS})
add_definitions(${LIBH2V4_CFLAGS})
set(LIST_LIBNAME ${LIST_LIBNAME} ${LIBH2V4_LIBRARIES})
set(ipufiles ${ipufiles}
	IPU/H2V4/H2.cpp IPU/H2V4/H2UnitControl.cpp
)
include_directories(${videobox_SOURCE_DIR}/IPU/H2V4)
endif()

#IPU VENCODER
if (COMPILE_IPU_VENCODER STREQUAL "yes")
    add_definitions(-DCOMPILE_IPU_VENCODER)
    if (VENCODER_DEPEND_H1V6 STREQUAL "yes")
        pkg_check_modules(LIBH1 REQUIRED hlibh1v6)
        include_directories(${LIBH1_INCLUDE_DIRS})
        add_definitions(-DVENCODER_DEPEND_H1V6 ${LIBH1_CFLAGS})
        set(LIST_LIBNAME ${LIST_LIBNAME} ${LIBH1_LIBRARIES})
    endif()
    if (VENCODER_DEPEND_H2V1 STREQUAL "yes")
        pkg_check_modules(LIBH2 REQUIRED hlibh2v1)
        include_directories(${LIBH2_INCLUDE_DIRS})
        add_definitions(-DVENCODER_DEPEND_H2V1 ${LIBH2_CFLAGS})
        set(LIST_LIBNAME ${LIST_LIBNAME} ${LIBH2_LIBRARIES})
    endif()
    if (VENCODER_DEPEND_H2V4 STREQUAL "yes")
        pkg_check_modules(LIBH2V4 REQUIRED hlibh2v4)
        include_directories(${LIBH2V4_INCLUDE_DIRS})
        add_definitions(-DVENCODER_DEPEND_H2V4 ${LIBH2V4_CFLAGS})
        set(LIST_LIBNAME ${LIST_LIBNAME} ${LIBH2V4_LIBRARIES})
    endif()
    set(ipufiles ${ipufiles}
        IPU/VENCODER/VEncoder.cpp
        IPU/VENCODER/VEncoderUnitControl.cpp
        IPU/VENCODER/VEncoderWrapper.cpp
        )
    include_directories(${videobox_SOURCE_DIR}/IPU/VENCODER)
endif()

# IPU G2
if (COMPILE_IPU_G2 STREQUAL "yes")
add_definitions(-DCOMPILE_IPU_G2)
pkg_check_modules(LIBG2 REQUIRED hlibg2v1)
include_directories(${LIBG2_INCLUDE_DIRS})
add_definitions(${LIBG2_CFLAGS})
add_definitions(-DDOWN_SCALER)
set(LIST_LIBNAME ${LIST_LIBNAME} ${LIBG2_LIBRARIES})
set(ipufiles ${ipufiles}
	IPU/G2/G2.cpp IPU/G2/G2UnitControl.cpp
)
include_directories(${videobox_SOURCE_DIR}/IPU/G2)
endif()

# IPU PP
if (COMPILE_IPU_PP STREQUAL "yes")
add_definitions(-DCOMPILE_IPU_PP)
pkg_check_modules(LIBG1 REQUIRED hlibg1v6)
include_directories(${LIBG1_INCLUDE_DIRS})
add_definitions(${LIBG1_CFLAGS})
set(LIST_LIBNAME ${LIST_LIBNAME} ${LIBG1_LIBRARIES})
set(ipufiles ${ipufiles}
	IPU/PP/PP.cpp IPU/PP/PPUnitControl.cpp
)
include_directories(${videobox_SOURCE_DIR}/IPU/PP)
endif()

# IPU FFVDEC
if (COMPILE_IPU_FFVDEC STREQUAL "yes")
add_definitions(-DCOMPILE_IPU_FFVDEC)
pkg_check_modules(LIBAVCODEC REQUIRED libavcodec libavutil)
include_directories(${LIBAVCODEC_INCLUDE_DIRS})
add_definitions(${LIBAVCODEC_CFLAGS})
set(LIST_LIBNAME ${LIST_LIBNAME} ${LIBAVCODEC_LIBRARIES})
set(LIST_LIBNAME ${LIST_LIBNAME} ${LIBAVUTIL_LIBRARIES})
set(ipufiles ${ipufiles}
	IPU/FFVDEC/FFVDEC.cpp IPU/FFVDEC/FFVDECUnitControl.cpp
)
include_directories(${videobox_SOURCE_DIR}/IPU/FFVDEC)
endif()

# IPU V2500
if (COMPILE_IPU_V2500 STREQUAL "yes")
    add_definitions(-DCOMPILE_IPU_V2500)
    pkg_check_modules(LIBV2500ISPC REQUIRED hlibispv2500)
    include_directories(${LIBV2500ISPC_INCLUDE_DIRS})
    add_definitions(${LIBV2500ISPC_CFLAGS})
    set(LIST_LIBNAME ${LIST_LIBNAME} ${LIBV2500ISPC_LIBRARIES})
    #link hlibcamsensor
    pkg_check_modules(LIBCAMSENSOR REQUIRED hlibcamsensor)
    include_directories(${LIBCAMSENSOR_INCLUDE_DIRS})
    add_definitions(${LIBCAMSENSOR_CFLAGS})
    set(LIST_LIBNAME ${LIST_LIBNAME} ${LIBCAMSENSOR_LIBRARIES})

    if (COMPILE_IQ_TUNING_TOOL_APIS STREQUAL "yes")
        message(STATUS "COMPILE_IQ_TUNING_TOOL_APIS on")
        add_definitions(-DCOMPILE_IQ_TUNING_TOOL_APIS)
        set(ipufiles ${ipufiles}
            IPU/V2500/V2500.cpp
            IPU/V2500/V2500UnitControl.cpp
            IPU/V2500/V2500IqTuning.cpp
        )
    else ()
        message(STATUS "COMPILE_IQ_TUNING_TOOL_APIS off")
        set(ipufiles ${ipufiles}
        IPU/V2500/V2500.cpp
            IPU/V2500/V2500UnitControl.cpp
        )
    endif()
    if (COMPILE_IPU_V2500_IQ_REALTIME_SAVE_RAW STREQUAL "yes")
        add_definitions(-DCOMPILE_IQ_REALTIME_SAVE_RAW)
    endif()
    include_directories(${videobox_SOURCE_DIR}/IPU/V2500)
endif()

# IPU V2505
if (COMPILE_IPU_V2505 STREQUAL "yes")
    add_definitions(-DCOMPILE_IPU_V2505)
    pkg_check_modules(LIBV2505ISPC REQUIRED hlibispv2505)
    include_directories(${LIBV2505ISPC_INCLUDE_DIRS})
    add_definitions(${LIBV2505ISPC_CFLAGS})
    set(LIST_LIBNAME ${LIST_LIBNAME} ${LIBV2505ISPC_LIBRARIES})
    #link hlibcamsensor
    pkg_check_modules(LIBCAMSENSOR REQUIRED hlibcamsensor)
    include_directories(${LIBCAMSENSOR_INCLUDE_DIRS})
    add_definitions(${LIBCAMSENSOR_CFLAGS})
    set(LIST_LIBNAME ${LIST_LIBNAME} ${LIBCAMSENSOR_LIBRARIES})

    if (COMPILE_IQ_TUNING_TOOL_APIS STREQUAL "yes")
        message(STATUS "COMPILE_IQ_TUNING_TOOL_APIS on")
        add_definitions(-DCOMPILE_IQ_TUNING_TOOL_APIS)
        set(ipufiles ${ipufiles}
            IPU/V2505/V2505.cpp
            IPU/V2505/V2505UnitControl.cpp
            IPU/V2505/V2505IqTuning.cpp
        )
    else ()
        message(STATUS "COMPILE_IQ_TUNING_TOOL_APIS off")
        set(ipufiles ${ipufiles}
            IPU/V2505/V2505.cpp
            IPU/V2505/V2505UnitControl.cpp
        )
    endif()
    if (COMPILE_IPU_V2505_IQ_REALTIME_SAVE_RAW STREQUAL "yes")
        add_definitions(-DCOMPILE_IQ_REALTIME_SAVE_RAW)
    endif()
    if (COMPILE_IPU_V2505_AWB_ALG_PLANCKIAN STREQUAL "yes")
        add_definitions(-DAWB_ALG_PLANCKIAN)
    endif()
    include_directories(${videobox_SOURCE_DIR}/IPU/V2505)
endif()

if (INFOTM_V2505_HW_AWB STREQUAL "yes")
    add_definitions(-DINFOTM_HW_AWB)
endif()

if (INFOTM_V2505_BLEND_8STEP_ALPHA STREQUAL "yes")
    add_definitions(-DINFOTM_BLEND_8STEP_ALPHA)
endif()

if (INFOTM_V2505_HW_BLENDING STREQUAL "yes")
    add_definitions(-DINFOTM_HW_BLEND)
endif()

# IPU ISP
if (COMPILE_IPU_ISP STREQUAL "yes")
    add_definitions(-DCOMPILE_IPU_ISP)
    
    if (COMPILE_ISP_V2500 STREQUAL "yes")
    		add_definitions(-DCOMPILE_ISP_V2500)
        pkg_check_modules(LIBV2500ISPC REQUIRED hlibispv2500)
    		include_directories(${LIBV2500ISPC_INCLUDE_DIRS})
    		add_definitions(${LIBV2500ISPC_CFLAGS})
    		set(LIST_LIBNAME ${LIST_LIBNAME} ${LIBV2500ISPC_LIBRARIES})
    endif()
    
    if (COMPILE_ISP_V2505 STREQUAL "yes")
    		add_definitions(-DCOMPILE_ISP_V2505)
   			pkg_check_modules(LIBV2505ISPC REQUIRED hlibispv2505)
   			include_directories(${LIBV2505ISPC_INCLUDE_DIRS})
   			add_definitions(${LIBV2505ISPC_CFLAGS})
   			set(LIST_LIBNAME ${LIST_LIBNAME} ${LIBV2505ISPC_LIBRARIES})
        if (INFOTM_V2505_BLEND_8STEP_ALPHA STREQUAL "yes")
            add_definitions(-DINFOTM_BLEND_8STEP_ALPHA)
        endif()
        if (COMPILE_IPU_V2505_AWB_ALG_PLANCKIAN STREQUAL "yes")
            add_definitions(-DAWB_ALG_PLANCKIAN)
        endif()
        if (INFOTM_V2505_HW_BLENDING STREQUAL "yes")
            add_definitions(-DINFOTM_HW_BLEND)
        endif()
   	endif()
   	
    #link hlibcamsensor
    pkg_check_modules(LIBCAMSENSOR REQUIRED hlibcamsensor)
    include_directories(${LIBCAMSENSOR_INCLUDE_DIRS})
    add_definitions(${LIBCAMSENSOR_CFLAGS})
    set(LIST_LIBNAME ${LIST_LIBNAME} ${LIBCAMSENSOR_LIBRARIES})

    if (COMPILE_IQ_TUNING_TOOL_APIS STREQUAL "yes")
        message(STATUS "COMPILE_IQ_TUNING_TOOL_APIS on")
        add_definitions(-DCOMPILE_IQ_TUNING_TOOL_APIS)
        if (COMPILE_ISP_V2500 STREQUAL "yes")
            set(ipufiles ${ipufiles}
                IPU/ISP/ISP.cpp
                IPU/ISP/ISPWrapper.cpp
                IPU/ISP/ISPUnitControl.cpp
                IPU/ISP/V2500IqTuning.cpp
            )
        elseif (COMPILE_ISP_V2505 STREQUAL "yes")
                set(ipufiles ${ipufiles}
                    IPU/ISP/ISP.cpp
                    IPU/ISP/ISPWrapper.cpp
                    IPU/ISP/ISPUnitControl.cpp
                    IPU/ISP/V2505IqTuning.cpp
                )
       endif()   
    else ()
        message(STATUS "COMPILE_IQ_TUNING_TOOL_APIS off")
        set(ipufiles ${ipufiles}
            IPU/ISP/ISP.cpp
            IPU/ISP/ISPWrapper.cpp
            IPU/ISP/ISPUnitControl.cpp
        )
    endif()
    if (COMPILE_IPU_ISP_IQ_REALTIME_SAVE_RAW STREQUAL "yes")
    		message(STATUS "COMPILE_IPU_ISP_IQ_REALTIME_SAVE_RAW on")
        add_definitions(-DCOMPILE_IQ_REALTIME_SAVE_RAW)
    endif()
    include_directories(${videobox_SOURCE_DIR}/IPU/ISP)
endif()


# IPU V4L2
if (COMPILE_IPU_V4L2 STREQUAL "yes")
add_definitions(-DCOMPILE_IPU_V4L2)
set(ipufiles ${ipufiles}
	IPU/V4L2/V4L2.cpp
)
include_directories(${videobox_SOURCE_DIR}/IPU/V4L2)
endif()

# IPU ISPOSTv1
if (COMPILE_IPU_ISPOSTV1 STREQUAL "yes")
    add_definitions(-DCOMPILE_IPU_ISPOSTV1)
    pkg_check_modules(LIBISPOST REQUIRED hlibispostv1)
    include_directories(${LIBISPOST})
    add_definitions(${LIBISPOST_CFLAGS})
    set(LIST_LIBNAME ${LIST_LIBNAME} ${LIBISPOST_LIBRARIES})
    include_directories(${videobox_SOURCE_DIR}/IPU/ISPOSTv1/lib/include)
    link_directories(${videobox_SOURCE_DIR}/IPU/ISPOSTv1/lib)
    if (COMPILE_ISPOSTV1_IQ_TUNING_TOOL_APIS STREQUAL "yes")
        message(STATUS "COMPILE_ISPOSTV1_IQ_TUNING_TOOL_APIS on")
        add_definitions(-DCOMPILE_ISPOSTV1_IQ_TUNING_TOOL_APIS)
        set(ipufiles ${ipufiles}
            IPU/ISPOSTv1/ISPOSTv1.cpp 
            IPU/ISPOSTv1/ISPOSTv1UnitControl.cpp
            IPU/ISPOSTv1/ISPOSTv1IqTuning.cpp
            IPU/ISPOSTv2/q3denoise.cpp

            IPU/V2500/V2500.cpp
            IPU/V2500/V2500UnitControl.cpp
            IPU/V2500/V2500IqTuning.cpp
        )
    else()
        message(STATUS "COMPILE_ISPOSTV1_IQ_TUNING_TOOL_APIS off")
        set(ipufiles ${ipufiles}
            IPU/ISPOSTv1/ISPOSTv1.cpp IPU/ISPOSTv1/ISPOSTv1UnitControl.cpp
            IPU/ISPOSTv2/q3denoise.cpp
    )
    endif()
    include_directories(${videobox_SOURCE_DIR}/IPU/ISPOSTv1)
    if (COMPILE_IPU_ISPOSTV1_FCE STREQUAL "yes")
        message(STATUS "enable ispostv1  fce")
        message(STATUS ${COMPILE_IPU_ISPOSTV1_FCE_PARA})
        add_definitions(-DMACRO_IPU_ISPOSTV1_FCE)
        add_definitions(-DMACRO_IPU_ISPOSTV1_FCE_PARA="${COMPILE_IPU_ISPOSTV1_FCE_PARA}")
        #link libgridtools
        pkg_check_modules(LIBGRIDTOOLS REQUIRED libgridtools)
        include_directories(${LIBGRIDTOOLS_INCLUDE_DIRS})
        add_definitions(${LIBGRIDTOOLS_CFLAGS})
        set(LIST_LIBNAME ${LIST_LIBNAME} ${LIBGRIDTOOLS_LIBRARIES})
    else ()
        message(STATUS "disable ispostv1 fce")
    endif()
endif()

# IPU ISPOSTv2
if (COMPILE_IPU_ISPOSTV2 STREQUAL "yes")
    add_definitions(-DCOMPILE_IPU_ISPOSTV2)
    pkg_check_modules(LIBISPOSTV2 REQUIRED hlibispostv2)
    pkg_check_modules(LIBMNEON REQUIRED libmneon)
    include_directories(${LIBISPOSTV2})
    add_definitions(${LIBISPOSTV2_CFLAGS})
    set(LIST_LIBNAME ${LIST_LIBNAME} ${LIBISPOSTV2_LIBRARIES})
    set(LIST_LIBNAME ${LIST_LIBNAME} mneon)
    include_directories(${videobox_SOURCE_DIR}/IPU/ISPOSTv2/lib/include)
    link_directories(${videobox_SOURCE_DIR}/IPU/ISPOSTv2/lib)
    if (COMPILE_ISPOSTV2_IQ_TUNING_TOOL_APIS STREQUAL "yes")
        message(STATUS "COMPILE_ISPOSTV2_IQ_TUNING_TOOL_APIS on")
        add_definitions(-DCOMPILE_ISPOSTV2_IQ_TUNING_TOOL_APIS)
        set(ipufiles ${ipufiles}
            IPU/ISPOSTv2/ISPOSTv2.cpp 
            IPU/ISPOSTv2/ISPOSTv2UnitControl.cpp
            IPU/ISPOSTv2/ISPOSTv2IqTuning.cpp
            IPU/ISPOSTv2/q3denoise.cpp
        )
    else ()
        message(STATUS "COMPILE_ISPOSTV2_IQ_TUNING_TOOL_APIS off")
        set(ipufiles ${ipufiles}
            IPU/ISPOSTv2/ISPOSTv2.cpp 
            IPU/ISPOSTv2/ISPOSTv2UnitControl.cpp
            IPU/ISPOSTv2/q3denoise.cpp
        )
    endif()
    include_directories(${videobox_SOURCE_DIR}/IPU/ISPOSTv2)
    if (COMPILE_IPU_ISPOSTV2_FCE STREQUAL "yes")
        message(STATUS "enable ispostv2  fce")
        message(STATUS ${COMPILE_IPU_ISPOSTV2_FCE_PARA})
        add_definitions(-DMACRO_IPU_ISPOSTV2_FCE)
        add_definitions(-DMACRO_IPU_ISPOSTV2_FCE_PARA="${COMPILE_IPU_ISPOSTV2_FCE_PARA}")
        #link libgridtools
        pkg_check_modules(LIBGRIDTOOLS REQUIRED libgridtools)
        include_directories(${LIBGRIDTOOLS_INCLUDE_DIRS})
        add_definitions(${LIBGRIDTOOLS_CFLAGS})
        set(LIST_LIBNAME ${LIST_LIBNAME} ${LIBGRIDTOOLS_LIBRARIES})
    else ()
        message(STATUS "disable ispostv2 fce")
    endif()
endif()

# IPU FODETv2
if (COMPILE_IPU_FODETV2 STREQUAL "yes")
add_definitions(-DCOMPILE_IPU_FODETV2)
pkg_check_modules(LIBFODETV2 REQUIRED hlibfodetv2)
include_directories(${LIBFODETV2})
add_definitions(${LIBFODETV2_CFLAGS})
set(LIST_LIBNAME ${LIST_LIBNAME} ${LIBFODETV2_LIBRARIES})
set(ipufiles ${ipufiles}
	IPU/FODETv2/FODETv2.cpp IPU/FODETv2/FODETv2UnitControl.cpp
)
include_directories(${videobox_SOURCE_DIR}/IPU/FODETv2)
endif()

# IPU VAMovement
if (COMPILE_IPU_VAMOVEMENT STREQUAL "yes")
add_definitions(-DCOMPILE_IPU_VAMOVEMENT)
set(ipufiles ${ipufiles}
	IPU/VAMovement/VAMovement.cpp
)
include_directories(${videobox_SOURCE_DIR}/IPU/VAMovement)
set(LIST_LIBNAME ${LIST_LIBNAME} m)
endif()

# IPU MVMovement
if (COMPILE_IPU_MVMOVEMENT STREQUAL "yes")
add_definitions(-DCOMPILE_IPU_MVMOVEMENT)
set(ipufiles ${ipufiles}
	IPU/MVMovement/MVMovement.cpp
)
include_directories(${videobox_SOURCE_DIR}/IPU/MVMovement)
set(LIST_LIBNAME ${LIST_LIBNAME} m)
endif()

# IPU VAQRScanner
if (COMPILE_IPU_VAQRSCANNER STREQUAL "yes")
add_definitions(-DCOMPILE_IPU_VAQRSCANNER)
pkg_check_modules(LIBZBAR REQUIRED zbar)
include_directories(${LIBZBAR_INCLUDE_DIRS})
add_definitions(${LIBZBAR_CFLAGS})
set(LIST_LIBNAME ${LIST_LIBNAME} ${LIBZBAR_LIBRARIES})
set(ipufiles ${ipufiles}
	IPU/VAQRScanner/VAQRScanner.cpp IPU/VAQRScanner/VAQRScannerUnitControl.cpp
)
include_directories(${videobox_SOURCE_DIR}/IPU/VAQRScanner)
endif()

# IPU BadEncoder
if (COMPILE_IPU_BADENCODER STREQUAL "yes")
add_definitions(-DCOMPILE_IPU_BADENCODER)
pkg_check_modules(LIBH1 REQUIRED hlibh1v6)
include_directories(${LIBH1_INCLUDE_DIRS})
add_definitions(${LIBH1_CFLAGS})
set(LIST_LIBNAME ${LIST_LIBNAME} ${LIBH1_LIBRARIES})
set(ipufiles ${ipufiles}
	IPU/BadEncoder/BadEncoder.cpp
)
include_directories(${videobox_SOURCE_DIR}/IPU/BadEncoder)
endif()

# IPU TFEStitcher
if (COMPILE_IPU_TFESTITCHER STREQUAL "yes")
add_definitions(-DCOMPILE_IPU_TFESTITCHER)
set(ipufiles ${ipufiles}
	IPU/TFEStitcher/TFEStitcher.cpp
)
include_directories(${videobox_SOURCE_DIR}/IPU/TFEStitcher)
endif()

# IPU G1264
if (COMPILE_IPU_G1264 STREQUAL "yes")
add_definitions(-DCOMPILE_IPU_G1264)
pkg_check_modules(LIBG1264 REQUIRED hlibg1v6)
include_directories(${LIBG1264_INCLUDE_DIRS})
add_definitions(${LIBG1264_CFLAGS})
set(LIST_LIBNAME ${LIST_LIBNAME} ${LIBG1264_LIBRARIES})
set(ipufiles ${ipufiles}
        IPU/G1264/G1264.cpp IPU/G1264/G1264UnitControl.cpp
)
include_directories(${videobox_SOURCE_DIR}/IPU/G1264)
endif()

#IPU_G1JDEC
if (COMPILE_IPU_G1JDEC STREQUAL "yes")
add_definitions(-DCOMPILE_IPU_G1JDEC)
pkg_check_modules(LIBG1JDEC REQUIRED hlibg1v6)
include_directories(${LIBG1JDEC_INCLUDE_DIRS})
add_definitions(${LIBG1JDEC_CFLAGS})
set(LIST_LIBNAME ${LIST_LIBNAME} ${LIBG1JDEC_LIBRARIES})
set(ipufiles ${ipufiles}
    IPU/G1JDEC/G1JDEC.cpp IPU/G1JDEC/G1JDECUnitControl.cpp
)
include_directories(${videobox_SOURCE_DIR}/IPU/G1JDEC)
endif()

# IPU SWC
if (COMPILE_IPU_SWC STREQUAL "yes")
pkg_check_modules(LIBG1 REQUIRED hlibg1v6)
include_directories(${LIBG1_INCLUDE_DIRS})
add_definitions(${LIBG1_CFLAGS})
set(LIST_LIBNAME ${LIST_LIBNAME} ${LIBG1_LIBRARIES})
add_definitions(-DCOMPILE_IPU_SWC)
pkg_check_modules(LIBISPOST REQUIRED hlibispostv1)
include_directories(${LIBISPOST})
add_definitions(${LIBISPOST_CFLAGS})
set(LIST_LIBNAME ${LIST_LIBNAME} ${LIBISPOST_LIBRARIES})
set(ipufiles ${ipufiles}
        IPU/SWC/SWC.cpp
)
include_directories(${videobox_SOURCE_DIR}/IPU/SWC)
endif()

# IPU FFPHOTO
OPTION(COMPILE_FFPHOTO_SWSCALE "Enable ffphoto swscale" "no")
if (COMPILE_IPU_FFPHOTO STREQUAL "yes")
    add_definitions(-DCOMPILE_IPU_FFPHOTO)
    pkg_check_modules(LIBAVCODEC REQUIRED libavcodec libavutil)
    pkg_check_modules(LIBAVFORMAT REQUIRED libavformat)
    include_directories(${LIBAVCODEC_INCLUDE_DIRS})
    include_directories(${LIBAVFORMAT_INCLUDE_DIRS})
    add_definitions(${LIBAVCODEC_CFLAGS})
    add_definitions(${LIBSWSCALE_CFLAGS})
    add_definitions(${LIBAVFORMAT_CFLAGS})
    set(LIST_LIBNAME ${LIST_LIBNAME} ${LIBAVCODEC_LIBRARIES})
    set(LIST_LIBNAME ${LIST_LIBNAME} ${LIBAVUTIL_LIBRARIES})
    set(LIST_LIBNAME ${LIST_LIBNAME} ${LIBAVFORMAT_LIBRARIES})
    if(COMPILE_VIDEOBOX_OPTIMIZATION STREQUAL "yes")
        set(LIST_LIBNAME ${LIST_LIBNAME} fdk-aac)
        set(LIST_LIBNAME ${LIST_LIBNAME} z)
    endif()

    set(ipufiles ${ipufiles}
        IPU/FFPHOTO/FFPHOTO.cpp IPU/FFPHOTO/FFPHOTOUnitControl.cpp
    )
    include_directories(${videobox_SOURCE_DIR}/IPU/FFPHOTO)

    if (COMPILE_FFPHOTO_SWSCALE STREQUAL "yes")
        message(WARNING "-------> FFPHOTO SWSCALE on")
        add_definitions(-DCOMPILE_IPU_FFPHOTO_SWSCALE)
        add_definitions(-DFFPHOTO_SWSCALE_ENABLE)
        pkg_check_modules(LIBSWSCALE REQUIRED libswscale)
        include_directories(${LIBSWSCALE_INCLUDE_DIRS})
        set(LIST_LIBNAME ${LIST_LIBNAME} ${LIBSWSCALE_LIBRARIES})
    else()
        message(WARNING "-------> FFPHOTO SWSCALE off")
    endif()

endif()

#ISPost Plus
if (COMPILE_IPU_ISPLUS STREQUAL "yes")
    add_definitions(-DCOMPILE_IPU_ISPLUS)
    pkg_check_modules(LIBISPOSTV2 REQUIRED hlibispostv2)
    pkg_check_modules(LIBMNEON REQUIRED libmneon)
    include_directories(${LIBISPOSTV2})
    add_definitions(${LIBISPOSTV2_CFLAGS})
    set(LIST_LIBNAME ${LIST_LIBNAME} ${LIBISPOSTV2_LIBRARIES})

    pkg_check_modules(LIBV2505ISPC REQUIRED hlibispv2505)
    include_directories(${LIBV2505ISPC_INCLUDE_DIRS})
    add_definitions(${LIBV2505ISPC_CFLAGS})
    set(LIST_LIBNAME ${LIST_LIBNAME} ${LIBV2505ISPC_LIBRARIES})
    #link hlibcamsensor
    pkg_check_modules(LIBCAMSENSOR REQUIRED hlibcamsensor)
    include_directories(${LIBCAMSENSOR_INCLUDE_DIRS})
    add_definitions(${LIBCAMSENSOR_CFLAGS})
    set(LIST_LIBNAME ${LIST_LIBNAME} ${LIBCAMSENSOR_LIBRARIES})


    set(ipufiles ${ipufiles}
        IPU/ISPLUS/ISPLUS.cpp IPU/ISPLUS/ISPLUSUnitControl.cpp
        IPU/ISPLUS/q3denoise.cpp
        )
    include_directories(${videobox_SOURCE_DIR}/IPU/ISPLUS)
    if (COMPILE_IPU_ISPLUS_FCE STREQUAL "yes")
        message(STATUS "enable isplus  fce")
        message(STATUS ${COMPILE_IPU_ISPLUS_FCE_PARA})
        add_definitions(-DMACRO_IPU_ISPLUS_FCE)
        add_definitions(-DMACRO_IPU_ISPLUS_FCE_PARA="${COMPILE_IPU_ISPLUS_FCE_PARA}")
        #link libgridtools
        pkg_check_modules(LIBGRIDTOOLS REQUIRED libgridtools)
        include_directories(${LIBGRIDTOOLS_INCLUDE_DIRS})
        add_definitions(${LIBGRIDTOOLS_CFLAGS})
        set(LIST_LIBNAME ${LIST_LIBNAME} ${LIBGRIDTOOLS_LIBRARIES})
    else ()
        message(STATUS "disable isplus fce")
    endif()
endif()

if (COMPILE_IPU_BUFSYNC STREQUAL "yes")
    add_definitions(-DCOMPILE_IPU_BUFSYNC)
    set(ipufiles ${ipufiles}
        IPU/BufSync/BufSync.cpp
    )
endif()

# build IPU library
add_library(IPU STATIC ${ipufiles})
link_libraries(IPU)

# build videobox daemon
add_executable(videoboxd Main.cpp)

if (COMPILE_VIDEOBOX_OPTIMIZATION STREQUAL "yes")
    message("begin LIST_LIB:${LIST_LIB}")
    message("begin LIST_LIBNAME:${LIST_LIBNAME}")
    search_slib(LIST_LIB ${LIST_LIBNAME})
    foreach(lib_var ${LIST_LIB})
        message("link lib_var-->${lib_var}")
    endforeach()
    message("search_slib_result  result:${LIST_LIB}")
    foreach(lib_var ${LIST_LIB}) 
        message("link lib_var-->${lib_var}")
        target_link_libraries(videoboxd ${lib_var})
    endforeach()
else()
    message("dynamic compile videobox")
    target_link_libraries(videoboxd ${LIST_LIBNAME})
endif()
# --> build API lib
add_library(videobox SHARED lib/video.c lib/camera.c
			lib/marker.c lib/common.c lib/vam.c)
add_library(videobox_static STATIC lib/video.c lib/camera.c
            lib/marker.c lib/common.c lib/vam.c)
set_target_properties(videobox_static PROPERTIES OUTPUT_NAME videobox CLEAN_DIRECT_OUTPUT 1)

# --> install
install(TARGETS videoboxd DESTINATION bin)
install(TARGETS videobox DESTINATION lib)
install(TARGETS videobox_static DESTINATION lib)
