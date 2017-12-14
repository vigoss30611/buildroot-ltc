#!/bin/bash
#-------------------------------------------------------------------------------
#-                                                                            --
#-       This software is confidential and proprietary and may be used        --
#-        only as expressly authorized by a licensing agreement from          --
#-                                                                            --
#-                            Hantro Products Oy.                             --
#-                                                                            --
#-                   (C) COPYRIGHT 2011 HANTRO PRODUCTS OY                    --
#-                            ALL RIGHTS RESERVED                             --
#-                                                                            --
#-                 The entire notice above must be reproduced                 --
#-                  on all copies and should not be removed.                  --
#-                                                                            --
#-------------------------------------------------------------------------------
#-
#--   Abstract     : Script for compiling versatile test benches              --
#--                                                                           --
#-------------------------------------------------------------------------------

#=====--------------------------------------------------------------------=====#

. ./commonconfig.sh

TARGET=$1

BUILD_LOG=$PWD/"build.log"
rm -rf $BUILD_LOG
if [ -z $TARGET ]
then
    TARGET=versatile
fi

#-------------------------------------------------------------------------------
#-                                                                            --
#- Compiles the decoder test bench.                                           --
#-                                                                            --
#- Parameters                                                                 --
#- 1 : The directory name where test bench make file is located               --
#- 2 : The resulting test bench binary                                        --
#-                                                                            --
#-------------------------------------------------------------------------------
compileDecoderTestBench()
{
    tb=$1
    bin=$2
    echo -n "Compiling ${tb} test bench... "
    if [ -d "../../$tb" ]
    then
        cd ../../${tb}
        make clean >> $BUILD_LOG 2>&1 
        make libclean >> $BUILD_LOG 2>&1 
        if [ "$tb" == "h264high" ]
        then
            if [ "$DEC_2ND_CHROMA_TABLE" == "ENABLED" ] && [ "$PRODUCT" == "9170" ]
            then
                make ENABLE_2ND_CHROMA_FLAG=y SET_EMPTY_PICTURE_DATA=128 $TARGET >> $BUILD_LOG 2>&1 
            else
                make SET_EMPTY_PICTURE_DATA=128 $TARGET >> $BUILD_LOG 2>&1 
            fi
        else
            if [[ "$PRODUCT" == "9170" || "$PRODUCT" == "9190"  || "$PRODUCT" == "g1" ]] && [ "$tb" == "mpeg4" ]
            then
                make CUSTOM_FMT_SUPPORT=y $TARGET >> $BUILD_LOG 2>&1
                #divx_configuration=`echo "$HWCONFIGTAG" | grep divx`
                #if [ ! -z "$divx_configuration" ]
                #then
                #    make CUSTOM_FMT_SUPPORT=y $TARGET >> $BUILD_LOG 2>&1
                #else
                #    make CUSTOM_FMT_SUPPORT=n $TARGET >> $BUILD_LOG 2>&1
                #fi
            elif [[ "$PRODUCT" == "9190" || "$PRODUCT" == "g1" ]] && [ "$tb" == "vp6" ]
            then
                make SET_EMPTY_PICTURE_DATA=128 $TARGET >> $BUILD_LOG 2>&1
            #elif [ "$PRODUCT" == "g1" ] && [ "$tb" == "vp8" ]
            #then
            #    make SET_EMPTY_PICTURE_DATA=128 $TARGET >> $BUILD_LOG 2>&1
            else
                make $TARGET >> $BUILD_LOG 2>&1 
            fi
        fi
        if [ ! -f "$bin" ]
        then
            echo "FAILED"
        else
            echo "OK"
        fi
        cd ../scripts/swhw
    else
        echo "FAILED"
    fi    
}

#-------------------------------------------------------------------------------
#-                                                                            --
#- Compiles the post-processor test bench.                                    --
#-                                                                            --
#- Parameters                                                                 --
#- 1 : The directory name where test bench make file is located               --
#- 2 : The resulting test bench binary                                        --
#-                                                                            --
#-------------------------------------------------------------------------------
compilePostProcessorTestBench()
{
    tb=$1
    bin=$2
    echo -n "Compiling post processor (${tb}) test bench... "
    if [ -d "../../pp/${tb}_mode" ]
    then
        cd "../../pp/${tb}_mode"
        make clean >> $BUILD_LOG 2>&1 
        make libclean >> $BUILD_LOG 2>&1 
        if [ "$tb" == "h264high_pipeline" ]
        then
            make SET_EMPTY_PICTURE_DATA=128 $TARGET >> $BUILD_LOG 2>&1 
        else
            if [[ "$PRODUCT" == "9170" || "$PRODUCT" == "9190"  || "$PRODUCT" == "g1" ]] && [ "$tb" == "mpeg4_pipeline" ]
            then
                make CUSTOM_FMT_SUPPORT=y $TARGET >> $BUILD_LOG 2>&1
                #divx_configuration=`echo "$HWCONFIGTAG" | grep divx`
                #if [ ! -z "$divx_configuration" ]
                #then
                #    make CUSTOM_FMT_SUPPORT=y $TARGET >> $BUILD_LOG 2>&1
                #else
                #    make CUSTOM_FMT_SUPPORT=n $TARGET >> $BUILD_LOG 2>&1
                #fi
            elif [[ "$PRODUCT" == "9190"  || "$PRODUCT" == "g1" ]] && [ "$tb" == "vp6_pipeline" ]
            then
                make SET_EMPTY_PICTURE_DATA=128 $TARGET >> $BUILD_LOG 2>&1 
                
            else
                make $TARGET >> $BUILD_LOG 2>&1 
            fi
        fi
        if [ ! -f "$bin" ]
        then
            echo "FAILED"
        else
            echo "OK"
        fi
        cd ../../scripts/swhw
    else
        echo "FAILED"
    fi    
}

#-------------------------------------------------------------------------------
#-                                                                            --
#- Compiles memalloc kernel module.                                           --
#-                                                                            --
#- Parameters                                                                 --
#- 1 : The resulting binary                                                   --
#-                                                                            --
#-------------------------------------------------------------------------------
compileMemalloc()
{
    bin=$1
    memalloc_dir="../../../linux/memalloc/"
    echo -n "Compiling memalloc... "
    if [ -d "$memalloc_dir" ]
    then
        cd ${memalloc_dir}
        make clean >> $BUILD_LOG 2>&1 
        make >> $BUILD_LOG 2>&1  
        if [ ! -f "$bin" ]
        then
            echo "FAILED"
        else
            echo "OK"
        fi
        cd ../../test/scripts/swhw
    else
        echo "FAILED"
    fi    
}

#-------------------------------------------------------------------------------
#-                                                                            --
#- Compiles ldriver kernel module.                                           --
#-                                                                            --
#- Parameters                                                                 --
#- 1 : The resulting binary                                                   --
#-                                                                            --
#-------------------------------------------------------------------------------
compileLDriver()
{
    bin=$1
    ldriver_dir="../../../linux/ldriver/kernel_26x/"
    echo -n "Compiling ldriver... "
    if [ -d "$ldriver_dir" ]
    then
        cd ${ldriver_dir}
        make clean >> $BUILD_LOG 2>&1 
        make >> $BUILD_LOG 2>&1 
        if [ ! -f "$bin" ]
        then
            echo "FAILED"
        else
            echo "OK"
        fi
        cd ../../../test/scripts/swhw
    else
        echo "FAILED"
    fi    
}

# compile all the binaries
echo "Compile target: ${TARGET}"
if [ "$PRODUCT" == "8170" ]
then
    compileDecoderTestBench "h264" "hx170dec_${TARGET}"

elif [ "$PRODUCT" == "8190" ] || [ "$PRODUCT" == "9170" ] || [ "$PRODUCT" == "9190" ] || [ "$PRODUCT" == "g1" ]
then
    compileDecoderTestBench "h264high" "hx170dec_${TARGET}"
fi
compileDecoderTestBench "jpeg" "jx170dec_${TARGET}"
compileDecoderTestBench "mpeg4" "mx170dec_${TARGET}"
compileDecoderTestBench "vc1" "vx170dec_${TARGET}"
compileDecoderTestBench "mpeg2" "m2x170dec_${TARGET}"
if [ "$PRODUCT" == "9170" ] || [ "$PRODUCT" == "9190" ] || [ "$PRODUCT" == "g1" ]
then
    compileDecoderTestBench "rv" "rvx170dec_${TARGET}"
fi
if [ "$PRODUCT" == "9190" ] || [ "$PRODUCT" == "g1" ]
then
    compileDecoderTestBench "vp6" "vp6dec_${TARGET}"
fi
if [ "$PRODUCT" == "g1" ]
then
    compileDecoderTestBench "vp8" "vp8x170dec_${TARGET}"
    compileDecoderTestBench "avs" "ax170dec_${TARGET}"
fi

compilePostProcessorTestBench "external" "px170dec_${TARGET}"
if [ "$PRODUCT" == "8170" ]
then
    compilePostProcessorTestBench "h264_pipeline" "pp_h264_pipe"
    
elif [ "$PRODUCT" == "8190" ] || [ "$PRODUCT" == "9170" ] || [ "$PRODUCT" == "9190" ] || [ "$PRODUCT" == "g1" ]
then
    compilePostProcessorTestBench "h264high_pipeline" "pp_h264_pipe"
fi 
compilePostProcessorTestBench "jpeg_pipeline" "pp_jpeg_pipe"
compilePostProcessorTestBench "mpeg4_pipeline" "pp_mpeg4_pipe"
compilePostProcessorTestBench "vc1_pipeline" "pp_vc1_pipe"
compilePostProcessorTestBench "mpeg2_pipeline" "pp_mpeg2_pipe"
if [ "$PRODUCT" == "9170" ] || [ "$PRODUCT" == "9190" ] || [ "$PRODUCT" == "g1" ]
then
    compilePostProcessorTestBench "rv_pipeline" "pp_rv_pipe"
fi
if [ "$PRODUCT" == "9190" ] || [ "$PRODUCT" == "g1" ]
then
    compilePostProcessorTestBench "vp6_pipeline" "pp_vp6_pipe"
fi
if [ "$PRODUCT" == "g1" ]
then
    compilePostProcessorTestBench "vp8_pipeline" "pp_vp8_pipe"
    compilePostProcessorTestBench "avs_pipeline" "pp_avs_pipe"
fi

compileMemalloc "memalloc.ko"
compileLDriver "hx170dec.ko"

warning=`cat $BUILD_LOG | grep -m 1 -w " warning:"`

if [ ! -z "$warning" ]
then
    echo "There was warning(s)."
fi

echo "See build.log for more information on building."

exit 0

