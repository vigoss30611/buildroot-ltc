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
#--   Abstract     : Script for setting up configuration(s).                  --
#--                                                                           --
#-------------------------------------------------------------------------------

#=====--------------------------------------------------------------------=====#

. ./f_testdbquery.sh

checkHWConfigTag()
{
    local hw_config_tag=$1
    
    local query="SELECT id FROM config_tag WHERE name = \"${hw_config_tag}\";"
    local db_result=$(dbQuery "$query")
    if [ -z "$db_result" ]
    then
        return 1
    fi
    return 0
}

# This script must be run in common dir
# Check that the dir is correct
if [ ! -e setupconfig.sh ]
then
    echo "This script must be run in masterscripts dir!"
    exit -1
fi

. commonconfig.sh

# Check that configuration tag is valid
checkHWConfigTag "$cfgtag"
retval=$?
if [ $retval -ne 0 ]
then
    echo "Not a valid HW configuration tag: ${cfgtag}"
    exit $retval
fi


# Check that the config is valid

if [ ! -d $KDIR ]
then
    echo "Invalid kernel dir: $KDIR"
    exit
fi

if [ ! -e $COMPILER_SETTINGS ]
then
    echo "Invalid compiler settings: $COMPILER_SETTINGS"
    exit
fi

sed_file()
{
    # $1 = sed parameter
    # $2 = file to parse

    sed "$1" $2 > tmp
    mv tmp $2
}

# set up compile time flags
echo "Setting up compiler flags in config headers and Makefiles..."
echo "IP=$testdeviceip"
echo "BOARD=$BOARD"

# HW base address
echo "HW_BASE_ADDRESS=$HW_BASE_ADDRESS"
sed_file "s/#define[ \t]*INTEGRATOR_LOGIC_MODULE0_BASE[ \t].*/#define INTEGRATOR_LOGIC_MODULE0_BASE $HW_BASE_ADDRESS/" $SWBASE/linux_reference/kernel_module/hx280enc.c

# Memory base address
echo "MEM_BASE_ADDRESS=$MEM_BASE_ADDRESS"
sed_file "s/HLINA_START[ \t]*=[ \t]*.*/HLINA_START = $MEM_BASE_ADDRESS/" $SWBASE/linux_reference/memalloc/Makefile

echo "MEM_BASE_ADDRESS_END=$MEM_BASE_ADDRESS_END"
sed_file "s/HLINA_END[ \t]*=[ \t]*.*/HLINA_END = $MEM_BASE_ADDRESS_END/" $SWBASE/linux_reference/memalloc/Makefile


# Kernel directory for kernel modules compilation
echo "KDIR=$KDIR"
sed_file "s,KDIR[ \t]*:=.*,KDIR := $KDIR," $SWBASE/linux_reference/memalloc/Makefile
sed_file "s,KDIR[ \t]*:=.*,KDIR := $KDIR," $SWBASE/linux_reference/kernel_module/Makefile


#Interrupt or polling mode
echo "DWL_IMPLEMENTATION=$DWL_IMPLEMENTATION"
if [ "$DWL_IMPLEMENTATION" == "IRQ" ]
then
    sed_file "s/POLLING[ \t]*=.*/POLLING = n/" $SWBASE/linux_reference/Makefile

elif [ "$DWL_IMPLEMENTATION" == "POLLING" ]
then
    sed_file "s/POLLING[ \t]*=.*/POLLING = y/" $SWBASE/linux_reference/Makefile
fi


# IRQ line
echo "IRQ_LINE=$IRQ_LINE"
sed_file "s/#define[ \t]*VP_PB_INT_LT.*/#define VP_PB_INT_LT $IRQ_LINE/" $SWBASE/linux_reference/kernel_module/hx280enc.c


#Endian width
if ( [ "$BOARD" == "EB" ] || [ "$BOARD" == "AXIVERSATILE" ] || [ "$BOARD" == "SOCLE" ]  || [ "$BOARD" == "VEXPRESS" ] )
then

    sed_file "s/#define[ \t]*ENCH1_INPUT_SWAP_32_YUV.*/#define ENCH1_INPUT_SWAP_32_YUV $ENDIAN_WIDTH_64_BIT/" $SWBASE/source/common/enccfg.h
    sed_file "s/#define[ \t]*ENCH1_INPUT_SWAP_32_RGB16.*/#define ENCH1_INPUT_SWAP_32_RGB16 $ENDIAN_WIDTH_64_BIT/" $SWBASE/source/common/enccfg.h
    sed_file "s/#define[ \t]*ENCH1_INPUT_SWAP_32_RGB32.*/#define ENCH1_INPUT_SWAP_32_RGB32 $ENDIAN_WIDTH_64_BIT/" $SWBASE/source/common/enccfg.h
    sed_file "s/#define[ \t]*ENCH1_OUTPUT_SWAP_32.*/#define ENCH1_OUTPUT_SWAP_32 $ENDIAN_WIDTH_64_BIT/" $SWBASE/source/common/enccfg.h
    
    sed_file "s/#define[ \t]*VSH1_INPUT_SWAP_32_YUV.*/#define VSH1_INPUT_SWAP_32_YUV $ENDIAN_WIDTH_64_BIT/" $SWBASE/source/camstab/vidstabcfg.h
    sed_file "s/#define[ \t]*VSH1_INPUT_SWAP_32_RGB16.*/#define VSH1_INPUT_SWAP_32_RGB16 $ENDIAN_WIDTH_64_BIT/" $SWBASE/source/camstab/vidstabcfg.h
    sed_file "s/#define[ \t]*VSH1_INPUT_SWAP_32_RGB32.*/#define VSH1_INPUT_SWAP_32_RGB32 $ENDIAN_WIDTH_64_BIT/" $SWBASE/source/camstab/vidstabcfg.h

elif [ "$BOARD" == "VERSATILE" ]
then

    sed_file "s/#define[ \t]*ENCH1_INPUT_SWAP_32_YUV.*/#define ENCH1_INPUT_SWAP_32_YUV $ENDIAN_WIDTH_32_BIT/" $SWBASE/source/common/enccfg.h
    sed_file "s/#define[ \t]*ENCH1_INPUT_SWAP_32_RGB16.*/#define ENCH1_INPUT_SWAP_32_RGB16 $ENDIAN_WIDTH_32_BIT/" $SWBASE/source/common/enccfg.h
    sed_file "s/#define[ \t]*ENCH1_INPUT_SWAP_32_RGB32.*/#define ENCH1_INPUT_SWAP_32_RGB32 $ENDIAN_WIDTH_32_BIT/" $SWBASE/source/common/enccfg.h
    sed_file "s/#define[ \t]*ENCH1_OUTPUT_SWAP_32.*/#define ENCH1_OUTPUT_SWAP_32 $ENDIAN_WIDTH_32_BIT/" $SWBASE/source/common/enccfg.h
    
    sed_file "s/#define[ \t]*VSH1_INPUT_SWAP_32_YUV.*/#define VSH1_INPUT_SWAP_32_YUV $ENDIAN_WIDTH_32_BIT/" $SWBASE/source/camstab/vidstabcfg.h
    sed_file "s/#define[ \t]*VSH1_INPUT_SWAP_32_RGB16.*/#define VSH1_INPUT_SWAP_32_RGB16 $ENDIAN_WIDTH_32_BIT/" $SWBASE/source/camstab/vidstabcfg.h
    sed_file "s/#define[ \t]*VSH1_INPUT_SWAP_32_RGB32.*/#define VSH1_INPUT_SWAP_32_RGB32 $ENDIAN_WIDTH_32_BIT/" $SWBASE/source/camstab/vidstabcfg.h
    
fi

#Setup Internal test
echo "INTERNAL_TEST=$INTERNAL_TEST"
if ( [ "$INTERNAL_TEST" == "y" ] )
then
    sed_file "s/INTERNAL_TEST[ \t]*=.*/INTERNAL_TEST = y/" $SWBASE/linux_reference/test/h264/Makefile

elif ( [ "$INTERNAL_TEST" == "n" ] )
then
    sed_file "s/INTERNAL_TEST[ \t]*=.*/INTERNAL_TEST = n/" $SWBASE/linux_reference/test/h264/Makefile

fi


echo "TRACE_EWL=$TRACE_EWL"
if ( [ "$TRACE_EWL" == "y" ] )
then
    sed_file "s,.DEBFLAGS += -DTRACE_EWL.,DEBFLAGS += -DTRACE_EWL," $SWBASE/linux_reference/Makefile

elif ( [ "$TRACE_EWL" == "n" ] )
then
    sed_file "s,.DEBFLAGS += -DTRACE_EWL.,#DEBFLAGS += -DTRACE_EWL," $SWBASE/linux_reference/Makefile

fi


# Checkout system model with correct tag
if [ ! -d "$SYSTEM_MODEL_BASE" ]
then
        (
        echo "Checking out system model with tag $systag"

        git clone -n $SYSTEM_MODEL_GIT $SYSTEM_MODEL_BASE

        cd $SYSTEM_MODEL_BASE

        if [ `git tag | grep -c $systag` == "0" ]
        then
                echo "System model tag $systag is not correct!"
                exit
        fi

        # First checkout the system and software using SW tag
        echo "git checkout -b branch_$systag $swtag"
        git checkout -b branch_$systag $swtag

        if [ "$swtag" != "$systag" ]; then
            # Then checkout just the system model library using system tag
            echo "git checkout $systag system"
            git checkout $systag system
        fi

        echo "video_stab_result.log" > test_data_files.txt
        #echo "swreg.trc" >> test_data_files.txt
        )
fi


echo "TRACE_SW_REGISTER=$TRACE_SW_REGISTER"
if ( [ "$TRACE_SW_REGISTER" == "y" ] )
then
    sed_file "s,.DEBFLAGS += -DTRACE_REGS.,DEBFLAGS += -DTRACE_REGS," $SWBASE/linux_reference/Makefile
    sed_file "s,.DEBFLAGS += -DTRACE_REGS.,DEBFLAGS += -DTRACE_REGS," $SYSTEM_MODEL_BASE/software/linux_reference/Makefile

elif ( [ "$TRACE_SW_REGISTER" == "n" ] )
then
    sed_file "s,.DEBFLAGS += -DTRACE_REGS.,#DEBFLAGS += -DTRACE_REGS," $SWBASE/linux_reference/Makefile
    sed_file "s,.DEBFLAGS += -DTRACE_REGS.,#DEBFLAGS += -DTRACE_REGS," $SYSTEM_MODEL_BASE/software/linux_reference/Makefile

fi

echo "TRACE_RECON_INTERNAL=$TRACE_RECON_INTERNAL"
if ( [ "$TRACE_RECON_INTERNAL" == "y" ] )
then
    sed_file "s,.DEBFLAGS += -DTRACE_RECON.,DEBFLAGS += -DTRACE_RECON," $SWBASE/linux_reference/Makefile
    sed_file "s,.DEBFLAGS += -DTRACE_RECON.,DEBFLAGS += -DTRACE_RECON," $SYSTEM_MODEL_BASE/software/linux_reference/Makefile

elif ( [ "$TRACE_RECON_INTERNAL" == "n" ] )
then
    sed_file "s,.DEBFLAGS += -DTRACE_RECON.,#DEBFLAGS += -DTRACE_RECON," $SWBASE/linux_reference/Makefile
    sed_file "s,.DEBFLAGS += -DTRACE_RECON.,#DEBFLAGS += -DTRACE_RECON," $SYSTEM_MODEL_BASE/software/linux_reference/Makefile

fi



