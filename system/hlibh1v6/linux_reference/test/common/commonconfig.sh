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
#--   Abstract     : Common configuration script for the execution of         --
#--                  the SW/HW  verification test cases.                      --
#--                                                                           --
#-------------------------------------------------------------------------------

# Check that this script is run in its dir
if [ ! -e commonconfig.sh ]; then
    echo "This script must be run in test/common dir!"
    exit -1
fi

#=================== UPDATE THESE VALUES ACCORDING TO TEST ===================

#Component versions
export swtag="ench1_4_47"   # This version of SW will be checked out
export hwtag="ench1_4_47"   # This should be what is installed on FPGA
export systag="ench1_4_47"  # This will be checked out and compared against
export cfgtag="cfg_ench1_4_47_vp8_h264_sahi_e1920_axiahb" # This should be what is installed on FPGA

#Test Device IP
export testdeviceip=75      # IP will set the board specific values

#Testcase groups: all vp8 h264 jpeg random-vp8 random-h264 random-jpeg
#Testcase sorting: size-order random-order
#Testcase limits eg. "width<1981" rotation=0 videoStab=0 "inputFormat<5"
tasklistparams="all size-order"

#Comments about test settings
export comment=""

#Write test results to test data base in labtest.sh
export use_db="y"

#To enable internal test cases with extended api. Usually disabled with customer releases
INTERNAL_TEST="y"

#Maximum input resolution according to config
MAX_WIDTH="4080"
MAX_HEIGHT="4080"

#Tracefiles used in case comparison
TRACE_SW_REGISTER="y"
TRACE_SCALED="y"
TRACE_EWL="y"
TRACE_RECON_INTERNAL="n"

# Optional information
export testdeviceversion=""

# How many cases to run with randomized parameters for H.264/JPEG, max 1000
export RANDOM_PARAMETER_CASE_AMOUNT=900

#============================ DON'T TOUCH =================================

PRODUCT="H1"

PROJECT_DIR="/export/work/gforce"
SYSTEM_MODEL_GIT="/export/work/gforce/GIT/h1_encoder"

# Base dir for product software
SWBASE="$PWD/../../.."

# System model directories used by the scripts
# Model will be checked out into sys_$systag
SYSTEM_MODEL_BASE="$PWD/sys_$systag"
export SYSTEM_MODEL_HOME="$SYSTEM_MODEL_BASE/system"
export TEST_DATA_HOME="$SYSTEM_MODEL_BASE"
export TEST_DATA_FILES="$SYSTEM_MODEL_BASE/test_data_files.txt"
system_model_home=$SYSTEM_MODEL_HOME
test_data_files=$TEST_DATA_FILES
test_case_list_dir="$SYSTEM_MODEL_HOME/test_data"
USER_DATA_HOME="$SYSTEM_MODEL_HOME/test_data"
# Testcase input data directories
export YUV_SEQUENCE_HOME=/export/sequence/yuv

# These input files are copied from:
# proto3@172.28.107.124:/mnt/lacie/decoder_stability/
movieinput=(/export/sequence/stream/raw/long_term/vc1/main/24_season5_1280x720_5200kbps.rcv
            /export/sequence/stream/raw/long_term/vc1/advance/v_for_vendetta_1920x1080_19000kbps.vc1)

# random_run signals that the cases are run in random order
# TODO fix this directory mess
if [ -e random_run ]
then
    TESTDIR=$PWD/random_cases
else
    TESTDIR=$PWD
fi

#Test environment configuration and information

# Configuration for AHB versatile board with interrupts
if [ $testdeviceip -ge 85 ] && [ $testdeviceip -le 87 ]
then
    export kernelversion=linux-2.6.28-arm1/v0_1/linux-2.6.28-arm1
    export rootfsversion=v2_3_0_2
    export compilerversion=arm-2009q1-203-arm-none-linux-gnueabi-i686-pc-linux-gnu

    # Board Configuration: EB, AXIVERSATILE, VERSATILE
    BOARD="VERSATILE"

    # Kernel directory for kernel modules compilation
    KDIR="/export/Testing/Board_Version_Control/Realview_PB/PB926EJS/SW/Linux/linux-2.6.28-arm1/v0_1/linux-2.6.28-arm1"

    # DWL implementation to use: POLLING, IRQ
    DWL_IMPLEMENTATION="IRQ"

fi

# Configuration for AXI versatile board without interrupts
if [ $testdeviceip -ge 80 ] && [ $testdeviceip -le 82 ]
then
    export kernelversion=linux-2.6.28-arm1/v0_0-v6/linux-2.6.28-arm1
    export rootfsversion=v2_5_0
    export compilerversion=arm-2009q1-203-arm-none-linux-gnueabi-i686-pc-linux-gnu

    # Board Configuration: EB, AXIVERSATILE, VERSATILE
    BOARD="AXIVERSATILE"

    # Kernel directory for kernel modules compilation
    KDIR="/export/Testing/Board_Version_Control/SW_Common/ARM_realview_v6/2.6.28-arm1/v0_0-v6/linux-2.6.28-arm1"

    # DWL implementation to use: POLLING, IRQ
    DWL_IMPLEMENTATION="POLLING"
fi

# Configuration for Socle board with interrupts
if [ $testdeviceip -ge 70 ] && [ $testdeviceip -le 76 ]
then
    export kernelversion=2.6.29/v0_5/android_linux-2.6.29
    export rootfsversion=openlinux_2_0
    export compilerversion=arm-2009q1-203-arm-none-linux-gnueabi-i686-pc-linux-gnu

    # Board Configuration: EB, AXIVERSATILE, VERSATILE, SOCLE
    BOARD="SOCLE"

    # Kernel directory for kernel modules compilation
    KDIR="/export/Testing/Board_Version_Control/SW_Common/SOCLE_MDK-3D/openlinux/2.6.29/v0_5/android_linux-2.6.29"

    # DWL implementation to use: POLLING, IRQ
    DWL_IMPLEMENTATION="POLLING"

fi

# Configuration for AXI VEXPRESS board without interrupts
if [ $testdeviceip -eq 99 ]
then
    export kernelversion=linux-linaro-3.2-2012.01-0
    export rootfsversion=Linaro-12.01-nano
    export compilerversion=arm-2011.03-arm-none-linux-gnueabi

    # Board Configuration: EB, AXIVERSATILE, VERSATILE, VEXPRESS
    BOARD="VEXPRESS"

    # Kernel directory for kernel modules compilation
    KDIR="/export/Testing/Board_Version_Control/SW_Common/VExpress/linux-linaro-3.2-2012.01-0"

    # DWL implementation to use: POLLING, IRQ
    DWL_IMPLEMENTATION="POLLING"
fi

# TODO add configurations if testing on some other board

# Report information
reporter=$USER
timeform1=`date +%d.%m.20%y`
timeform2=`date +%k:%M:%S`

if [ "$REPORTTIME" == "" ]
then
    reporttime=`date +%d%m%y_%k%M | sed -e 's/ //'`
else
    reporttime=$REPORTTIME
fi

root_prefix=""
if [ -d "/misc/export" ]
then
    root_prefix="/misc"
fi

csv_path="$PWD"

#Compiler Settings
COMPILER_SETTINGS="/export/tools/i386_linux24/usr/arm/${compilerversion}/settings.sh"

#Endian width
ENDIAN_WIDTH_64_BIT="1"
ENDIAN_WIDTH_32_BIT="0"

# Memory base address configuration and interrupt line

if [ "$DWL_IMPLEMENTATION" == "POLLING" ]
then
    IRQ_LINE="-1"
fi

if [ "$BOARD" == "EB" ]
then
    HW_BASE_ADDRESS="0x84000000"
    MEM_BASE_ADDRESS="0x08000000"
    MEM_BASE_ADDRESS_END="0x0DFFFFFF"
    
    if [ "$DWL_IMPLEMENTATION" == "IRQ" ]
    then 
        IRQ_LINE="72"
    fi

elif [ "$BOARD" == "AXIVERSATILE" ]
then
    HW_BASE_ADDRESS="0xC4000000"
    # Linear memory space 256MB
    MEM_BASE_ADDRESS="0x80000000"
    MEM_BASE_ADDRESS_END="0x8FFFFFFF"

    # IRQ's not supported
elif [ "$BOARD" == "VERSATILE" ]
then
    HW_BASE_ADDRESS="0xC0000000"
    # Linear memory space 96MB
    MEM_BASE_ADDRESS="0x02000000"
    MEM_BASE_ADDRESS_END="0x07FFFFFF"

    if [ "$DWL_IMPLEMENTATION" == "IRQ" ]
    then
        IRQ_LINE="30"
    fi
elif [ "$BOARD" == "SOCLE" ]
then
    HW_BASE_ADDRESS="0x20000000"
    # Linear memory space 128MB
    MEM_BASE_ADDRESS="0x44000000"
    MEM_BASE_ADDRESS_END="0x4FFFFFFF"

    if [ "$DWL_IMPLEMENTATION" == "IRQ" ]
    then
        IRQ_LINE="30"
    fi
elif [ "$BOARD" == "VEXPRESS" ]
then
    HW_BASE_ADDRESS="0xFC010000"
    # Linear memory space 448MB
    MEM_BASE_ADDRESS="0x84000000"
    MEM_BASE_ADDRESS_END="0x9FFFFFFF"

    if [ "$DWL_IMPLEMENTATION" == "IRQ" ]
    then
        IRQ_LINE="30"
    fi
else
    echo "Unknown board configuration, fix commonconfig.sh!"
    exit -1
fi





