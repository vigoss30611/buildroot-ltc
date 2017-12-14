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
#--   Abstract     : Script functions for checking the results the SW/HW      --
#--                  verification test cases                                  --
#--                                                                           --
#-------------------------------------------------------------------------------


importFiles()
{
    # import commonconfig
    if [ ! -e commonconfig.sh ]
    then
        echo "commonconfig.sh not found!"
        exit -1
    else
        .  ./commonconfig.sh
    fi

    # check that config dirs are valid
    if [ ! -d "$test_case_list_dir" ]; then
        echo "Invalid test_case_list_dir in commonconfig.sh."
        exit -1
    fi
    if [ ! -d "$SYSTEM_MODEL_HOME" ]; then
        echo "Invalid SYSTEM_MODEL_HOME in commonconfig.sh."
        exit -1
    fi

    # Allow all permissions to access files on labws and fpga board
    umask 000
}

saveFileToDir()
{
    # param1 = file to copy
    # param2 = destination directory
    # param3 = destination filename

    if [ ! -e $1 ]
    then
        return
    fi

    if [ ! -e $2 ]
    then
        mkdir -p $2
        if [ ! -e $2 ]
        then
            echo -n "Failed to create $2! "
            return
        fi
    fi

    cp $1 $2/$3
}

#-------------------------------------------------------------------------------
#-                                                                            --
#- Calculates the execution time for the test case                            --
#-                                                                            --
#-                                                                            --
#- Parameters                                                                 --
#- 1 : Test case number			                                      --
#-                                                                            --
#- Exports                                                                    --
#- execution_time                                                             --
#-                                                                            --
#-------------------------------------------------------------------------------

getExecutionTime()
{
    casetime="$1/case.time"
    
    execution_time="N/A"
    if [ -e $casetime ]
    then
        if [ `grep -c START ${casetime}` == 1 ] && [ `grep -c END ${casetime}` == 1 ]
        then
            start_time=`grep START ${casetime} | awk -F : '{print $2}'`
            end_time=`grep END ${casetime} | awk -F : '{print $2}'`
            let 'execution_time=end_time-start_time'
        fi
    fi
    
    export execution_time
}

# Find the failing picture
# $1 and $2 are .yuv files to compare
# $picWidth and $picHeight are used for picture dimensions
# return 0 for equal, 1 for difference 
findFailingPicture()
{
    if ! [ -e $1 ] && ! [ -e $2  ]; then
        echo -n "No YUV files! "
        return 1
    elif ! [ -e $1 ]
    then
        echo -n "No YUV test file! "
        return 1
    elif ! [ -e $2 ]
    then
        echo -n "No YUV ref file! "
        return 1
    fi

    # Find the failing picture
    # Calculate the failing picture using the picture width and height
    error_pixel_number=`cmp $1 $2 | awk '{print $5}' | awk 'BEGIN {FS=","} {print $1}'`

    if [ "$error_pixel_number" != "" ]
    then
        # The multiplier should be 1,5
        pixel_in_pic=$((picWidth*picHeight*3/2))
        if [ $pixel_in_pic -eq 0 ]
        then
            echo -n "YUV differs in pixel=$error_pixel_number. "
            return 1
        fi
        failing_picture=$((error_pixel_number/pixel_in_pic))
        error_pixel=$((error_pixel_number-pixel_in_pic*failing_picture))
        pixelx=$((error_pixel%picWidth))
        pixely=$((error_pixel/picWidth))
        echo -n "YUV differs in picture=$failing_picture pixel=$error_pixel ($pixelx,$pixely). "
        return 1
    else
        echo -n "OK "
        return 0
    fi
}

removeBaseAddress()
{
    # Remove base addresses and other registers that will not match between
    # system model and test run from SW/HW register trace file
    # param1 = register trace file
    # param2 = jpeg check, remove quant tables

    if [ ! -e $1 ]
    then
        return 1
    fi

    # mb= not printed by system model
    grep -e "mb=" -v $1 > foo.tmp; mv -f foo.tmp $1
    # ASIC ID not present on system model
    grep -e "000/" -v $1 > foo.tmp; mv -f foo.tmp $1
    # Swap bits don't match
    grep -e "008/" -v $1 > foo.tmp; mv -f foo.tmp $1
    grep -e "00c/" -v $1 > foo.tmp; mv -f foo.tmp $1
    # Base addresses
    grep -e "014/" -v $1 > foo.tmp; mv -f foo.tmp $1
    grep -e "018/" -v $1 > foo.tmp; mv -f foo.tmp $1
    grep -e "01c/" -v $1 > foo.tmp; mv -f foo.tmp $1
    grep -e "020/" -v $1 > foo.tmp; mv -f foo.tmp $1
    grep -e "024/" -v $1 > foo.tmp; mv -f foo.tmp $1
    grep -e "028/" -v $1 > foo.tmp; mv -f foo.tmp $1
    grep -e "02c/" -v $1 > foo.tmp; mv -f foo.tmp $1
    grep -e "030/" -v $1 > foo.tmp; mv -f foo.tmp $1
    grep -e "034/" -v $1 > foo.tmp; mv -f foo.tmp $1
    grep -e "09c/" -v $1 > foo.tmp; mv -f foo.tmp $1
    grep -e "0cc/" -v $1 > foo.tmp; mv -f foo.tmp $1
    grep -e "0d0/" -v $1 > foo.tmp; mv -f foo.tmp $1
    # VP8 only bases, should be checked for h264
    grep -e "040/" -v $1 > foo.tmp; mv -f foo.tmp $1
    grep -e "044/" -v $1 > foo.tmp; mv -f foo.tmp $1
    grep -e "068/" -v $1 > foo.tmp; mv -f foo.tmp $1
    grep -e "0e8/" -v $1 > foo.tmp; mv -f foo.tmp $1
    grep -e "0ec/" -v $1 > foo.tmp; mv -f foo.tmp $1
    grep -e "11c/" -v $1 > foo.tmp; mv -f foo.tmp $1
    grep -e "39c/" -v $1 > foo.tmp; mv -f foo.tmp $1
    grep -e "3d0/" -v $1 > foo.tmp; mv -f foo.tmp $1
    grep -e "3d4/" -v $1 > foo.tmp; mv -f foo.tmp $1
    # RLC word count differs in VP8
    grep -e "094/" -v $1 > foo.tmp; mv -f foo.tmp $1
    # Stream header remainder differs in VP8
    grep -e "058/" -v $1 > foo.tmp; mv -f foo.tmp $1
    # Stream buffer size
    grep -e "060/" -v $1 > foo.tmp; mv -f foo.tmp $1
    # Config register
    grep -e "0fc/" -v $1 > foo.tmp; mv -f foo.tmp $1
    # VP8 deadzone registers, not written for H.264
    grep -e " 0000029" -v $1 > foo.tmp; mv -f foo.tmp $1
    grep -e " 000002a" -v $1 > foo.tmp; mv -f foo.tmp $1
    grep -e " 000002b" -v $1 > foo.tmp; mv -f foo.tmp $1
    grep -e " 000002c" -v $1 > foo.tmp; mv -f foo.tmp $1
    grep -e " 000002d" -v $1 > foo.tmp; mv -f foo.tmp $1
    grep -e " 000002e" -v $1 > foo.tmp; mv -f foo.tmp $1
    grep -e " 000002f" -v $1 > foo.tmp; mv -f foo.tmp $1
    grep -e " 0000030" -v $1 > foo.tmp; mv -f foo.tmp $1
    grep -e " 0000031" -v $1 > foo.tmp; mv -f foo.tmp $1
    grep -e " 0000032" -v $1 > foo.tmp; mv -f foo.tmp $1
    grep -e " 0000033" -v $1 > foo.tmp; mv -f foo.tmp $1
    grep -e " 0000034" -v $1 > foo.tmp; mv -f foo.tmp $1
    grep -e " 0000035" -v $1 > foo.tmp; mv -f foo.tmp $1
    grep -e " 0000036" -v $1 > foo.tmp; mv -f foo.tmp $1
    grep -e " 0000037" -v $1 > foo.tmp; mv -f foo.tmp $1
    grep -e " 0000038" -v $1 > foo.tmp; mv -f foo.tmp $1
    grep -e " 0000039" -v $1 > foo.tmp; mv -f foo.tmp $1

    # Remove only if jpeg flag == 1
    if [ "$2" == "1" ]
    then
        # Jpeg quant table regs 0x100-0x17c not possible to read from ASIC
        grep -e " 000001" -v $1 > foo.tmp; mv -f foo.tmp $1
    fi

}
