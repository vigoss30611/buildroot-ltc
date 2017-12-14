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
#--   Abstract     : Script for copying test scripts and binaries to
#                    test directory                                           --
#--                                                                           --
#-------------------------------------------------------------------------------

#=====--------------------------------------------------------------------=====#

. commonconfig.sh

cp $SWBASE/linux_reference/memalloc/memalloc.ko .
cp $SWBASE/linux_reference/memalloc/memalloc_load.sh .
cp $SWBASE/linux_reference/kernel_module/hx280enc.ko .
cp $SWBASE/linux_reference/kernel_module/driver_load.sh .
# Testbenches copied in make script

if [ -e ref ] ; then rm ref ; fi
ln -s $TEST_DATA_HOME ref
