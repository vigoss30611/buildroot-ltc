#!/bin/bash
#-------------------------------------------------------------------------------
#-                                                                            --
#-       This software is confidential and proprietary and may be used        --
#-        only as expressly authorized by a licensing agreement from          --
#-                                                                            --
#-                            Hantro Products Oy.                             --
#-                                                                            --
#-                   (C) COPYRIGHT 2006 HANTRO PRODUCTS OY                    --
#-                            ALL RIGHTS RESERVED                             --
#-                                                                            --
#-                 The entire notice above must be reproduced                 --
#-                  on all copies and should not be removed.                  --
#-                                                                            --
#-------------------------------------------------------------------------------
#-
#--   Abstract     : Script for compiling software and test benches for       --
#                    versatile                                                --
#--                                                                           --
#-------------------------------------------------------------------------------

#=====--------------------------------------------------------------------=====#

# This script must be run in test/common dir
# Check that the dir is correct
if [ ! -e make_binaries.sh ]
then
    echo "This script must be run in common dir!"
    exit -1
fi


. commonconfig.sh

BUILD_LOG=$PWD/"build.log"
rm -rf $BUILD_LOG

ROOTDIR=$PWD
TESTDIR=$PWD

#-------------------------------------------------------------------------------
#-                                                                            --
#- Compiles modules                                                           --
#-                                                                            --
#- Parameters                                                                 --
#- 1 : The module dir to compile                                              --
#- 2 : The resulting module binary                                            --
#-                                                                            --
#-------------------------------------------------------------------------------

compileModule()
{
    compile_dir=$1
    bin=$2
    echo -n "Compiling "${compile_dir##*/}" module... "
    if [ -d "$compile_dir" ]
    then
        cd ${compile_dir}
        make clean >> $BUILD_LOG 2>&1
        make $TARGET >> $BUILD_LOG 2>&1
        if [ ! -f "$bin" ]
        then
            echo "FAILED"
        else
            echo "OK"
        fi
        cd $ROOTDIR
    else
        echo "FAILED"
    fi
}

compileTestbench()
{
    compile_dir=$1
    bin=$2
    echo -n "Compiling "${compile_dir##*/}" module... "
    if [ -d "$compile_dir" ]
    then
        cd ${compile_dir}
        make clean >> $BUILD_LOG 2>&1
        make $TARGET >> $BUILD_LOG 2>&1
        if [ ! -f "$bin" ]
        then
            echo "FAILED"
        else
            echo "OK"
            cp $2 $ROOTDIR  # Copy the binary into testdir
        fi
        cd $ROOTDIR
    else
        echo "FAILED"
    fi
}

# compile all the binaries

echo COMPILER_SETTINGS=$COMPILER_SETTINGS
. "${COMPILER_SETTINGS}"

TARGET=versatile
echo "Compile target: ${TARGET}"
compileModule "$SWBASE/linux_reference" "libh1enc.a"
compileTestbench "$SWBASE/linux_reference/test/h264" "h264_testenc"
compileTestbench "$SWBASE/linux_reference/test/vp8" "vp8_testenc"
compileTestbench "$SWBASE/linux_reference/test/jpeg" "jpeg_testenc"
compileTestbench "$SWBASE/linux_reference/test/camstab" "videostabtest"

TARGET=versatile_multifile
echo "Compile target: ${TARGET}"
compileTestbench "$SWBASE/linux_reference/test/h264" "h264_testenc_multifile"
compileTestbench "$SWBASE/linux_reference/test/vp8" "vp8_testenc_multifile"

TARGET=""
echo "Compile target: ${TARGET}"
compileModule "$SWBASE/linux_reference/memalloc" "memalloc.ko"
compileModule "$SWBASE/linux_reference/kernel_module" "hx280enc.ko"
compileModule "$SWBASE/linux_reference/test/integration" "integration_testbench"

compileModule "$SYSTEM_MODEL_HOME" "vp8_testenc"

