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
#--   Abstract     : Configuration script for the multiinstance test cases    --
#--                                                                           --
#-------------------------------------------------------------------------------

#=====--------------------------------------------------------------------=====#

rm -rf multiinstance
mkdir multiinstance
cd multiinstance
ln -s ../*.sh .
chmod 777 .

# Settings from default directory is used
cp ../integration_default/config.sh .
. ./config.sh

# Copy and modify the test case list for mi testing
rm -f testcase_list
cp $TEST_CASE_LIST_CHECK testcase_list.tmp
sed 's/category_list_multi_inst/category_list_swhw/' testcase_list.tmp > testcase_list
rm -f testcase_list.tmp

# Modify the configuration file for mi testing
mv -f config.sh config.sh.tmp
sed 's/export TEST_CASE_LIST_RUN=.*/export TEST_CASE_LIST_RUN=\"\.\/testcase_list\"/' config.sh.tmp > config.sh
mv -f config.sh config.sh.tmp
sed 's/export TEST_CASE_LIST_CHECK=.*/export TEST_CASE_LIST_CHECK=\"\.\/testcase_list\"/' config.sh.tmp > config.sh
mv -f config.sh config.sh.tmp
sed 's/="..\/..\//="..\/..\/..\//' config.sh.tmp > config.sh
mv -f config.sh config.sh.tmp
sed 's/export TB_H264_LONG_STREAM=.*/export TB_H264_LONG_STREAM=\"DISABLED"/' config.sh.tmp > config.sh
rm -f config.sh.tmp
