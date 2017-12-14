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
#--   Abstract     : Script for cleaning the test directory. Doesn't delete   --
#                    test scripts                                             --
#--                                                                           --
#-------------------------------------------------------------------------------

#=====--------------------------------------------------------------------=====#

echo -n "Delete all compiled binaries from previous testing? [y/n] "
read ans
if [ "$ans" == "y" ]
then
    rm -rf memalloc.ko
    rm -rf hx280enc.ko
    rm -rf h264_testenc 
    rm -rf jpeg_testenc 
    rm -rf vp8_testenc 
    rm -rf videostabtest
fi

echo -n "Delete reference data from previous testing? [y/n] "
read rmRef

echo -n "Delete test data from previous testing? [y/n] "
read rmAll

if [ "$rmRef" == "y" ]
then
    rm -rf sys_*/case_*
fi

if [ "$rmAll" == "y" ]
then
    rm -rf smoketest_ok
    rm -rf smoketest_not_ok
    rm -rf smoketest_done
    rm -rf case_*
    rm -rf vs_case*
    rm -rf random_*
    rm -rf *.trc
    rm -rf *.yuv
fi


