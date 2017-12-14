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
#--   Abstract     : Script functions for checking the results the SW/HW      --
#--                  verification test cases                                  --
#--                                                                           --
#-------------------------------------------------------------------------------

#=====--------------------------------------------------------------------=====#

# imports
. ./config.sh
. ./f_utils.sh "check"
. $TEST_CASE_LIST_CHECK
if [ "$TB_TEST_DB" == "ENABLED" ]
then
    . ./f_testdb.sh
fi

# extra information for the failing case.
comment=""

#-------------------------------------------------------------------------------
#-                                                                            --
#- Gets the status date of the test case result.                              --
#-                                                                            --
#- Parameters                                                                 --
#- 1 : Test case number                                                       --
#-                                                                            --
#- Exports                                                                    --
#- timeform1 : The date part of the status                                    --                                                                           
#- timeform2 : The time part of the status                                    --
#-                                                                            --
#-------------------------------------------------------------------------------
getStatusDate()
{
    case_number=$1
    #timeform1=`ls -l --time-style=full-iso case_${case_number}${TB_LOG_FILE_SUFFIX} 2> /dev/null | awk '{print $6}'`
    #timeform2=`ls -l --time-style=full-iso case_${case_number}${TB_LOG_FILE_SUFFIX} 2> /dev/null | awk '{print $7}'`
    timeform1=`date +%Y-%m-%d`
    timeform2=`date +%T`
    export timeform1
    export timeform2
}

#-------------------------------------------------------------------------------
#-                                                                            --
#- Gets the configuration string of the test case.                            --
#-                                                                            --
#- Parameters                                                                 --
#- 1 : Test case number                                                       --
#-                                                                            --
#- Exports                                                                    --
#- configuration : the configuration string                                   --
#-                                                                            --
#-------------------------------------------------------------------------------
getConfiguration()
{
    case_number=$1
    configuration=`grep -w "Configuration" case_${case_number}${TB_LOG_FILE_SUFFIX} 2> /dev/null | awk -F : '{print $2}'`
    export configuration
}

#-------------------------------------------------------------------------------
#-                                                                            --
#- Gets HW performance metrics of the test case.                              --
#-                                                                            --
#- Parameters                                                                 --
#- 1 : Test case number                                                       --
#-                                                                            --
#- Exports                                                                    --
#- hw_performance : HW performance metrics                                    --
#-                                                                            --
#-------------------------------------------------------------------------------
getHwPerformance()
{
    case_number=$1
    
    hw_performance=
    if [ ! -e case_${case_number}_${HW_PERFORMANCE_LOG} ]
    then
        hw_performance="N/A;N/A"
    else    
        # parse the performance file
        dec_total_time=`grep "dec" case_${case_number}_${HW_PERFORMANCE_LOG} 2> /dev/null | awk -F \; '{print $3}'`
        dec_frames=`grep "dec" case_${case_number}_${HW_PERFORMANCE_LOG} 2> /dev/null | awk -F \; '{print $2}'`
        pp_total_time=`grep "pp" case_${case_number}_${HW_PERFORMANCE_LOG} 2> /dev/null | awk -F \; '{print $3}'`
        pp_frames=`grep "pp" case_${case_number}_${HW_PERFORMANCE_LOG} 2> /dev/null | awk -F \; '{print $2}'`
    
        # calculate the hw performance
        hw_performance_dec="N/A"
        hw_performance_pp="N/A"
        
        # decoder performance
        if [ ! -z $dec_total_time ] && [ ${dec_frames} != 0 ]
        then
            hw_performance_dec=`echo "scale=2; ${dec_total_time}/${dec_frames}" | bc`
        fi
        
        # pp performance
        if [ ! -z $pp_total_time ] && [ ${pp_frames} != 0 ]
        then
            hw_performance_pp=`echo "scale=2; ${pp_total_time}/${pp_frames}" | bc`
        fi
        hw_performance="${hw_performance_dec};${hw_performance_pp}"
    fi
    export hw_performance
}

#-------------------------------------------------------------------------------
#-                                                                            --
#- Gets SW performance metrics of the test case.                              --
#-                                                                            --
#- Parameters                                                                 --
#- 1 : Test case number                                                       --
#-                                                                            --
#- Exports                                                                    --
#- sw_performance : SW performance metrics                                    --
#-                                                                            --
#-------------------------------------------------------------------------------
getSwPerformance()
{
    case_number=$1
    
    dec_performance=`grep -w "SW_PERFORMANCE" case_${case_number}${TB_LOG_FILE_SUFFIX} 2> /dev/null | awk '{print $2}'`
    pp_perf=`grep -w "SW_PERFORMANCE_PP" case_${case_number}${TB_LOG_FILE_SUFFIX} 2> /dev/null | awk '{print $2}'`
    
    sw_performance=
    if [ -z "$dec_performance" ] && [ -z "$pp_perf" ]
    then
        sw_performance="N/A;N/A"
        
    else
        sw_performance_dec="N/A"
        sw_performance_pp="N/A"
        if [ ! -z "$dec_performance" ]
        then
            sw_performance_dec="$dec_performance"
        fi
    
        if [ ! -z "$pp_perf" ]
        then
            sw_performance_pp="$pp_perf"
        fi
    
        sw_performance="${sw_performance_dec};${sw_performance_pp}"
    fi
    export sw_performance
}

#-------------------------------------------------------------------------------
#-                                                                            --
#- Gets the execution time of the test case.                                  --
#-                                                                            --
#- Parameters                                                                 --
#- 1 : Test case number                                                       --
#-                                                                            --
#- Exports                                                                    --
#- execution_time : Test case execution time                                  --
#-                                                                            --
#-------------------------------------------------------------------------------
getExecutionTime()
{
    case_number=$1
    
    execution_time="N/A"
    if [ -e "case_${case_number}.time" ]
    then
        #start_time=`grep START case_${case_number}.time | awk -F : '{print $2}'`
        #end_time=`grep END case_${case_number}.time | awk -F : '{print $2}'`
        #let 'execution_time=end_time-start_time'
        execution_time=`cat case_${case_number}.time`
    fi
    export execution_time
}

#-------------------------------------------------------------------------------
#-                                                                            --
#- Determines whether the test case is valid for the current configuration    --
#- (post-processor maximum resolution).                                       --
#-                                                                            --
#- Parameters                                                                 --
#- 1 : Test case number                                                       --
#-                                                                            --
#- Returns                                                                    --
#- 1 : Test case is not supported by the configuration                        --
#- 0 : Otherwise                                                              --
#-                                                                            --
#-------------------------------------------------------------------------------
checkPpMaxOutput()
{
    case_number=$1
    
    max_width=$PP_MAX_OUTPUT
    #max_height=$max_width
    
    #heights=`cat ${TEST_DATA_HOME_CHECK}/case_${case_number}/picture_ctrl_pp.trc | grep scaledHeight | awk '{print $1}'`
    #for height in $heights
    #do
    #    if [ $height -gt $max_height ]
    #    then
    #         return 1
    #    fi
    #done
     
    widths=`cat ${TEST_DATA_HOME_CHECK}/case_${case_number}/picture_ctrl_pp.trc | grep "scaled width" | awk '{print $1}'`
    if [ -z "$widths" ]
    then
        echo "Cannot determine picture width; assume supported case"
        return 0
    fi
    
    for width in $widths
    do
        if [ $width -gt $max_width ]
        then
             return 1
        fi
    done
    # size is in range
    return 0
}

#-------------------------------------------------------------------------------
#-                                                                            --
#- Determines whether the test case is valid for the current configuration    --
#- (decoder maximum resolution).                                              --
#-                                                                            --
#- Parameters                                                                 --
#- 1 : Test case number                                                       --
#-                                                                            --
#- Returns                                                                    --
#- 1 : Test case is not supported by the configuration                        --
#- 0 : Otherwise                                                              --
#-                                                                            --
#-------------------------------------------------------------------------------
checkDecMaxOutput()
{
    case_number=$1
    
    max_width=$DEC_MAX_OUTPUT
    #max_height=$max_width
     
    #heights=`cat ${TEST_DATA_HOME_CHECK}/case_${case_number}/picture_ctrl_dec.trc | grep -i "picture height" | awk '{print $1}'`
    #for height in $heights
    #do
    #    # Convert to pixel size
    #    let 'height=height*16'
    #    if [ $height -gt $max_height ]
    #    then
    #         return 1
    #    fi
    #done
    
    widths=`cat ${TEST_DATA_HOME_CHECK}/case_${case_number}/picture_ctrl_dec.trc | grep -i "picture width" | awk '{print $1}'`
    if [ -z "$widths" ]
    then
        echo "Cannot determine picture width; assume supported case"
        return 0
    fi
    
    for width in $widths
    do
        # Convert to pixel size
        let 'width=width*16'
        if [ $width -gt $max_width ]
        then
            return 1
        fi
    done
    # size is in range
    return 0
}

#-------------------------------------------------------------------------------
#-                                                                            --
#- Determines whether the test case is valid for the current configuration    --
#- (alpha blending).                                                          --
#-                                                                            --
#- Parameters                                                                 --
#- 1 : Test case number                                                       --
#-                                                                            --
#- Returns                                                                    --
#- 1 : Test case is not supported by the configuration                        --
#- 0 : Otherwise                                                              --
#-                                                                            --
#-------------------------------------------------------------------------------
checkPpAlphaBlending()
{
    case_number=$1

    getCategory "$case_number"
    present=`echo "$category" | grep "ablend"`
    if [ ! -z "$present" ] && [ "$PP_ALPHA_BLENDING" == "DISABLED" ]
    then
        return 1
    fi
    return 0
}

#-------------------------------------------------------------------------------
#-                                                                            --
#- Determines whether the test case is valid for the current configuration    --
#- (scaling).                                                                 --
#-                                                                            --
#- Parameters                                                                 --
#- 1 : Test case number                                                       --
#-                                                                            --
#- Returns                                                                    --
#- 1 : Test case is not supported by the configuration                        --
#- 0 : Otherwise                                                              --
#-                                                                            --
#-------------------------------------------------------------------------------
checkPpScaling()
{
    case_number=$1
    
    getCategory "$case_number"
    present=`echo "$category" | grep "scaling"`
    if [ ! -z "$present" ] && [ "$PP_SCALING" == "DISABLED" ]
    then
        return 1
    fi
    return 0
}

#-------------------------------------------------------------------------------
#-                                                                            --
#- Determines whether the test case is valid for the current configuration    --
#- (de-interlacing).                                                          --
#-                                                                            --
#- Parameters                                                                 --
#- 1 : Test case number                                                       --
#-                                                                            --
#- Returns                                                                    --
#- 1 : Test case is not supported by the configuration                        --
#- 0 : Otherwise                                                              --
#-                                                                            --
#-------------------------------------------------------------------------------
checkPpDeinterlacing()
{
    case_number=$1
    
    getCategory "$case_number"
    present=`echo "$category" | grep "deint"`
    if [ ! -z "$present" ] && [ "$PP_DEINTERLACING" == "DISABLED" ]
    then
        return 1
    fi
    return 0
}

#-------------------------------------------------------------------------------
#-                                                                            --
#- Determines whether the test case is valid for the current configuration    --
#- (de-interlacing).                                                          --
#-                                                                            --
#- Parameters                                                                 --
#- 1 : Test case number                                                       --
#-                                                                            --
#- Returns                                                                    --
#- 1 : Test case is not supported by the configuration                        --
#- 0 : Otherwise                                                              --
#-                                                                            --
#-------------------------------------------------------------------------------
checkPpDithering()
{
    case_number=$1
    
    getCategory "$case_number"
    present=`echo "$category" | grep "dithering"`
    if [ ! -z "$present" ] && [ "$PP_DITHERING" == "DISABLED" ]
    then
        return 1
    fi
    return 0
}

#-------------------------------------------------------------------------------
#-                                                                            --
#- Determines whether the case is valid for the current configuration in 8170 --
#- product. The cases that are non bit exact are treated also in the same way.--
#-                                                                            --
#- Parameters                                                                 --
#- 1 : Test case number                                                       --
#- 2 : The decoding format in question                                        --
#- 3 : Possible post-processor                                                --
#-                                                                            --
#- Returns                                                                    --
#- 1 : Test case is negative case                                             --
#- 0 : Otherwise                                                              --
#-                                                                            --
#-------------------------------------------------------------------------------
isNegativeCase8170()
{
    case_number=$1
    decoder=$2
    post_processor=$3
    
    getCategory "$case_number"
    
    if [ "$case_number" == 779 ]
    then
        return 1
    fi
    
    if [ "$case_number" == 5085 ]
    then
        return 1
    fi
    
    if [ "$case_number" == 4405 ] || [ "$case_number" == 4416 ] || [ "$case_number" == 4417 ]
    then
        return 1
    fi
    
    if [ $DEC_RLC_MODE_FORCED == "DISABLED" ]
    then
        if [ "$case_number" == 6000 ] || [ "$case_number" == 6001 ] || [ "$case_number" == 6042 ] || [ "$case_number" == 6043 ] || [ "$case_number" == 6044 ] || [ "$case_number" == 6045 ] || [ "$case_number" == 6046 ] || [ "$case_number" == 6047 ] || [ "$case_number" == 6048 ] || [ "$case_number" == 6049 ] || [ "$case_number" == 6050 ] || [ "$case_number" == 6051 ] || [ "$case_number" == 6052 ] || [ "$case_number" == 6053 ] || [ "$case_number" == 6054 ] || [ "$case_number" == 6068 ] || [ "$case_number" == 6069 ] || [ "$case_number" == 6070 ] || [ "$case_number" == 6071 ] || [ "$case_number" == 6072 ] || [ "$case_number" == 6086 ] || [ "$case_number" == 6087 ] || [ "$case_number" == 6088 ] || [ "$case_number" == 6089 ]
        then
            return 1
        fi
    fi
    
    if [ "$category" == "jpeg_420_error" ]
    then
        return 1
    fi
    
    if [ "$case_number" == 6575 ] || [ "$case_number" == 6578 ] || [ "$case_number" == 6581 ] || [ "$case_number" == 6582 ] || [ "$case_number" == 6587 ] || [ "$case_number" == 6589 ] || [ "$case_number" == 6592 ] || [ "$case_number" == 6593 ] || [ "$case_number" == 6595 ]
    then
        return 1
    fi
    
    if [ "$case_number" == 6601 ] && [ $DEC_RLC_MODE_FORCED == ENABLED ]
    then
        return 1
    fi
    
    if [ "$case_number" == 6601 ] && [ $TB_NAL_UNIT_STREAM == ENABLED ]
    then
        return 1
    fi
    
    if [ "$case_number" == 6601 ] && [ $TB_PACKET_BY_PACKET == DISABLED ]
    then
        return 1
    fi
    
    if [ "$post_processor" == "pp" ]
    then
        if [ ! -e "${TEST_DATA_HOME_CHECK}/case_${case_number}/picture_ctrl_pp.trc" ]
        then
             echo "Cannot find picture_ctrl_pp.trc; assume supported case"
             return 0
        fi
        
        # check PP resolution support
        checkPpMaxOutput "$case_number"
        if [ $? == 1 ]
        then
            return 1
        fi
        
        # check video resolution if pipelined case
        if [ $decoder != "none" ] && [ $decoder != "jpeg" ]
        then
            checkDecMaxOutput "$case_number"
            if [ $? == 1 ]
            then
                return 1
            fi
        fi
        
        # check alpha blending
        checkPpAlphaBlending "$case_number"
        if [ $? == 1 ]
        then
            return 1
        fi
        
        # check scaling
        checkPpScaling "$case_number"
        if [ $? == 1 ]
        then
            return 1
        fi
        
        # check deinterlacing
        checkPpDeinterlacing "$case_number"
        if [ $? == 1 ]
        then
            return 1
        fi
        
        # check dithering
        checkPpDithering "$case_number"
        if [ $? == 1 ]
        then
            return 1
        fi
        return 0
    else
        if [ ! -e "${TEST_DATA_HOME_CHECK}/case_${case_number}/picture_ctrl_dec.trc" ]
        then
             echo "Cannot find picture_ctrl_dec.trc; assume supported case"
             return 0
        fi
        # for JPEG, there is no configurable output resolution
        if [ "$decoder" != "jpeg" ]
        then
            # Check decoder resolution support
            checkDecMaxOutput "$case_number"
            return $?  
        fi
        return 0
    fi
}

#-------------------------------------------------------------------------------
#-                                                                            --
#- Determines whether the case is valid for the current configuration in 8190 --
#- product. The cases that are non bit exact are treated also in the same way.--
#-                                                                            --
#- Parameters                                                                 --
#- 1 : Test case number                                                       --
#- 2 : The decoding format in question                                        --
#- 3 : Possible post-processor                                                --
#-                                                                            --
#- Returns                                                                    --
#- 1 : Test case is negative case                                             --
#- 0 : Otherwise                                                              --
#-                                                                            --
#-------------------------------------------------------------------------------
isNegativeCase8190()
{
    case_number=$1
    decoder=$2
    post_processor=$3
    
    getCategory "$case_number"
    
    if [ "$case_number" == 10057 ] || [ "$case_number" == 10058 ]
    then
        return 1
    fi
    
    if [ "$case_number" == 779 ]
    then
        return 1
    fi
    
    if [ "$category" == "jpeg_420_error" ] || [ "$category" == "jpeg_444_error" ] || [ "$category" == "jpeg_411_error" ] 
    then
        return 1
    fi
    
    if [ "$case_number" == 4405 ] || [ "$case_number" == 4416 ] || [ "$case_number" == 4417 ]
    then
        return 1
    fi
    
    if [ "$case_number" == 6575 ] || [ "$case_number" == 6581 ] || [ "$case_number" == 6582 ] || [ "$case_number" == 6587 ] || [ "$case_number" == 6589 ] || [ "$case_number" == 6592 ] || [ "$case_number" == 6593 ] || [ "$case_number" == 6595 ]
    then
        return 1
    fi
    
    if [ "$category" == "h264_hp_cavlc_ilaced_error_sw" ] || [ "$category" == "h264_hp_cabac_ilaced_error_sw" ] || [ "$category" == "h264_hp_cavlc_error_sw" ]  || [ "$category" == "h264_hp_cabac_error_sw" ]
    then
        return 1
    fi
    
    if [ "$case_number" == 9006 ]
    then
        baseline_configuration=`echo "$HWCONFIGTAG" | grep "h264bp"`
        if [ ! -z "$baseline_configuration" ]
        then
            return 1
        fi
    fi
    
    
    getCategory "$case_number"
    webp_category=`echo $category | grep webp`
    
    if [ "$post_processor" == "pp" ]
    then
        if [ ! -e "${TEST_DATA_HOME_CHECK}/case_${case_number}/picture_ctrl_pp.trc" ]
        then
             echo "Cannot find picture_ctrl_pp.trc; assume supported case"
             return 0
        fi
        # check PP resolution support
        checkPpMaxOutput "$case_number"
        if [ $? == 1 ]
        then
            return 1
        fi
        # check video resolution if pipelined case
        if [ $decoder != "none" ] && [ $decoder != "jpeg" ] && [ -z "$webp_category" ]
        then
            checkDecMaxOutput "$case_number"
            if [ $? == 1 ]
            then
                return 1
            fi
        fi
        # check alpha blending
        checkPpAlphaBlending "$case_number"
        if [ $? == 1 ]
        then
            return 1
        fi
        # check scaling
        checkPpScaling "$case_number"
        if [ $? == 1 ]
        then
            return 1
        fi
        # check deinterlacing
        checkPpDeinterlacing "$case_number"
        if [ $? == 1 ]
        then
            return 1
        fi
        # check dithering
        checkPpDithering "$case_number"
        if [ $? == 1 ]
        then
            return 1
        fi
        return 0
    else
        if [ ! -e "${TEST_DATA_HOME_CHECK}/case_${case_number}/picture_ctrl_dec.trc" ]
        then
             echo "Cannot find picture_ctrl_dec.trc; assume supported case"
             return 0
        fi
        # for JPEG/Webp, there is no configurable output resolution
        if [ "$decoder" != "jpeg" ] && [ -z "$webp_category" ]
        then
            # Check decoder resolution support
            checkDecMaxOutput "$case_number"
            return $?  
        fi
        return 0
    fi
}

#-------------------------------------------------------------------------------
#-                                                                            --
#- Checks if the test case was crashed.                                       --
#-                                                                            --
#- Parameters                                                                 --
#- 1 : Test case number                                                       --
#-                                                                            --
#- Returns                                                                    --
#- 1 : Test case was crashed                                                  --
#- 0 : Otherwise                                                              --
#-                                                                            --
#-------------------------------------------------------------------------------
isCrashed()
{
    case_number=$1
     
    crash=`ls case_${case_number}.crash 2> /dev/null`
    if [ -z ${crash} ]
    then
        return 0
    else
        return 1
    fi
}

#-------------------------------------------------------------------------------
#-                                                                            --
#- Checks if the test case was timed out by HW.                               --
#-                                                                            --
#- Parameters                                                                 --
#- 1 : Test case number                                                       --
#-                                                                            --
#- Returns                                                                    --
#- 1 : Test case was timed out                                                --
#- 0 : Otherwise                                                              --
#-                                                                            --
#-------------------------------------------------------------------------------
isHWTimeout()
{
    case_number=$1
    
    # timeout status is read from log file
    timeout=`grep -m 1 TIMEOUT case_${case_number}${TB_LOG_FILE_SUFFIX} 2> /dev/null`
    if [ ! -z "$timeout" ]
    then
        return 1
    else
        return 0
    fi
}

#-------------------------------------------------------------------------------
#-                                                                            --
#- Checks if the test case was timed out by test bench.                       --
#-                                                                            --
#- Parameters                                                                 --
#- 1 : Test case number                                                       --
#-                                                                            --
#- Returns                                                                    --
#- 1 : Test case was timed out                                                --
#- 0 : Otherwise                                                              --
#-                                                                            --
#-------------------------------------------------------------------------------
isTBTimeout()
{
    case_number=$1
    
    # timeout is set by the watchdog script
    timeout=`ls case_${case_number}.timeout 2> /dev/null`
    if [ -z ${timeout} ]
    then
        return 0
    else
        return 1
    fi
}

#-------------------------------------------------------------------------------
#-                                                                            --
#- Checks if not enough space was left.                                       --
#-                                                                            --
#- Parameters                                                                 --
#- 1 : Test case number                                                       --
#-                                                                            --
#- Returns                                                                    --
#- 1 : Out of space                                                           --
#- 0 : Otherwise                                                              --
#-                                                                            --
#-------------------------------------------------------------------------------
isOutOfSpace()
{
    case_number=$1
     
    out_of_space=`ls case_${case_number}.out_of_space 2> /dev/null`
    if [ -z ${out_of_space} ]
    then
        return 0
    else
        return 1
    fi
}

#-------------------------------------------------------------------------------
#-                                                                            --
#- Prints out and stroes the test case status and comment.                    --
#-                                                                            --
#- Parameters                                                                 --
#- 1 : Test case number                                                       --
#- 2 : Test case status                                                       --
#- 3 : Test case comment                                                      --
#-                                                                            --
#-------------------------------------------------------------------------------
result2ScreenAndFile()
{
    case_number=$1
    status=$2
    comment=$3
    
    if [ -e case_${case_number}.time_limit ] && [ -z "$comment" ]
    then
        let 'pics=0'
        if [ -e case_${case_number}_raster_scan.yuv ]
        then
            file="case_${case_number}_raster_scan.yuv"
        elif [ -e case_${case_number}.yuv ]
        then
            file="case_${case_number}.yuv"
        elif [ -e case_${case_number}.rgb ]
        then
            file="case_${case_number}.rgb"
        fi
        if [ "$TB_MD5SUM" == "ENABLED" ]
        then
            pics=`cat ${file} | nl | tac | head -n 1 | awk '{print $1}'`
            
        elif [ "$TB_MD5SUM" == "DISABLED" ]
        then
            pic_width=`cat ${TEST_DATA_HOME_CHECK}/case_${case_number}/picture_ctrl_dec.trc | grep -m 1 -i "picture width" | awk '{print $1}' 2> /dev/null`
            pic_height=`cat ${TEST_DATA_HOME_CHECK}/case_${case_number}/picture_ctrl_dec.trc | grep -m 1 -i "picture height" | awk '{print $1}' 2> /dev/null`
            let 'pic_width*=16'
            let 'pic_height*=16'
    
            if [ -z "$pic_width" ] || [ -z "$pic_height" ] || [ "$pic_width" -eq 0 ] || [ "$pic_height" -eq 0 ] || [ "$file" != "case_${case_number}_raster_scan.yuv" ]
            then
                comment="Case not fully decoded"
            else
                file_size=`ls -la ${file} | awk '{print $5}'`
                let 'pixel_in_pic=pic_width*pic_height*15'
                let 'file_size=file_size*10'
                let 'pics=file_size/pixel_in_pic'
                let 'extra_picture=file_size%pixel_in_pic'
                if [ "$extra_picture" != 0 ]
                then
                    let 'pics+=1'
                fi
                comment="Case not fully decoded, ${pics} pictures written"
            fi
        fi
    fi
    
    if [ -z "$comment" ]
    then
        echo "${status}"
        echo "${case_number} : " >> ${COMMENT_FILE}
    else
        echo "${status} (${comment})"
        echo "${case_number} : ${comment}" >> ${COMMENT_FILE}
    fi
}

#-------------------------------------------------------------------------------
#-                                                                            --
#- Determines the number of the failing picture (assumes 4:2:0 output         --
#- format).                                                                   --
#-                                                                            --
#- Parameters                                                                 --
#- 1 : Test case number                                                       --
#- 2 : Decoder output data                                                    --
#- 3 : Reference data                                                         --
#-                                                                            --
#- Exports                                                                    --
#- failing_picture : The failing picture                                      --
#-                                                                            --
#- Returns                                                                    --
#- 1 : Picture dimension was not written                                      --
#- 0 : Otherwise                                                              --
#-                                                                            --
#-------------------------------------------------------------------------------
findFailingPicture()
{
    # find the failing picture
    # parse the height and width of the current case
    pic_width=`cat ${TEST_DATA_HOME_CHECK}/case_${case_number}/picture_ctrl_dec.trc | grep -m 1 -i "picture width" | awk '{print $1}' 2> /dev/null`
    pic_height=`cat ${TEST_DATA_HOME_CHECK}/case_${case_number}/picture_ctrl_dec.trc | grep -m 1 -i "picture height" | awk '{print $1}' 2> /dev/null`
    let 'pic_width*=16'
    let 'pic_height*=16'
    
    # if picture dimension not written
    if [ -z "$pic_width" ] || [ -z "$pic_height" ] || [ "$pic_width" -eq 0 ] || [ "$pic_height" -eq 0 ]
    then
        return 1
    fi
    
    # calculate the failing picture using the picture width and height
    # parse first differing byte
    error_pixel_number=`cmp $2 $3 | awk '{print $5}' | awk 'BEGIN {FS=","} {print $1}'`
    
    # the multiplier should be 1,5 (multiply the values by 10)
    let 'pixel_in_pic=pic_width*pic_height*15'
    let 'error_pixel_number=error_pixel_number*10'
    let 'failing_picture=error_pixel_number/pixel_in_pic'
    
    export failing_picture
    return 0
}

#-------------------------------------------------------------------------------
#-                                                                            --
#- Determines the number of the failing picture when decoder output file is   -- 
#- not fully written (assumes 4:2:0 output format).                           --
#-                                                                            --
#- Parameters                                                                 --
#- 1 : Test case number                                                       --
#- 2 : Decoder output data                                                    --
#- 3 : Reference data                                                         --
#-                                                                            --
#- Exports                                                                    --
#- failing_picture : The failing picture                                      --
#-                                                                            --
#- Returns                                                                    --
#- 1 : Picture dimension was not written                                      --
#- 0 : Otherwise                                                              --
#-                                                                            --
#-------------------------------------------------------------------------------
findFailingPictureWhenNotFullyDecoded()
{
    # find the failing picture
    # parse the height and width of the current case
    pic_width=`cat ${TEST_DATA_HOME_CHECK}/case_${case_number}/picture_ctrl_dec.trc | grep -m 1 -i "picture width" | awk '{print $1}' 2> /dev/null`
    pic_height=`cat ${TEST_DATA_HOME_CHECK}/case_${case_number}/picture_ctrl_dec.trc | grep -m 1 -i "picture height" | awk '{print $1}' 2> /dev/null`
    let 'pic_width*=16'
    let 'pic_height*=16'
    
    error_pixel_number=$2
    
    # if picture dimension not written
    if [ -z "$pic_width" ] || [ -z "$pic_height" ] || [ "$pic_width" -eq 0 ] || [ "$pic_height" -eq 0 ]
    then
        return 1
    fi
    
    # calculate the failing picture using the picture width and height
    # the multiplier should be 1,5 (multiply the values by 10)
    let 'pixel_in_pic=pic_width*pic_height*15'
    let 'error_pixel_number=error_pixel_number*10'
    let 'failing_picture=error_pixel_number/pixel_in_pic'
    
    export failing_picture
    return 0
}

#-------------------------------------------------------------------------------
#-                                                                            --
#- Makes the system model and versatile SW register traces comparable.        --
#-                                                                            --
#- Parameters                                                                 --
#- 1 : Tracefile to be modified                                               --
#-                                                                            --
#-------------------------------------------------------------------------------
modSwRegTrace()
{
    sw_reg_trace_file=$1
    
    awk -v support_end_addr=$2 ' {
    start_addr=0
    start_addr_ctrl=0
    start_bit=0
    print_int_status=0
    print_stream_consumed=0
    print_stream_consumed_ctrl=0
    print_hit_sum=0
    print_intra_sum=0
    print_y_mv_sum=0
    print_top_sum=0
    print_bottom_sum=0
    print_pp_int_status=0
    while ((getline line) > 0)
    {
        #print "ARHG: ", line
        if (line ~ /\<W\>/ && line ~ /\<STREAM START ADDRESS\>/)
        {
            if (line !~ /\<RLC\>/ && line !~ /\<VP8\>/)
            {
                split(line, start_addr_arr, ":")
                if (line !~ /\<CTRL\>/)
                {
                    start_addr=strtonum(start_addr_arr[2])
                }
                else
                {
                    start_addr_ctrl=strtonum(start_addr_arr[2])
                }
            }
        }
        else if (line ~ /\<R\>/ && line ~ /\<INTERRUPT STATUS\>/ && line !~ /\<PICTURE DECODED\>/ && line !~ /\<PP\>/)
        {
            swregs[1]=line
            print_int_status=1
            # print return values only if picture decoded
            print_hit_sum=0
            print_intra_sum=0
            print_y_mv_sum=0
            print_top_sum=0
            print_bottom_sum=0
        }
        else if (line ~ /\<R\>/ && line ~ /\<INTERRUPT STATUS\>/ && line ~ /\<PICTURE DECODED\>/)
        {
            swregs[1]=line
            print_int_status=1
            # print return values only if picture decoded
            print_hit_sum=1
            print_intra_sum=1
            print_y_mv_sum=1
            print_top_sum=1
            print_bottom_sum=1
        }
        else if (line ~ /\<R\>/ && line ~ /\<PP\>/ && line ~ /\<INTERRUPT STATUS\>/ && line ~ /\<PICTURE PROCESSED\>/)
        {
            swregs[8]=line
            print_pp_int_status=1
        }
        else if (line ~ /\<R\>/ && line ~ /\<STREAM END ADDRESS\>/)
        {
            split(line, end_addr_arr, ":")
            # If stream end address
            if (line !~ /\<ERROR\>/ && line !~ /\<ASO\>/ && line !~ /\<RLC\>/ && line !~ /\<TIMEOUT\>/ && line !~ /\<SLICE\>/)
            {
                end_addr=strtonum(end_addr_arr[2])
                if (line !~ /\<CTRL\>/)
                {
                    # No need to take account the start bit in stream consuption calculation
                    stream_consumed=end_addr-start_addr
                    swregs[2]=sprintf("R STREAM CONSUMED: %d", stream_consumed)
                    print_stream_consumed=1
                }
                else
                {
                    if (line !~ /\<VP8\>/)
                    {
                        end_addr_ctrl=strtonum(end_addr_arr[2])
                        # No need to take account the start bit in stream consuption calculation
                        stream_consumed_ctrl=end_addr_ctrl-start_addr_ctrl
                        swregs[9]=sprintf("R CTRL STREAM CONSUMED: %d", stream_consumed_ctrl)
                        print_stream_consumed_ctrl=1
                    }
                    else
                    {
                        print_stream_consumed_ctrl=0
                    }
                }
            }
            else
            {
                print_stream_consumed=0
                print_stream_consumed_ctrl=0
            }
        }
        else if (line ~ /\<R\>/ && line ~ /\<REFERENCE BUFFER HIT SUM\>/)
        {
            swregs[3]=line
            #print_hit_sum=1
            
        }
        else if (line ~ /\<R\>/ && line ~ /\<REFERENCE BUFFER INTRA SUM\>/)
        {
            swregs[4]=line
            #print_intra_sum=1
        }
        else if (line ~ /\<R\>/ && line ~ /\<REFERENCE BUFFER Y_MV SUM\>/)
        {
            swregs[5]=line
            #print_y_mv_sum=1
        }
        else if (line ~ /\<R\>/ && line ~ /\<REFERENCE BUFFER TOP SUM\>/)
        {
            swregs[6]=line
        }
        else if (line ~ /\<R\>/ && line ~ /\<REFERENCE BUFFER BOTTOM SUM\>/)
        {
            swregs[7]=line
        }
        else if (line ~ /-/)
        {
            #print "-"
            if (print_int_status)
                print swregs[1]
            if (print_stream_consumed && support_end_addr)
                print swregs[2]
            if (print_stream_consumed_ctrl && support_end_addr)
                print swregs[9]
            if (print_hit_sum)
                print swregs[3]
            if (print_intra_sum)
                print swregs[4]
            if (print_y_mv_sum)
                print swregs[5]
            if (print_top_sum)
                print swregs[6]
            if (print_bottom_sum)
                print swregs[7]
            if (print_pp_int_status)
                print swregs[8]
                
            start_addr=0
            start_bit=0
            print_int_status=0
            print_stream_consumed=0
            print_stream_consumed_ctrl=0
            print_hit_sum=0
            print_intra_sum=0
            print_y_mv_sum=0
            print_top_sum=0
            print_bottom_sum=0
            print_pp_int_status=0
        }
        else
        {
            print "UNKNOWN SW REGISTER TRACE: ", line
            exit
        }
    }
    #print "-"
    if (print_int_status)
        print swregs[1]
    if (print_stream_consumed && support_end_addr)
        print swregs[2]
    if (print_stream_consumed_ctrl  && support_end_addr)
        print swregs[9]
    if (print_hit_sum)
        print swregs[3]
    if (print_intra_sum)
        print swregs[4]
    if (print_y_mv_sum)
        print swregs[5]
    if (print_top_sum)
        print swregs[6]
    if (print_bottom_sum)
        print swregs[7]
    if (print_pp_int_status)
        print swregs[8]
    }' "$sw_reg_trace_file" > ${sw_reg_trace_file}.mod
}
#-------------------------------------------------------------------------------
#-                                                                            --
#- Some comparison are disabled for dec + pp decoding.                        --
#-                                                                            --
#- Parameters                                                                 --
#- 1 : Tracefile to be modified                                               --
#-                                                                            --
#-------------------------------------------------------------------------------
modSwRegTraceDecPp()
{
    sw_reg_trace_file=$1
    
    awk -v support_end_addr=$2 ' {
    start_addr=0
    start_addr_ctrl=0
    start_bit=0
    print_int_status=0
    print_stream_consumed=0
    print_stream_consumed_ctrl=0
    print_hit_sum=0
    print_intra_sum=0
    print_y_mv_sum=0
    print_top_sum=0
    print_bottom_sum=0
    print_pp_int_status=0
    while ((getline line) > 0)
    {
        #print "ARHG: ", line
        if (line ~ /\<W\>/ && line ~ /\<STREAM START ADDRESS\>/)
        {
            if (line !~ /\<RLC\>/ && line !~ /\<VP8\>/)
            {
                split(line, start_addr_arr, ":")
                if (line !~ /\<CTRL\>/)
                {
                    start_addr=strtonum(start_addr_arr[2])
                }
                else
                {
                    start_addr_ctrl=strtonum(start_addr_arr[2])
                }
            }
        }
        else if (line ~ /\<R\>/ && line ~ /\<INTERRUPT STATUS\>/ && line !~ /\<PICTURE DECODED\>/ && line !~ /\<PP\>/)
        {
            swregs[1]=line
            if (line ~ /\<R\>/ && line ~ /\<INTERRUPT STATUS\>/ && line !~ /\<BUFFER EMPTY\>/)
            {
                print_int_status=1
            }
            # Post-processed picture could be ready prior to all stream is processed
            else
            {
                print_int_status=0
            }
            # print return values only if picture decoded
            print_hit_sum=0
            print_intra_sum=0
            print_y_mv_sum=0
            print_top_sum=0
            print_bottom_sum=0
        }
        else if (line ~ /\<R\>/ && line ~ /\<INTERRUPT STATUS\>/ && line ~ /\<PICTURE DECODED\>/)
        {
            swregs[1]=line
            print_int_status=1
            # print return values only if picture decoded
            print_hit_sum=1
            print_intra_sum=1
            print_y_mv_sum=1
            print_top_sum=1
            print_bottom_sum=1
        }
        else if (line ~ /\<R\>/ && line ~ /\<PP\>/ && line ~ /\<INTERRUPT STATUS\>/ && line ~ /\<PICTURE PROCESSED\>/)
        {
            swregs[8]=line
            print_pp_int_status=1
        }
        else if (line ~ /\<R\>/ && line ~ /\<STREAM END ADDRESS\>/)
        {
            split(line, end_addr_arr, ":")
            # If stream end address
            if (line !~ /\<ERROR\>/ && line !~ /\<ASO\>/ && line !~ /\<RLC\>/ && line !~ /\<TIMEOUT\>/ && line !~ /\<SLICE\>/)
            {
                end_addr=strtonum(end_addr_arr[2])
                if (line !~ /\<CTRL\>/)
                {
                    # No need to take account the start bit in stream consuption calculation
                    stream_consumed=end_addr-start_addr
                    swregs[2]=sprintf("R STREAM CONSUMED: %d", stream_consumed)
                    print_stream_consumed=1
                }
                else
                {
                    if (line !~ /\<VP8\>/)
                    {
                        end_addr_ctrl=strtonum(end_addr_arr[2])
                        # No need to take account the start bit in stream consuption calculation
                        stream_consumed_ctrl=end_addr_ctrl-start_addr_ctrl
                        swregs[9]=sprintf("R CTRL STREAM CONSUMED: %d", stream_consumed_ctrl)
                        print_stream_consumed_ctrl=1
                    }
                    else
                    {
                        print_stream_consumed_ctrl=0
                    }
                }
            }
            else
            {
                print_stream_consumed=0
                print_stream_consumed_ctrl=0
            }
        }
        else if (line ~ /\<R\>/ && line ~ /\<REFERENCE BUFFER HIT SUM\>/)
        {
            swregs[3]=line
            #print_hit_sum=1
            
        }
        else if (line ~ /\<R\>/ && line ~ /\<REFERENCE BUFFER INTRA SUM\>/)
        {
            swregs[4]=line
            #print_intra_sum=1
        }
        else if (line ~ /\<R\>/ && line ~ /\<REFERENCE BUFFER Y_MV SUM\>/)
        {
            swregs[5]=line
            #print_y_mv_sum=1
        }
        else if (line ~ /\<R\>/ && line ~ /\<REFERENCE BUFFER TOP SUM\>/)
        {
            swregs[6]=line
        }
        else if (line ~ /\<R\>/ && line ~ /\<REFERENCE BUFFER BOTTOM SUM\>/)
        {
            swregs[7]=line
        }
        else if (line ~ /-/)
        {
            #print "-"
            if (print_int_status)
                print swregs[1]
            if (print_stream_consumed && support_end_addr)
                print swregs[2]
            if (print_stream_consumed_ctrl && support_end_addr)
                print swregs[9]
            if (print_hit_sum)
                print swregs[3]
            if (print_intra_sum)
                print swregs[4]
            if (print_y_mv_sum)
                print swregs[5]
            if (print_top_sum)
                print swregs[6]
            if (print_bottom_sum)
                print swregs[7]
            if (print_pp_int_status)
                print swregs[8]
                
            start_addr=0
            start_bit=0
            print_int_status=0
            print_stream_consumed=0
            print_stream_consumed_ctrl=0
            print_hit_sum=0
            print_intra_sum=0
            print_y_mv_sum=0
            print_top_sum=0
            print_bottom_sum=0
            print_pp_int_status=0
        }
        else
        {
            print "UNKNOWN SW REGISTER TRACE: ", line
            exit
        }
    }
    #print "-"
    if (print_int_status)
        print swregs[1]
    if (print_stream_consumed && support_end_addr)
        print swregs[2]
    if (print_stream_consumed_ctrl  && support_end_addr)
        print swregs[9]
    if (print_hit_sum)
        print swregs[3]
    if (print_intra_sum)
        print swregs[4]
    if (print_y_mv_sum)
        print swregs[5]
    if (print_top_sum)
        print swregs[6]
    if (print_bottom_sum)
        print swregs[7]
    if (print_pp_int_status)
        print swregs[8]
    }' "$sw_reg_trace_file" > ${sw_reg_trace_file}.mod
}

#-------------------------------------------------------------------------------
#-                                                                            --
#- Some comparison are disabled for jpeg decoding.                            --
#-                                                                            --
#- Parameters                                                                 --
#- 1 : Tracefile to be modified                                               --
#-                                                                            --
#-------------------------------------------------------------------------------
modSwRegTraceJpeg8170()
{
    sw_reg_trace_file=$1
    
    awk ' {
    start_addr=0
    start_bit=0
    print_int_status=0
    print_stream_consumed=0
    print_hit_sum=0
    print_intra_sum=0
    print_y_mv_sum=0
    print_top_sum=0
    print_bottom_sum=0
    print_pp_int_status=0
    while ((getline line) > 0)
    {
        #print "ARHG: ", line
        if (line ~ /\<W\>/ && line ~ /\<STREAM START ADDRESS\>/)
        {
            # If stream start address
            if (line !~ /\<RLC\>/)
            {
                split(line, start_addr_arr, ":")
                start_addr=strtonum(start_addr_arr[2])
            }
        }
        else if (line ~ /\<R\>/ && line ~ /\<INTERRUPT STATUS\>/ && line !~ /\<PP\>/)
        {
            swregs[1]=line
            print_int_status=1
        }
        else if (line ~ /\<R\>/ && line ~ /\<PP\>/ && line ~ /\<INTERRUPT STATUS\>/ && line ~ /\<PICTURE PROCESSED\>/)
        {
            swregs[8]=line
            print_pp_int_status=1
        }
        else if (line ~ /\<R\>/ && line ~ /\<STREAM END ADDRESS\>/)
        {
            split(line, end_addr_arr, ":")
            # If stream end address
            if (line !~ /\<ASO\>/ && line !~ /\<ERROR\>/ && line !~ /\<RLC\>/)
            {
                end_addr=strtonum(end_addr_arr[2])
                # No need to take account the start bit in stream consuption calculation
                stream_consumed=end_addr-start_addr
                swregs[2]=sprintf("R STREAM CONSUMED:             %d", stream_consumed)
                print_stream_consumed=1
            }
            else
            {
                #swregs[2]=sprintf("R STREAM CONSUMED:   %s", end_addr_arr[2])
                print_stream_consumed=0
            }
            #print_stream_consumed=1
        }
        else if (line ~ /\<R\>/ && line ~ /\<REFERENCE BUFFER HIT SUM\>/)
        {
            swregs[3]=line
            print_hit_sum=0
            
        }
        else if (line ~ /\<R\>/ && line ~ /\<REFERENCE BUFFER INTRA SUM\>/)
        {
            swregs[4]=line
            print_intra_sum=0
        }
        else if (line ~ /\<R\>/ && line ~ /\<REFERENCE BUFFER Y_MV SUM\>/)
        {
            swregs[5]=line
            print_y_mv_sum=0
        }
        else if (line ~ /\<R\>/ && line ~ /\<REFERENCE BUFFER TOP SUM\>/)
        {
            swregs[6]=line
            print_top_sum=0
        }
        else if (line ~ /\<R\>/ && line ~ /\<REFERENCE BUFFER BOTTOM SUM\>/)
        {
            swregs[7]=line
            print_bottom_sum=0
        }
        else if (line ~ /-/)
        {
            #print "-"
            if (print_int_status)
                print swregs[1]
            #if (print_stream_consumed)
            #    print swregs[2]
            if (print_hit_sum)
                print swregs[3]
            if (print_intra_sum)
                print swregs[4]
            if (print_y_mv_sum)
                print swregs[5]
            if (print_top_sum)
                print swregs[6]
            if (print_bottom_sum)
                print swregs[7]
            if (print_pp_int_status)
                print swregs[8]
                
            start_addr=0
            start_bit=0
            print_int_status=0
            print_stream_consumed=0
            print_hit_sum=0
            print_intra_sum=0
            print_y_mv_sum=0
            print_top_sum=0
            print_bottom_sum=0
            print_pp_int_status=0
        }
        else
        {
            print "UNKNOWN SW REGISTER TRACE: ", line
            exit
        }
    }
    #print "-"
    if (print_int_status)
        print swregs[1]
    #if (print_stream_consumed)
    #    print swregs[2]
    if (print_hit_sum)
        print swregs[3]
    if (print_intra_sum)
        print swregs[4]
    if (print_y_mv_sum)
        print swregs[5]
    if (print_top_sum)
        print swregs[6]
    if (print_bottom_sum)
        print swregs[7]
    if (print_pp_int_status)
        print swregs[8]
    }' "$sw_reg_trace_file" > ${sw_reg_trace_file}.mod
}

#-------------------------------------------------------------------------------
#-                                                                            --
#- Some comparison are disabled for jpeg (+ pp) decoding.                     --
#-                                                                            --
#- Parameters                                                                 --
#- 1 : Tracefile to be modified                                               --
#-                                                                            --
#-------------------------------------------------------------------------------
modSwRegTraceJpegPp8170()
{
    sw_reg_trace_file=$1
    
    awk ' {
    start_addr=0
    start_bit=0
    print_int_status=0
    print_stream_consumed=0
    print_hit_sum=0
    print_intra_sum=0
    print_y_mv_sum=0
    print_top_sum=0
    print_bottom_sum=0
    print_pp_int_status=0
    while ((getline line) > 0)
    {
        #print "ARHG: ", line
        if (line ~ /\<W\>/ && line ~ /\<STREAM START ADDRESS\>/)
        {
            # If stream start address
            if (line !~ /\<RLC\>/)
            {
                split(line, start_addr_arr, ":")
                start_addr=strtonum(start_addr_arr[2])
            }
        }
        else if (line ~ /\<R\>/ && line ~ /\<INTERRUPT STATUS\>/ && line !~ /\<PP\>/)
        {
            swregs[1]=line
            if (line ~ /\<R\>/ && line ~ /\<INTERRUPT STATUS\>/ && line !~ /\<BUFFER EMPTY\>/)
            {
                print_int_status=1
            }
            # Post-processed picture could be ready prior to all stream is processed
            else
            {
                print_int_status=0
            }
        }
        else if (line ~ /\<R\>/ && line ~ /\<PP\>/ && line ~ /\<INTERRUPT STATUS\>/ && line ~ /\<PICTURE PROCESSED\>/)
        {
            swregs[8]=line
            print_pp_int_status=1
        }
        else if (line ~ /\<R\>/ && line ~ /\<STREAM END ADDRESS\>/)
        {
            split(line, end_addr_arr, ":")
            # If stream end address
            if (line !~ /\<ASO\>/ && line !~ /\<ERROR\>/ && line !~ /\<RLC\>/)
            {
                end_addr=strtonum(end_addr_arr[2])
                # No need to take account the start bit in stream consuption calculation
                stream_consumed=end_addr-start_addr
                swregs[2]=sprintf("R STREAM CONSUMED:             %d", stream_consumed)
                print_stream_consumed=1
            }
            else
            {
                #swregs[2]=sprintf("R STREAM CONSUMED:   %s", end_addr_arr[2])
                print_stream_consumed=0
            }
            #print_stream_consumed=1
        }
        else if (line ~ /\<R\>/ && line ~ /\<REFERENCE BUFFER HIT SUM\>/)
        {
            swregs[3]=line
            print_hit_sum=0
            
        }
        else if (line ~ /\<R\>/ && line ~ /\<REFERENCE BUFFER INTRA SUM\>/)
        {
            swregs[4]=line
            print_intra_sum=0
        }
        else if (line ~ /\<R\>/ && line ~ /\<REFERENCE BUFFER Y_MV SUM\>/)
        {
            swregs[5]=line
            print_y_mv_sum=0
        }
        else if (line ~ /\<R\>/ && line ~ /\<REFERENCE BUFFER TOP SUM\>/)
        {
            swregs[6]=line
            print_top_sum=0
        }
        else if (line ~ /\<R\>/ && line ~ /\<REFERENCE BUFFER BOTTOM SUM\>/)
        {
            swregs[7]=line
            print_bottom_sum=0
        }
        else if (line ~ /-/)
        {
            #print "-"
            if (print_int_status)
                print swregs[1]
            #if (print_stream_consumed)
            #    print swregs[2]
            if (print_hit_sum)
                print swregs[3]
            if (print_intra_sum)
                print swregs[4]
            if (print_y_mv_sum)
                print swregs[5]
            if (print_top_sum)
                print swregs[6]
            if (print_bottom_sum)
                print swregs[7]
            if (print_pp_int_status)
                print swregs[8]
                
            start_addr=0
            start_bit=0
            print_int_status=0
            print_stream_consumed=0
            print_hit_sum=0
            print_intra_sum=0
            print_y_mv_sum=0
            print_top_sum=0
            print_bottom_sum=0
            print_pp_int_status=0
        }
        else
        {
            print "UNKNOWN SW REGISTER TRACE: ", line
            exit
        }
    }
    #print "-"
    if (print_int_status)
        print swregs[1]
    #if (print_stream_consumed)
    #    print swregs[2]
    if (print_hit_sum)
        print swregs[3]
    if (print_intra_sum)
        print swregs[4]
    if (print_y_mv_sum)
        print swregs[5]
    if (print_top_sum)
        print swregs[6]
    if (print_bottom_sum)
        print swregs[7]
    if (print_pp_int_status)
        print swregs[8]
    }' "$sw_reg_trace_file" > ${sw_reg_trace_file}.mod
}

#-------------------------------------------------------------------------------
#-                                                                            --
#- Compares the provided files (i.e., expected and actual outputs).           --
#-                                                                            --
#- Parameters                                                                 --
#- 1 : Decoder/post-processor output data                                     --
#- 2 : Reference data                                                         --
#- 3 : Flag to decide wheher to calculate the failing picture                 --
#-                                                                            --
#- Returns                                                                    --
#- 3:  Reference data missing                                                 -- 
#- 2:  Output data missing                                                    --
#- 1 : Output data does not match with reference data                         --
#- 0 : Otherwise                                                              --
#-                                                                            --
#-------------------------------------------------------------------------------
compare()
{
    decoder_out=$1
    ref_out=$2
    no_failing_picture=$3
    case_number=$4
    decoder=$5
    post_processor=$6
    
    getCategory $case_number
    error_category=`echo $category | grep _error`
    webp_category=`echo $category | grep webp`
    vp8_category=`echo $category | grep vp8`
    vp7_category=`echo $category | grep vp7`
    
    # HOX!
    # Allow * in file names; do not use ""
    
    # add path to reference output put
    if [ "$TB_TEST_DATA_SOURCE" == "SYSTEM" ]
    then
        case_dir="${PWD}/case_${case_number}"
    else
        case_dir="${TEST_DATA_HOME_CHECK}/case_${case_number}"
    fi
    ref_out="${case_dir}/${ref_out}"
    
    # add path to decoder output
    decoder_out="${PWD}/${decoder_out}"
    
    # in error cases, output might not be written
    if [ ! -z "$error_category" ] && [[ -d $decoder_out || ! -e $decoder_out ]] && [[ -d $ref_out || ! -e $ref_out ]]
    then
        return 0
    fi
    
    # check if the files to be compared exists -> return if not
    if [ -d $decoder_out ] || [ ! -e $decoder_out ]
    then
        comment="Output data missing"
        return 2
    fi
    if [ -d $ref_out ] || [ ! -e $ref_out ]
    then
        comment="Reference data missing"
        return 3
    fi
    
     # check if pictures are written -> return if not
    output_size_hw=`ls -la  $decoder_out |awk '{print $5}'`
    output_size_model=`ls -la  $ref_out |awk '{print $5}'`
    
    # in error cases, output might not be written
    if [ ! -z "$error_category" ] && [ "$output_size_hw" -eq 0 ] && [ "$output_size_model" -eq 0 ]
    then
        return 0
    fi
    if [ "$output_size_hw" -eq 0 ]
    then
        comment="No output picture written"
        return 2
    fi
    if [ "$output_size_model" -eq 0 ]
    then
        comment="No reference picture written"
        return 3
    fi
    
    comment=""
    
    end_addr_support=1
    if [ "$decoder" == "vp6" ]
    then
        end_addr_support=0
    elif  [ ! -z "$vp7_category" ]
    then
        end_addr_support=0
    else
        if [ ! -z "$webp_category" ] || [ ! -z "$vp8_category" ]
        then
            i=`expr index "$HWBASETAG" '_'`
            hw_buid_string=${HWBASETAG:$i}
            i=`expr index "$hw_buid_string" '_'`
            let 'i-=1'
            hw_major=${hw_buid_string:0:$i}
            let 'i+=1'
            hw_minor=${hw_buid_string:$i}
            if [[ "$PRODUCT" == "g1" && "$hw_major" -eq "2" && "$hw_minor" -le "34" ]] || [[ "$PRODUCT" == "g1" && "$hw_major" -le 1 ]]
            then
                end_addr_support=0
            fi
        fi
    fi
    if [ "$TB_MD5SUM" == "ENABLED" ] && [[ "$decoder" != "jpeg" || "$post_processor" == "pp" ]]
    then
        # compare output to reference
        # the decoder output matches the expected output
        #if (diff -q $ref_out $decoder_out >> /dev/null 2>&1)
        if (cmp -s $decoder_out $ref_out)
        then
            if [ "$TB_INTERNAL_TEST" == "ENABLED" ]
            then
                if [ ! -e case_${case_number}_swreg.trc ]
                then
                    comment="case_${case_number}_swreg.trc missing"
                    return 1
                fi
                if [ ! -e case_${case_number}/swreg.trc ]
                then
                    comment="case_${case_number}/swreg.trc missing"
                    return 1
                fi
                # HOX! Not yet supported
                #if [ "$PRODUCT" == "8170" ] && [ "$decoder" == "jpeg" ]
                if [ "$decoder" != "none" ] && [ "$post_processor" !=  "none" ]
                then
                    if [ "$decoder" == "jpeg" ]
                    then
                        modSwRegTraceJpegPp8170 "case_${case_number}_swreg.trc"
                        modSwRegTraceJpegPp8170 "case_${case_number}/swreg.trc"
                    else
                        modSwRegTraceDecPp "case_${case_number}_swreg.trc" "$end_addr_support"
                        modSwRegTraceDecPp "case_${case_number}/swreg.trc" "$end_addr_support"
                    fi
                elif [ "$decoder" == "jpeg" ]
                then
                    modSwRegTraceJpeg8170 "case_${case_number}_swreg.trc"
                    modSwRegTraceJpeg8170 "case_${case_number}/swreg.trc"
                else
                    modSwRegTrace "case_${case_number}_swreg.trc" "$end_addr_support"
                    modSwRegTrace "case_${case_number}/swreg.trc" "$end_addr_support"
                fi
                if (cmp -s case_${case_number}_swreg.trc.mod case_${case_number}/swreg.trc.mod)
                then
                    return 0
                else
                    #comment="SW register trace does not match"
                    # print differing lines
                    `sdiff -s case_${case_number}_swreg.trc.mod case_${case_number}/swreg.trc.mod | cat -n > swreg.diff.tmp`
                    `cat swreg.diff.tmp | tr "\t" " " > swreg.diff`
                    rm -f swreg.diff.tmp
                    # get last line number
                    last_line_number=`tail -n 1 swreg.diff | awk '{print $1}'`
                    # check swreg.trc line by line
                    let 'current_line_number=1'
                    while [ "$current_line_number" -le "$last_line_number" ]
                    do
                        diff_line=`grep -w "${current_line_number} R" swreg.diff`
                        diff_line_number=`echo "$diff_line" | awk '{print $1}'`
                        # not a grep mistake
                        if [ "$current_line_number" == "$diff_line_number" ]
                        then
                            fail_reason=`echo "$diff_line" | grep -w "STREAM CONSUMED:"`
                            if [ ! -z "$fail_reason" ]
                            then
                                fail_reason=`echo "$diff_line" | grep -w "CTRL"`
                                if [ ! -z "$fail_reason" ]
                                then
                                    a_comment="Amount of control stream consumed differs"
                                else
                                    a_comment="Amount of stream consumed differs"
                                fi
                            fi
                            fail_reason=`echo "$diff_line" | grep -w "REFERENCE BUFFER HIT SUM:"`
                            if [ ! -z "$fail_reason" ]
                            then
                                a_comment="Reference buffer return value differs"
                            fi
                            fail_reason=`echo "$diff_line" | grep -w "REFERENCE BUFFER INTRA SUM:"`
                            if [ ! -z "$fail_reason" ]
                            then
                                a_comment="Reference buffer return value differs"
                            fi
                            fail_reason=`echo "$diff_line" | grep -w "REFERENCE BUFFER Y_MV SUM:"`
                            if [ ! -z "$fail_reason" ]
                            then
                                a_comment="Reference buffer return value differs"
                            fi
                            fail_reason=`echo "$diff_line" | grep -w "REFERENCE BUFFER TOP SUM:"`
                            if [ ! -z "$fail_reason" ]
                            then
                                a_comment="Reference buffer return value differs"
                            fi
                            fail_reason=`echo "$diff_line" | grep -w "REFERENCE BUFFER BOTTOM SUM:"`
                            if [ ! -z "$fail_reason" ]
                            then
                                a_comment="Reference buffer return value differs"
                            fi
                            fail_reason=`echo "$diff_line" | grep -w "INTERRUPT STATUS:"`
                            if [ ! -z "$fail_reason" ]
                            then
                                a_comment="Interrupt status differs"
                            fi
                            already_found_failed=`echo "$comment" | grep "$a_comment"`
                            if [ -z "$already_found_failed" ]
                            then
                                if [ -z "$comment" ]
                                then
                                    comment="${a_comment}"
                                else
                                    comment="${comment}, ${a_comment}"
                                fi
                            fi
                        fi
                        let 'current_line_number+=1'
                    done
                    return 1
                fi
            else
                return 0
            fi
        else
            rm -f tmp
            `cmp $decoder_out $ref_out >& tmp`
            
            # check if EOF was printed by cmp
            eof=`awk '{print $2}' tmp`
            if [ "$eof" == "EOF" ]
            then
                # check if decoder_out is shorter
                eof_on_decoder_out=`grep "$decoder_out" tmp`
                if [ ! -z "$eof_on_decoder_out" ]
                then
                    #getLastline "$decoder_out"
                    if [ -e case_${case_number}.time_limit ]
                    then
                        return 0
                    fi
                    comment="Output data includes less pictures"
                # reference out is shorter
                else
                    #getLastline "$ref_out"
                    comment="Output data includes extra pictures"
                fi
            else
                failing_picture=`cat tmp | awk -F \, '{print $2}' | awk '{print $2}'`
                let 'failing_picture-=1'
                comment="Picture ${failing_picture} not bit exact"
            fi
            
            return 1
        fi
    
    else
        # compare output to reference
        # the decoder output matches the expected output
        if (cmp -s $decoder_out $ref_out)
        then
            #result2ScreenAndFile "$case_number" "OK"
            if [ "$TB_INTERNAL_TEST" == "ENABLED" ]
            then
                if [ ! -e case_${case_number}_swreg.trc ]
                then
                    comment="case_${case_number}_swreg.trc missing"
                    return 1
                fi
                if [ ! -e case_${case_number}/swreg.trc ]
                then
                    comment="case_${case_number}/swreg.trc missing"
                    return 1
                fi
                # HOX! Not yet supported
                #if [ "$PRODUCT" == "8170" ] && [ "$decoder" == "jpeg" ]
                if [ "$decoder" != "none" ] && [ "$post_processor" !=  "none" ]
                then
                    if [ "$decoder" == "jpeg" ]
                    then
                        modSwRegTraceJpegPp8170 "case_${case_number}_swreg.trc"
                        modSwRegTraceJpegPp8170 "case_${case_number}/swreg.trc"
                    else
                        modSwRegTraceDecPp "case_${case_number}_swreg.trc" "$end_addr_support"
                        modSwRegTraceDecPp "case_${case_number}/swreg.trc" "$end_addr_support"
                    fi
                elif [ "$decoder" == "jpeg" ]
                then
                    modSwRegTraceJpeg8170 "case_${case_number}_swreg.trc"
                    modSwRegTraceJpeg8170 "case_${case_number}/swreg.trc"
                else
                    modSwRegTrace "case_${case_number}_swreg.trc" "$end_addr_support"
                    modSwRegTrace "case_${case_number}/swreg.trc" "$end_addr_support"
                fi
                if (cmp -s case_${case_number}_swreg.trc.mod case_${case_number}/swreg.trc.mod)
                then
                    return 0
                else
                    #comment="SW register trace does not match"
                    # print differing lines
                    `sdiff -s case_${case_number}_swreg.trc.mod case_${case_number}/swreg.trc.mod | cat -n > swreg.diff.tmp`
                    `cat swreg.diff.tmp | tr "\t" " " > swreg.diff`
                    rm -f swreg.diff.tmp
                    # get last line number
                    last_line_number=`tail -n 1 swreg.diff | awk '{print $1}'`
                    # check swreg.trc line by line
                    let 'current_line_number=1'
                    while [ "$current_line_number" -le "$last_line_number" ]
                    do
                        diff_line=`grep -w "${current_line_number} R" swreg.diff`
                        diff_line_number=`echo "$diff_line" | awk '{print $1}'`
                        # not a grep mistake
                        if [ "$current_line_number" == "$diff_line_number" ]
                        then
                            fail_reason=`echo "$diff_line" | grep -w "STREAM CONSUMED:"`
                            if [ ! -z "$fail_reason" ]
                            then
                                fail_reason=`echo "$diff_line" | grep -w "CTRL"`
                                if [ ! -z "$fail_reason" ]
                                then
                                    a_comment="Amount of control stream consumed differs"
                                else
                                    a_comment="Amount of stream consumed differs"
                                fi
                            fi
                            fail_reason=`echo "$diff_line" | grep -w "REFERENCE BUFFER HIT SUM:"`
                            if [ ! -z "$fail_reason" ]
                            then
                                a_comment="Reference buffer return value differs"
                            fi
                            fail_reason=`echo "$diff_line" | grep -w "REFERENCE BUFFER INTRA SUM:"`
                            if [ ! -z "$fail_reason" ]
                            then
                                a_comment="Reference buffer return value differs"
                            fi
                            fail_reason=`echo "$diff_line" | grep -w "REFERENCE BUFFER Y_MV SUM:"`
                            if [ ! -z "$fail_reason" ]
                            then
                                a_comment="Reference buffer return value differs"
                            fi
                            fail_reason=`echo "$diff_line" | grep -w "REFERENCE BUFFER TOP SUM:"`
                            if [ ! -z "$fail_reason" ]
                            then
                                a_comment="Reference buffer return value differs"
                            fi
                            fail_reason=`echo "$diff_line" | grep -w "REFERENCE BUFFER BOTTOM SUM:"`
                            if [ ! -z "$fail_reason" ]
                            then
                                a_comment="Reference buffer return value differs"
                            fi
                            fail_reason=`echo "$diff_line" | grep -w "INTERRUPT STATUS:"`
                            if [ ! -z "$fail_reason" ]
                            then
                                a_comment="Interrupt status differs"
                            fi
                            already_found_failed=`echo "$comment" | grep "$a_comment"`
                            if [ -z "$already_found_failed" ]
                            then
                                if [ -z "$comment" ]
                                then
                                    comment="${a_comment}"
                                else
                                    comment="${comment}, ${a_comment}"
                                fi
                            fi
                        fi
                        let 'current_line_number+=1'
                    done
                    return 1
                fi
            else
                return 0
            fi
        
        # if extra information (the failing picture number) is needed in the report
        elif [ "$no_failing_picture" == 0 ]
        then
            # check if EOF was printed by cmp
            rm -f tmp
            `cmp $decoder_out $ref_out >& tmp`
            eof=`awk '{print $2}' tmp`
        
            # compare the file size of the reference file and the decoder output
            decoder_out_size=`ls -l $decoder_out 2> /dev/null | awk '{print $5}'`
            reference_out_size=`ls -l $ref_out 2> /dev/null | awk '{print $5}'`
            if [ "$decoder_out_size" -gt "$reference_out_size" ]
            then
                if [ "$eof" == "EOF" ]
                then
                    comment="Output data includes extra pictures"
                
                else
                    findFailingPicture "$case_number" "$decoder_out" "$ref_out"
                    if [ $? -eq 0 ]
                    then
                        comment="Picture ${failing_picture} not bit exact"
                    
                    else
                        comment="Picture ?(cannot calculate the failing picture)"
                    fi
                fi
            
            elif [ "$decoder_out_size" -lt "$reference_out_size" ]
            then
                if [ "$eof" == "EOF" ]
                then
                    if [ -e case_${case_number}.time_limit ]
                    then
                        return 0
                    fi
                    
                    findFailingPictureWhenNotFullyDecoded "$case_number" "$decoder_out_size"
                    if [ $? -eq 0 ]
                    then
                        comment="Picture ${failing_picture} not fully decoded"
                    
                    else
                        comment="Picture ?(cannot calculate the failing picture)"
                    fi
                
                else
                    findFailingPicture "$case_number" "$decoder_out" "$ref_out"
                    if [ $? -eq 0 ]
                    then
                        comment="Picture ${failing_picture} not bit exact"
                    
                    else
                        comment="Picture ?(cannot calculate the failing picture)"
                    fi
                fi
            
            else
                findFailingPicture "$case_number" "$decoder_out" "$ref_out"
                if [ $? -eq 0 ]
                then
                    comment="Picture ${failing_picture} not bit exact"
                
                else
                    comment="Picture ?(cannot calculate the failing picture)"
                fi
            fi
            #result2ScreenAndFile "$case_number" "FAILED" "$comment"
            return 1
        
        elif [ "$no_failing_picture" == 1 ] &&  [ -e case_${case_number}.time_limit ]
        then
            # check if EOF was printed by cmp
            rm -f tmp
            `cmp $decoder_out $ref_out >& tmp`
            eof=`awk '{print $2}' tmp`
        
            # compare the file size of the reference file and the decoder output
            decoder_out_size=`ls -l $decoder_out 2> /dev/null | awk '{print $5}'`
            reference_out_size=`ls -l $ref_out 2> /dev/null | awk '{print $5}'`
            if [ "$decoder_out_size" -lt "$reference_out_size" ] && [ "$eof" == "EOF" ]
            then
				return 0
            fi
        
        else
            #result2ScreenAndFile "$case_number" "FAILED"
            return 1
        fi
    fi
    
    #return 0
}

#-------------------------------------------------------------------------------
#-                                                                            --
#- Generates reference data for the case in question.                         --
#-                                                                            --
#- Parameters                                                                 --
#- 1 : Test case number                                                       --
#- 2 : Executable for reference model                                         --
#- 3 : Commandline arguments for the reference model                          --
#-                                                                           --
#-------------------------------------------------------------------------------
runModel()
{
    case_number=$1
    ref_model=$2
    ref_model_args=$3
    
    # create reference data directory if not exists
    if [ ! -d "case_${case_number}" ]
    then
        mkdir "case_${case_number}"
    fi
        
    case_dir="$PWD/case_${case_number}"
    cd "$case_dir"
    
    # if the configuration has been changed or not exists 
    # -> do generate reference data
    generate_data="0"
    if (! cmp -s "../case_${case_number}.cfg" ./tb.cfg)
    then
        generate_data="1"
    elif [ -e ../case_${case_number}_tb_params ]
    then
        if (! cmp -s "../case_${case_number}_tb_params" ./case_${case_number}_tb_params)
        then
            generate_data="1"
        fi
    fi
    if [ $generate_data -eq "1" ]
    then
        rm -rf *
        getCategory "$case_number"
        if eval "echo ${category} | grep 'pp_422order' >/dev/null"
        then
            cp ${TEST_DATA_HOME_CHECK}/case_${case_number}/input.yuv .
        fi
        echo "fpga" >> "trace.cfg"
        echo "decoding_tools" >> trace.cfg
        if [ "$TB_MODEL_TRACE" == "ENABLED" ]
        then
            echo "toplevel" >> trace.cfg
        fi
        cp ../case_${case_number}.cfg ./tb.cfg
        if [ -e ../case_${case_number}_tb_params ]
        then
            cp "../case_${case_number}_tb_params" ./case_${case_number}_tb_params
            ref_model_args_tmp=`cat case_${case_number}_tb_params`
            ref_model_args="${ref_model_args_tmp} ${ref_model_args}"
        fi
        echo "$ref_model $ref_model_args"
        $ref_model $ref_model_args > system.log 2>&1
    fi
        
    cd - >> /dev/null
}

#-------------------------------------------------------------------------------
#-                                                                            --
#- Prepares a H.264 test case for system model execution.                     --
#-                                                                            --
#- Parameters                                                                 --
#- 1 : Test case number                                                       --
#-                                                                            --
#-------------------------------------------------------------------------------
prepareH264Case()
{
    case_number=$1
    pics=$2
    
    if [ "$TB_H264_TOOLS_NOT_SUPPORTED" != "DISABLED" ]
    then
        tools_not_supported=`echo "$TB_H264_TOOLS_NOT_SUPPORTED" | tr " " "_"`
        tools_not_supported=`echo "$tools_not_supported" | tr "," " "`
        for tool_not_supported in $tools_not_supported
        do
            tool_not_supported=`echo "$tool_not_supported" | tr "_" " "`
            if [ -e "${TEST_DATA_HOME_CHECK}/case_${case_number}/decoding_tools.trc" ]
            then
                not_supported_tool_in_strm_tmp=`grep -i "$tool_not_supported" ${TEST_DATA_HOME_CHECK}/case_${case_number}/decoding_tools.trc | grep 1`
                if [ ! -z "$not_supported_tool_in_strm_tmp" ]
                then
                    return
                fi
            fi
        done
    fi

    runModel "$case_number" "$H264_DECODER_REF" "-N$pics ${TEST_DATA_HOME_CHECK}/case_${case_number}/${H264_STREAM}"
}

#-------------------------------------------------------------------------------
#-                                                                            --
#- Prepares a MPEG-4/H.263 test case for system model execution.              --
#-                                                                            --
#- Parameters                                                                 --
#- 1 : Test case number                                                       --
#-                                                                            --
#-------------------------------------------------------------------------------
prepareMpeg4Case()
{
    case_number=$1
    pics=$2
    
    instream=""
    if [ -e "${TEST_DATA_HOME_CHECK}/case_${case_number}/${MPEG4_STREAM}" ]
    then
        instream="${TEST_DATA_HOME_CHECK}/case_${case_number}/$MPEG4_STREAM"
        
    elif [ -e "${TEST_DATA_HOME_CHECK}/case_${case_number}/${DIVX_STREAM}" ]
    then
        instream="${TEST_DATA_HOME_CHECK}/case_${case_number}/$DIVX_STREAM"
    fi
    runModel "$case_number" "$MPEG4_DECODER_REF" "-N$pics ${instream}"
}

#-------------------------------------------------------------------------------
#-                                                                            --
#- Prepares a VC-1 test case for system model execution and runs the model.   --
#-                                                                            --
#- Parameters                                                                 --
#- 1 : Test case number                                                       --
#-                                                                            --
#-------------------------------------------------------------------------------
prepareVc1Case()
{
    case_number=$1
    pics=$2

    runModel "$case_number" "$VC1_DECODER_REF" "-N$pics ${TEST_DATA_HOME_CHECK}/case_${case_number}/${VC1_STREAM}"
}

#-------------------------------------------------------------------------------
#-                                                                            --
#- Prepares a JPEG test case for system model execution.                      --
#-                                                                            --
#- Parameters                                                                 --
#- 1 : Test case number                                                       --
#-                                                                            --
#-------------------------------------------------------------------------------
prepareJpegCase()
{
    case_number=$1
    pics=$2
    
    runModel "$case_number" "$JPEG_DECODER_REF" "${TEST_DATA_HOME_CHECK}/case_${case_number}/${JPEG_STREAM}"
}

#-------------------------------------------------------------------------------
#-                                                                            --
#- Prepares a MPEG-2/MPEG-1 test casee for system model execution.            --
#-                                                                            --
#- Parameters                                                                 --
#- 1 : Test case number                                                       --
#-                                                                            --
#-------------------------------------------------------------------------------
prepareMpeg2Case()
{
    case_number=$1
    pics=$2
    
    stream="${TEST_DATA_HOME_CHECK}/case_${case_number}/${MPEG2_STREAM}"
    # Try mpeg1 stream
    if [ ! -e "$stream" ]
    then
        stream="${TEST_DATA_HOME_CHECK}/case_${case_number}/${MPEG1_STREAM}"
    fi
    runModel "$case_number" "$MPEG2_DECODER_REF" "-N$pics $stream"
}

#-------------------------------------------------------------------------------
#-                                                                            --
#- Prepares a Real test casee for system model execution.                     --
#-                                                                            --
#- Parameters                                                                 --
#- 1 : Test case number                                                       --
#-                                                                            --
#-------------------------------------------------------------------------------
prepareRealCase()
{
    case_number=$1
    pics=$2
    
    stream="${TEST_DATA_HOME_CHECK}/case_${case_number}/${REAL_STREAM}"
    runModel "$case_number" "$REAL_DECODER_REF" "-N$pics ${stream}"
}

#-------------------------------------------------------------------------------
#-                                                                            --
#- Prepares a VP6 test casee for system model execution.                     --
#-                                                                            --
#- Parameters                                                                 --
#- 1 : Test case number                                                       --
#-                                                                            --
#-------------------------------------------------------------------------------
prepareVP6Case()
{
    case_number=$1
    pics=$2
    
    stream="${TEST_DATA_HOME_CHECK}/case_${case_number}/${VP6_STREAM}"
    
    if [ "$TB_MD5SUM" == "ENABLED" ]
    then
        if [ ${TB_MAX_NUM_PICS} != 0 ]
        then
            runModel "$case_number" "$VP6_DECODER_REF" "-m -N${TB_MAX_NUM_PICS} ${stream}"
        else
            runModel "$case_number" "$VP6_DECODER_REF" "-m ${stream}"
        fi
    else
        if [ ${TB_MAX_NUM_PICS} != 0 ]
        then
            runModel "$case_number" "$VP6_DECODER_REF" "-N${pics} ${stream}"
        else
            runModel "$case_number" "$VP6_DECODER_REF" "${stream}"
        fi
    fi
}

#-------------------------------------------------------------------------------
#-                                                                            --
#- Prepares a VP8 test casee for system model execution.                     --
#-                                                                            --
#- Parameters                                                                 --
#- 1 : Test case number                                                       --
#-                                                                            --
#-------------------------------------------------------------------------------
prepareVP8Case()
{
    case_number=$1
    pics=$2
    
    stream="${TEST_DATA_HOME_CHECK}/case_${case_number}/${VP8_STREAM}"
    # try vp7 stream
    if [ ! -e "$stream" ]
    then
        stream="${TEST_DATA_HOME_CHECK}/case_${case_number}/${VP7_STREAM}"
    fi
    # try webp stream
    if [ ! -e "$stream" ]
    then
        stream="${TEST_DATA_HOME_CHECK}/case_${case_number}/${WEBP_STREAM}"
    fi
    
    runModel "$case_number" "$VP8_DECODER_REF" "-N$pics ${stream}"
}

#-------------------------------------------------------------------------------
#-                                                                            --
#- Prepares an AVS test casee for system model execution.                     --
#-                                                                            --
#- Parameters                                                                 --
#- 1 : Test case number                                                       --
#-                                                                            --
#-------------------------------------------------------------------------------
prepareAVSCase()
{
    case_number=$1
    pics=$2
    
    stream="${TEST_DATA_HOME_CHECK}/case_${case_number}/${AVS_STREAM}"
    runModel "$case_number" "$AVS_DECODER_REF" "-N$pics ${stream}"
}

#-------------------------------------------------------------------------------
#-                                                                            --
#- Prepares a post-processor test case (including pipelined cases) for system --
#- model execution.                                                           --
#-                                                                            --
#- Parameters                                                                 --
#- 1 : Test case number                                                       --
#-                                                                            --
#-------------------------------------------------------------------------------
preparePpCase()
{
    case_number=$1
    
    getComponent "$case_number"
    
    # external pp
    if [ "$post_processor" == "pp" ] && [ "$decoder" == "none" ]
    then
        if [ -e "case_${case_number}_pp.cfg" ]
        then
            runModel "$case_number" "$PP_EXTERNAL_REF" "../case_${case_number}_pp.cfg"
        else
            runModel "$case_number" "$PP_EXTERNAL_REF" "${TEST_DATA_HOME_CHECK}/case_${case_number}/${PP_CFG}"
        fi
    fi
    
    if [ "$decoder" == "h264" ] && [ "$post_processor" == "pp" ]
    then
        runModel "$case_number" "$PP_H264_PIPELINE_REF" "-N$TB_MAX_NUM_PICS ${TEST_DATA_HOME_CHECK}/case_${case_number}/${H264_STREAM} ${TEST_DATA_HOME_CHECK}/case_${case_number}/${PP_CFG}"
    fi
    
    # mpeg-4 pipelined
    if [ "$decoder" == "mpeg4" ] && [ "$post_processor" == "pp" ]
    then
        instream=""
        if [ -e "${TEST_DATA_HOME_CHECK}/case_${case_number}/${MPEG4_STREAM}" ]
        then
            instream="${TEST_DATA_HOME_CHECK}/case_${case_number}/${MPEG4_STREAM}"
            
        elif [ -e "${TEST_DATA_HOME_CHECK}/case_${case_number}/${DIVX_STREAM}" ]
        then
            instream="${TEST_DATA_HOME_CHECK}/case_${case_number}/${DIVX_STREAM}"
        fi
        
        runModel "$case_number" "$PP_MPEG4_PIPELINE_REF" "-N$TB_MAX_NUM_PICS ${instream} ${TEST_DATA_HOME_CHECK}/case_${case_number}/${PP_CFG}"
    fi
    
    if [ "$decoder" == "vc1" ] && [ "$post_processor" == "pp" ]
    then
        runModel "$case_number" "$PP_VC1_PIPELINE_REF" "-N$TB_MAX_NUM_PICS ${TEST_DATA_HOME_CHECK}/case_${case_number}/${VC1_STREAM} ${TEST_DATA_HOME_CHECK}/case_${case_number}/${PP_CFG}"
    fi

    # jpeg pipelined
    if [ "$decoder" == "jpeg" ] && [ "$post_processor" == "pp" ]
    then
        runModel "$case_number" "$PP_JPEG_PIPELINE_REF" "${TEST_DATA_HOME_CHECK}/case_${case_number}/${JPEG_STREAM} ${TEST_DATA_HOME_CHECK}/case_${case_number}/${PP_CFG}"
    fi
    
    # mpeg-2 pipelined
    if [ "$decoder" == "mpeg2" ] && [ "$post_processor" == "pp" ]
    then
        runModel "$case_number" "$PP_MPEG2_PIPELINE_REF" "-N$TB_MAX_NUM_PICS ${TEST_DATA_HOME_CHECK}/case_${case_number}/${MPEG2_STREAM} ${TEST_DATA_HOME_CHECK}/case_${case_number}/${PP_CFG}"
    fi
    
    # real pipelined
    if [ "$decoder" == "real" ] && [ "$post_processor" == "pp" ]
    then
        runModel "$case_number" "$PP_REAL_PIPELINE_REF" "-N$TB_MAX_NUM_PICS ${TEST_DATA_HOME_CHECK}/case_${case_number}/${REAL_STREAM} ${TEST_DATA_HOME_CHECK}/case_${case_number}/${PP_CFG}"
    fi
    
    # vp6 pipelined
    if [ "$decoder" == "vp6" ] && [ "$post_processor" == "pp" ]
    then
        if [ "$TB_MAX_NUM_PICS" != 0 ]
        then
            runModel "$case_number" "$PP_VP6_PIPELINE_REF" "-N$TB_MAX_NUM_PICS ${TEST_DATA_HOME_CHECK}/case_${case_number}/${VP6_STREAM} ${TEST_DATA_HOME_CHECK}/case_${case_number}/${PP_CFG}"
        else
            runModel "$case_number" "$PP_VP6_PIPELINE_REF" "${TEST_DATA_HOME_CHECK}/case_${case_number}/${VP6_STREAM} ${TEST_DATA_HOME_CHECK}/case_${case_number}/${PP_CFG}"
        fi
    fi
    
    # vp8 pipelined
    if [ "$decoder" == "vp8" ] && [ "$post_processor" == "pp" ]
    then
        stream="${TEST_DATA_HOME_CHECK}/case_${case_number}/${VP8_STREAM}"
        # try vp7 stream
        if [ ! -e "$stream" ]
        then
            stream="${TEST_DATA_HOME_CHECK}/case_${case_number}/${VP7_STREAM}"
        fi
        # try webp stream
        if [ ! -e "$stream" ]
        then
            stream="${TEST_DATA_HOME_CHECK}/case_${case_number}/${WEBP_STREAM}"
            tiled_mode_flag=""
        fi
        runModel "$case_number" "$PP_VP8_PIPELINE_REF" "${stream} ${TEST_DATA_HOME_CHECK}/case_${case_number}/${PP_CFG}"
    fi
    
    # avs pipelined
    if [ "$decoder" == "avs" ] && [ "$post_processor" == "pp" ]
    then
        stream="${TEST_DATA_HOME_CHECK}/case_${case_number}/${AVS_STREAM}"
        
        runModel "$case_number" "$PP_AVS_PIPELINE_REF" "${stream} ${TEST_DATA_HOME_CHECK}/case_${case_number}/${PP_CFG}"
    fi
}

#-------------------------------------------------------------------------------
#-                                                                            --
#- Prepares a test case for system model execution and runs the model.        --
#-                                                                            --
#- Parameters                                                                 --
#- 1 : Test case number                                                       --
#- 2 : The decoding format in question                                        --
#- 3 : Possible post-processor                                                --
#-                                                                            --
#-------------------------------------------------------------------------------
prepareCheckCase()
{
    case_number=$1
    decoder=$2
    post_processor=$3
    
    # don't run if not enough disk space
    export_disk=`df -m . | grep export`
    if [ ! -z "$export_disk" ]
    then
        let element_number=0
        for i in $export_disk
        do
            available_space="$i"
            let 'element_number=element_number+1'
            if [ "$element_number" == 4 ]
            then
                if [ "$available_space" -lt 100 ]
                then
                    return
                fi
                break
            fi
        done
    fi
    
    pics=$TB_MAX_NUM_PICS
    
    # determine the correct decoder output file
    decoder_out=""
    if [ "$DEC_OUTPUT_FORMAT" == "TILED" ] || [ -e "case_${case_number}_tiled_mode" ]
    then
        decoder_out="case_${case_number}_tiled.yuv"
    else
        decoder_out="case_${case_number}_raster_scan.yuv"
    fi
    
    # if execution time is limited, we need to set the correct amount pictures executed
    if [ -e "case_${case_number}.time_limit" ] && [ -e "$decoder_out" ] && [ "$case_number" != 8330 ] && [ "$case_number" != 8332 ] && [ "$case_number" != 8333 ] && [ "$case_number" != 8334 ]
    then
        # we need to wait to case to finalize in order to get the correct number of pictures decoded
        while [ ! -e "case_${case_number}.done" ]
        do
            sleep 1
        done
        if [ "$TB_MD5SUM" == "ENABLED" ]
        then
            pics=`cat "$decoder_out" | nl | tac | head -n 1 | awk '{print $1}'`
            # add some extra pictures
            let 'pics+=10'
            
        elif [ "$TB_MD5SUM" == "DISABLED" ]
        then
            pic_width=`cat ${TEST_DATA_HOME_CHECK}/case_${case_number}/picture_ctrl_dec.trc | grep -m 1 -i "picture width" | awk '{print $1}' 2> /dev/null`
            pic_height=`cat ${TEST_DATA_HOME_CHECK}/case_${case_number}/picture_ctrl_dec.trc | grep -m 1 -i "picture height" | awk '{print $1}' 2> /dev/null`
            let 'pic_width*=16'
            let 'pic_height*=16'
    
            if [ ! -z "$pic_width" ] && [ ! -z "$pic_height" ] && [ "$pic_width" -ne 0 ] && [ "$pic_height" -ne 0 ]
            then
                file_size=`ls -la "$decoder_out" | awk '{print $5}'`
                let 'pixel_in_pic=pic_width*pic_height*15'
                let 'file_size=file_size*10'
                let 'pics=file_size/pixel_in_pic'
                let 'extra_picture=file_size%pixel_in_pic'
                if [ "$extra_picture" != 0 ]
                then
                    let 'pics+=1'
                fi
                # add some extra pictures
                let 'pics+=10'
            fi
        fi
        # interlaced sequence requires x2 amount of pictures in the worst case
        interlaced_sequence=`cat case_${case_number}${TB_LOG_FILE_SUFFIX} | grep -m 1 "INTERLACED SEQUENCE"`
        if [ ! -z "$interlaced_sequence" ]
        then
            let 'pics*=2'
        else
            interlaced_sequence=`cat case_${case_number}${TB_LOG_FILE_SUFFIX} | grep -m 1 "Interlaced = 1"`
            if [ ! -z "$interlaced_sequence" ]
            then
                let 'pics*=2'
            fi
        fi
    fi
    
    if [ "$post_processor" == "pp" ]
    then
        preparePpCase "$case_number"
    fi
    
    if [ "$decoder" == "h264" ] && [ "$post_processor" != "pp" ]
    then
        prepareH264Case "$case_number" "$pics"
    fi

    if [ "$decoder" == "mpeg4" ]  && [ "$post_processor" != "pp" ]
    then
        prepareMpeg4Case "$case_number" "$pics"
    fi

    if [ "$decoder" == "vc1" ]  && [ "$post_processor" != "pp" ]
    then
        prepareVc1Case "$case_number" "$pics"
    fi

    if [ "$decoder" == "jpeg" ]  && [ "$post_processor" != "pp" ]
    then
        prepareJpegCase "$case_number"
    fi
    
    if [ "$decoder" == "mpeg2" ]  && [ "$post_processor" != "pp" ]
    then
        prepareMpeg2Case "$case_number" "$pics"
    fi
    
    if [ "$decoder" == "real" ]  && [ "$post_processor" != "pp" ]
    then
        prepareRealCase "$case_number" "$pics"
    fi

    if [ "$decoder" == "vp6" ]  && [ "$post_processor" != "pp" ]
    then
        prepareVP6Case "$case_number" "$pics"
    fi
    
    if [ "$decoder" == "vp8" ]  && [ "$post_processor" != "pp" ]
    then
        prepareVP8Case "$case_number" "$pics"
    fi
    
    if [ "$decoder" == "avs" ]  && [ "$post_processor" != "pp" ]
    then
        prepareAVSCase "$case_number" "$pics"
    fi
}

#-------------------------------------------------------------------------------
#-                                                                            --
#- Checks the result of a H.264 test case                                     --
#-                                                                            --
#- Parameters                                                                 --
#- 1 : Test case number                                                       --
#-                                                                            --
#- Returns                                                                    --
#- 3:  Reference data missing                                                 -- 
#- 2:  Output data missing                                                    --
#- 1 : Output data does not match with reference data                         --
#- 0 : Otherwise                                                              --
#-                                                                            --
#-------------------------------------------------------------------------------
checkH264Case()
{
    case_number=$1
    
    # determine the correct files to be compared
    decoder_out=""
    ref_out=""
    if [ -e "case_${case_number}${H264_OUT_TILED_SUFFIX}" ]
    then
        if [ "$PRODUCT" != 8170 ] && [ "$PRODUCT" != "g1" ]
        then
            echo "INTERNAL ERROR: TILED not supported for ${PRODUCT}"
        fi
        decoder_out="case_${case_number}${H264_OUT_TILED_SUFFIX}"
        if [ "$PRODUCT" == 8170 ]
        then
            ref_out="${H264_REFERENCE_TILED}"
        else
            ref_out="${H264_REFERENCE_RASTER_SCAN}"
        fi
       
    elif [ -e "case_${case_number}${H264_OUT_RASTER_SCAN_SUFFIX}" ]
    then
       decoder_out="case_${case_number}${H264_OUT_RASTER_SCAN_SUFFIX}"
       ref_out="${H264_REFERENCE_RASTER_SCAN}"
    fi
    
    compare "$decoder_out" "$ref_out" 0 "$case_number" "h264" "none"
    retval=$?
    if [ "$retval" != 0 ]
    then
        return "$retval"
    fi
    
    if [ "$DEC_2ND_CHROMA_TABLE" == "ENABLED" ] && [ "$PRODUCT" == "9170" ]
    then
        #monochrome=`echo "$category" | grep monoc`
        monochrome=`cat "${TEST_DATA_HOME_CHECK}/case_${case_number}/decoding_tools.trc" | grep Monochrome | grep 1`
        if [ -z "$monochrome" ] 
        then
            compare "2nd_chroma_case_${case_number}.yuv" "$H264_2ND_CHROMA_TABLE_OUTPUT" 0 "$case_number" "h264" "none"
            return $?
        else
            return "$retval"    
        fi  
    fi
}

#-------------------------------------------------------------------------------
#-                                                                            --
#- Checks the result of a MPEG-4/H.263 test case.                             --
#-                                                                            --
#- Parameters                                                                 --
#- 1 : Test case number                                                       --
#-                                                                            --
#- Returns                                                                    --
#- 3:  Reference data missing                                                 -- 
#- 2:  Output data missing                                                    --
#- 1 : Output data does not match with reference data                         --
#- 0 : Otherwise                                                              --
#-                                                                            --
#-------------------------------------------------------------------------------
checkMpeg4Case()
{
    case_number=$1
    
    # determine the correct files to be compared
    decoder_out=""
    ref_out=""
    if [ -e "case_${case_number}${MPEG4_OUT_TILED_SUFFIX}" ]
    then
        if [ "$PRODUCT" != 8170 ] && [ "$PRODUCT" != "g1" ]
        then
            echo "INTERNAL ERROR: TILED not supported for ${PRODUCT}"
        fi
        decoder_out="case_${case_number}${MPEG4_OUT_TILED_SUFFIX}"
        if [ "$PRODUCT" == 8170 ]
        then
            ref_out="${MPEG4_REFERENCE_TILED}"
        else
            ref_out="${MPEG4_REFERENCE_RASTER_SCAN}"
        fi
       
    elif [ -e "case_${case_number}${MPEG4_OUT_RASTER_SCAN_SUFFIX}" ]
    then
        decoder_out="case_${case_number}${MPEG4_OUT_RASTER_SCAN_SUFFIX}"
        ref_out="${MPEG4_REFERENCE_RASTER_SCAN}"
    fi
    
    #interlaced_sequence=`cat ${TEST_DATA_HOME_CHECK}/case_${case_number}/picture_ctrl_dec.trc | grep "interlace enable" | awk '{print $1}' | grep -m 1 1`
    interlaced_sequence=`cat case_${case_number}${TB_LOG_FILE_SUFFIX} | grep -m 1 "INTERLACED SEQUENCE"`
    if [ ! -z "$interlaced_sequence" ] && [ -e "case_${case_number}${MPEG4_OUT_TILED_SUFFIX}" ] && [ "$PRODUCT" == 8170 ]
    then
        # if interlaced sequence, the decoder writes out in raster scan format
        ref_out="${MPEG4_REFERENCE_RASTER_SCAN}"
    fi
    
    compare "$decoder_out" "$ref_out" 0 "$case_number" "mpeg4" "none"
    return $?
}

#-------------------------------------------------------------------------------
#-                                                                            --
#- Checks the result of a VC-1 test case                                      --
#-                                                                            --
#- Parameters                                                                 --
#- 1 : Test case number                                                       --
#-                                                                            --
#- Returns                                                                    --
#- 3:  Reference data missing                                                 -- 
#- 2:  Output data missing                                                    --
#- 1 : Output data does not match with reference data                         --
#- 0 : Otherwise                                                              --
#-                                                                            --
#-------------------------------------------------------------------------------
checkVc1Case()
{
    case_number=$1
    
    # determine the correct files to be compared
    decoder_out=""
    ref_out=""
    if [ -e "case_${case_number}${VC1_OUT_TILED_SUFFIX}" ]
    then
        if [ "$PRODUCT" != 8170 ] && [ "$PRODUCT" != "g1" ]
        then
            echo "INTERNAL ERROR: TILED not supported for ${PRODUCT}"
        fi
        decoder_out="case_${case_number}${VC1_OUT_TILED_SUFFIX}"
        if [ "$PRODUCT" == 8170 ]
        then
            ref_out="${VC1_REFERENCE_TILED}"
        else
            ref_out="${VC1_REFERENCE_RASTER_SCAN}"
        fi
        
    elif [ -e "case_${case_number}${VC1_OUT_RASTER_SCAN_SUFFIX}" ]
    then
        decoder_out="case_${case_number}${VC1_OUT_RASTER_SCAN_SUFFIX}"
        ref_out="${VC1_REFERENCE_RASTER_SCAN}"
    fi
    
    #interlaced_sequence=`cat ${TEST_DATA_HOME_CHECK}/case_${case_number}/picture_ctrl_dec.trc | grep "interlace enable" | awk '{print $1}' | grep -m 1 1`
    interlaced_sequence=`cat case_${case_number}${TB_LOG_FILE_SUFFIX} | grep -m 1 "INTERLACED SEQUENCE"`
    if [ ! -z "$interlaced_sequence" ] && [ -e "case_${case_number}${VC1_OUT_TILED_SUFFIX}" ] && [ "$PRODUCT" == 8170 ]
    then
        # if interlaced sequence, the decoder writes out in raster scan format
        ref_out="${VC1_REFERENCE_RASTER_SCAN}"
    fi

    compare "$decoder_out" "$ref_out" 0 "$case_number" "vc1" "none"
    return $?
}

#-------------------------------------------------------------------------------
#-                                                                            --
#- Checks the result of a JPEG test case.                                     --
#-                                                                            --
#- Parameters                                                                 --
#- 1 : Test case number                                                       --
#-                                                                            --
#- Returns                                                                    --
#- 3:  Reference data missing                                                 -- 
#- 2:  Output data missing                                                    --
#- 1 : Output data does not match with reference data                         --
#- 0 : Otherwise                                                              --
#-                                                                            --
#-------------------------------------------------------------------------------
checkJpegCase()
{
    case_number=$1
    
    # compare stream to reference
    compare "case_${case_number}.yuv" "${JPEG_OUT}" 1 "$case_number" "jpeg" "none"
    if [ $? != 0 ]
    then
        return 1
    fi
    
    # check also thumbnail
    if [ "$TB_TEST_DATA_SOURCE" == "SYSTEM" ]
    then
        case_dir="${PWD}/case_${case_number}"
    else
        case_dir="${TEST_DATA_HOME_CHECK}/case_${case_number}"
    fi
    if [ -e "${case_dir}/${JPEG_OUT_THUMB}" ]
    then
        # compare also thumbnail stream to reference
        compare "tn_case_${case_number}.yuv" "${JPEG_OUT_THUMB}" 1 "$case_number" "jpeg" "none"
        if [ $? != 0 ]
        then
            return 1
        fi
    fi
    
    return 0
}

#-------------------------------------------------------------------------------
#-                                                                            --
#- Checks the result of a MPEG-2/MPEG-1 test case.                            --
#-                                                                            --
#- Parameters                                                                 --
#- 1 : Test case number                                                       --
#-                                                                            --
#- Returns                                                                    --
#- 3:  Reference data missing                                                 -- 
#- 2:  Output data missing                                                    --
#- 1 : Output data does not match with reference data                         --
#- 0 : Otherwise                                                              --
#-                                                                            --
#-------------------------------------------------------------------------------
checkMpeg2Case()
{
    case_number=$1
    
    # determine the correct files to be compared
    decoder_out=""
    ref_out=""
    if [ -e case_${case_number}${MPEG2_OUT_TILED_SUFFIX} ] 
    then
        if [ "$PRODUCT" != 8170 ] && [ "$PRODUCT" != "g1" ]
        then
            echo "INTERNAL ERROR: TILED not supported for ${PRODUCT}"
        fi
        decoder_out="case_${case_number}${MPEG2_OUT_TILED_SUFFIX}"
        if [ "$PRODUCT" == 8170 ]
        then
            ref_out="${MPEG2_REFERENCE_TILED}"
        else
            ref_out="${MPEG2_REFERENCE_RASTER_SCAN}"
        fi
       
    elif [ -e "case_${case_number}${MPEG2_OUT_RASTER_SCAN_SUFFIX}" ]
    then
        decoder_out="case_${case_number}${MPEG2_OUT_RASTER_SCAN_SUFFIX}"
        ref_out="${MPEG2_REFERENCE_RASTER_SCAN}"
    fi
    
    #interlaced_sequence=`cat ${TEST_DATA_HOME_CHECK}/case_${case_number}/picture_ctrl_dec.trc | grep "interlace enable" | awk '{print $1}' | grep -m 1 1`
    interlaced_sequence=`cat case_${case_number}${TB_LOG_FILE_SUFFIX} | grep -m 1 "INTERLACED SEQUENCE"`
    if [ ! -z "$interlaced_sequence" ] && [ -e "case_${case_number}${MPEG2_OUT_TILED_SUFFIX}" ] && [ "$PRODUCT" == 8170 ]
    then
        # if interlaced sequence, the decoder writes out in raster scan format
        ref_out="${MPEG2_REFERENCE_RASTER_SCAN}"
    fi
    
    compare "$decoder_out" "$ref_out" 0 "$case_number" "mpeg2" "none"
    return $?
}

#-------------------------------------------------------------------------------
#-                                                                            --
#- Checks the result of a Real test case.                                     --
#-                                                                            --
#- Parameters                                                                 --
#- 1 : Test case number                                                       --
#-                                                                            --
#- Returns                                                                    --
#- 3:  Reference data missing                                                 -- 
#- 2:  Output data missing                                                    --
#- 1 : Output data does not match with reference data                         --
#- 0 : Otherwise                                                              --
#-                                                                            --
#-------------------------------------------------------------------------------
checkRealCase()
{
    case_number=$1
    
    # determine the correct files to be compared
    if [ -e case_${case_number}${REAL_OUT_TILED_SUFFIX} ]
    then
        decoder_out="case_${case_number}${REAL_OUT_TILED_SUFFIX}"
    else
        decoder_out="case_${case_number}${REAL_OUT_RASTER_SCAN_SUFFIX}"
    fi
    ref_out="${REAL_REFERENCE_RASTER_SCAN}"
    compare "$decoder_out" "$ref_out" 0 "$case_number" "real" "none"
    return $?
}

#-------------------------------------------------------------------------------
#-                                                                            --
#- Checks the result of a VP6 test case.                                     --
#-                                                                            --
#- Parameters                                                                 --
#- 1 : Test case number                                                       --
#-                                                                            --
#- Returns                                                                    --
#- 3:  Reference data missing                                                 -- 
#- 2:  Output data missing                                                    --
#- 1 : Output data does not match with reference data                         --
#- 0 : Otherwise                                                              --
#-                                                                            --
#-------------------------------------------------------------------------------
checkVP6Case()
{
    case_number=$1
    
    # determine the correct files to be compared
    if [ -e case_${case_number}${VP6_OUT_TILED_SUFFIX} ]
    then
        decoder_out="case_${case_number}${VP6_OUT_TILED_SUFFIX}"
    else
        decoder_out="case_${case_number}${VP6_OUT_RASTER_SCAN_SUFFIX}"
    fi
    ref_out="${VP6_REFERENCE_RASTER_SCAN}"
    compare "$decoder_out" "$ref_out" 0 "$case_number" "vp6" "none"
    return $?
}

#-------------------------------------------------------------------------------
#-                                                                            --
#- Checks the result of a VP8 test case.                                     --
#-                                                                            --
#- Parameters                                                                 --
#- 1 : Test case number                                                       --
#-                                                                            --
#- Returns                                                                    --
#- 3:  Reference data missing                                                 -- 
#- 2:  Output data missing                                                    --
#- 1 : Output data does not match with reference data                         --
#- 0 : Otherwise                                                              --
#-                                                                            --
#-------------------------------------------------------------------------------
checkVP8Case()
{
    case_number=$1
    
    # determine the correct files to be compared
    if [ -e case_${case_number}${VP8_OUT_TILED_SUFFIX} ]
    then
        decoder_out="case_${case_number}${VP8_OUT_TILED_SUFFIX}"
    else
        decoder_out="case_${case_number}${VP8_OUT_RASTER_SCAN_SUFFIX}"
    fi
    ref_out="${VP8_REFERENCE_RASTER_SCAN}"
    compare "$decoder_out" "$ref_out" 0 "$case_number" "vp8" "none"
    return $?
}

#-------------------------------------------------------------------------------
#-                                                                            --
#- Checks the result of an AVS test case.                                     --
#-                                                                            --
#- Parameters                                                                 --
#- 1 : Test case number                                                       --
#-                                                                            --
#- Returns                                                                    --
#- 3:  Reference data missing                                                 -- 
#- 2:  Output data missing                                                    --
#- 1 : Output data does not match with reference data                         --
#- 0 : Otherwise                                                              --
#-                                                                            --
#-------------------------------------------------------------------------------
checkAVSCase()
{
    case_number=$1
    
    # determine the correct files to be compared
    if [ -e case_${case_number}${AVS_OUT_TILED_SUFFIX} ]
    then
        decoder_out="case_${case_number}${AVS_OUT_TILED_SUFFIX}"
    else
        decoder_out="case_${case_number}${AVS_OUT_RASTER_SCAN_SUFFIX}"
    fi
    ref_out="${AVS_REFERENCE_RASTER_SCAN}"
    compare "$decoder_out" "$ref_out" 0 "$case_number" "avs" "none"
    return $?
}

#-------------------------------------------------------------------------------
#-                                                                            --
#- Checks the result of a post-processor test case (including pipelined       --
#- cases).                                                                    --
#-                                                                            --
#- Parameters                                                                 --
#- 1 : Test case number                                                       --
#-                                                                            --
#- Returns                                                                    --
#- 3:  Reference data missing                                                 -- 
#- 2:  Output data missing                                                    --
#- 1 : Output data does not match with reference data                         --
#- 0 : Otherwise                                                              --
#-                                                                            --
#-------------------------------------------------------------------------------
checkPpCase()
{
    case_number=$1
    decoder=$2
    
    # Determine the correct files to be compared
    decoder_out=""
    ref_out=""
    if [ -e "case_$case_number.rgb" ]
    then
        decoder_out="case_${case_number}.rgb"
        ref_out="${PP_OUT_RGB}"
        
    elif [ -e "case_${case_number}.yuv" ]
    then
        decoder_out="case_${case_number}.yuv"
        ref_out="${PP_OUT_YUV}"
    fi
    
    compare "$decoder_out" "$ref_out" 1 "$case_number" "$decoder" "pp"
    return $?
}

#-------------------------------------------------------------------------------
#-                                                                            --
#- Checks the result of a test case that should be bit exact. Check also      --
#- whether case was crashed or timeout occured.                               --
#-                                                                            --
#- Parameters                                                                 --
#- 1 : Test case number                                                       --
#- 2 : The decoding format in question                                        --
#- 3 : Possible post-processor                                                --
#-                                                                            --
#- Returns                                                                    --
#- 1 : - Decoder/post-processor output data does not match with reference     --
#-       data                                                                 --
#-     - Decoder/post-processor data missing                                  --
#-     - Reference data missing                                               --
#-     - Core dumped and SW crashed                                           --
#-     - Timeout occured                                                      --
#- 0 : Otherwise                                                              --
#-                                                                            --
#-------------------------------------------------------------------------------
checkPositiveCase()
{
    case_number=$1
    decoder=$2
    post_processor=$3
    
    ret_val="0"
    not_supported_tool_in_strm="0"
    if [ "$decoder" == "h264" ] && [ "$TB_H264_TOOLS_NOT_SUPPORTED" != "DISABLED" ]
    then
        tools_not_supported=`echo "$TB_H264_TOOLS_NOT_SUPPORTED" | tr " " "_"`
        tools_not_supported=`echo "$tools_not_supported" | tr "," " "`
        for tool_not_supported in $tools_not_supported
        do
            tool_not_supported=`echo "$tool_not_supported" | tr "_" " "`
            if [ -e "${TEST_DATA_HOME_RUN}/case_${case_number}/decoding_tools.trc" ]
            then
                not_supported_tool_in_strm_tmp=`grep -i "$tool_not_supported" ${TEST_DATA_HOME_RUN}/case_${case_number}/decoding_tools.trc | grep 1`
                if [ ! -z "$not_supported_tool_in_strm_tmp" ]
                then
                    not_supported_tool_in_strm="1"
                    comment="Case not supported due to ${tool_not_supported}"
                    return 0
                fi
            fi
        done
    fi
    if [ "$post_processor" == "pp" ] && [ "$not_supported_tool_in_strm" == "0" ]
    then
        checkPpCase "$case_number" "$decoder"
        ret_val=$?
    fi

    if [ "$decoder" == "h264" ] && [ "$post_processor" != "pp" ] && [ "$not_supported_tool_in_strm" == "0" ]
    then
        checkH264Case "$case_number"
        ret_val=$?
    fi

    if [ "$decoder" == "mpeg4" ] && [ "$post_processor" != "pp" ]
    then
        checkMpeg4Case "$case_number"
        ret_val=$?
    fi

    if [ "$decoder" == "vc1" ] && [ "$post_processor" != "pp" ]
    then
        checkVc1Case "$case_number"
        ret_val=$?
    fi

    if [ "$decoder" == "jpeg" ] && [ "$post_processor" != "pp" ]
    then
        checkJpegCase "$case_number"
        ret_val=$?
    fi
    
    if [ "$decoder" == "mpeg2" ] && [ "$post_processor" != "pp" ]
    then
        checkMpeg2Case "$case_number"
        ret_val=$?
    fi
    
    if [ "$decoder" == "real" ] && [ "$post_processor" != "pp" ]
    then
        checkRealCase "$case_number"
        ret_val=$?
    fi
    
    if [ "$decoder" == "vp6" ] && [ "$post_processor" != "pp" ]
    then
        checkVP6Case "$case_number"
        ret_val=$?
    fi
    
    if [ "$decoder" == "vp8" ] && [ "$post_processor" != "pp" ]
    then
        checkVP8Case "$case_number"
        ret_val=$?
    fi
    
    if [ "$decoder" == "avs" ] && [ "$post_processor" != "pp" ]
    then
        checkAVSCase "$case_number"
        ret_val=$?
    fi
    
    isCrashed "$case_number"
    if [ $? == 1 ]
    then
        comment="Core dumped and ${comment}"
        return 1
    fi
    
    isTBTimeout "$case_number"
    if [ $? == 1 ]
    then
        comment="TB timeout and ${comment}"
        return 1
    fi
    
    if [ "$not_supported_tool_in_strm" == "0" ]
    then
        isHWTimeout "$case_number"
        if [ $? == 1 ]
        then
            comment="HW timeout and ${comment}"
            return 1
        fi
    fi
    
    if [ "$ret_val" == 2 ] ||  [ "$ret_val" == 3 ]
    then
        return 1
    fi
    
    return $ret_val
}

#-------------------------------------------------------------------------------
#-                                                                            --
#- Checks the result of a test case that should not be bit exact, i.e., check --
#- whether case was crashed or timeout occured, and the result is not bit     -- 
#- exact.                                                                     --
#-                                                                            --
#- Parameters                                                                 --
#- 1 : Test case number                                                       --
#- 2 : The decoding format in question                                        --
#- 3 : Possible post-processor                                                --
#-                                                                            --
#- Returns                                                                    --
#- 3 : Otherwise                                                              --
#- 2 : - Decoder/post-processor output data does match with reference         --
#-       data                                                                 --
#- 1 : - Core dumped and SW crashed                                           --
#-     - Timeout occured                                                      --
#-                                                                            --
#-------------------------------------------------------------------------------
checkNegativeCase8170()
{
    case_number=$1
    decoder=$2
    post_processor=$3
    
    not_supported_tool_in_strm="0"
    if [ "$decoder" == "h264" ] && [ "$TB_H264_TOOLS_NOT_SUPPORTED" != "DISABLED" ]
    then
        tools_not_supported=`echo "$TB_H264_TOOLS_NOT_SUPPORTED" | tr " " "_"`
        tools_not_supported=`echo "$tools_not_supported" | tr "," " "`
        for tool_not_supported in $tools_not_supported
        do
            tool_not_supported=`echo "$tool_not_supported" | tr "_" " "`
            if [ -e "case_${case_number}/decoding_tools.trc" ]
            then
                not_supported_tool_in_strm_tmp=`grep -i "$tool_not_supported" case_${case_number}/decoding_tools.trc | grep 1`
                if [ ! -z "$not_supported_tool_in_strm_tmp" ]
                then
                    not_supported_tool_in_strm="1"
                    comment="Case not supported due to ${tool_not_supported}"
                    break
                fi
            fi
        done
    fi
    
    getCategory "$case_number"
    
    isCrashed "$case_number"
    if [ $? == 1 ]
    then
        #result2ScreenAndFile "$case_number" "FAILED" "Core dumped"
        comment="Core dumped"
        return 1
    fi
    
    isTBTimeout "$case_number"
    if [ $? == 1 ]
    then
        comment="TB timeout"
        return 1
    fi
    
    if [ "$not_supported_tool_in_strm" == "1" ]
    then
        return 3
    fi
    
    isHWTimeout "$case_number"
    if [ $? == 1 ]
    then
        comment="HW timeout"
        return 1
    fi
    
    # Do not check output for case 5085
    # Instead check that if enough pictures decoded
    if [ "$case_number" == 5085 ]
    then
        error_returned=`grep "PIC 239, type NON-IDR, decoded pic 240" case_${case_number}${TB_LOG_FILE_SUFFIX}`
        if [ -z "$error_returned" ]
        then
            comment="Not enough pictures decoded"
            return 2
        else
            comment="240 pictures decoded"
            return 3
        fi
    fi
    
    isTBTimeout "$case_number"
    if [ $? == 1 ]
    then
        #result2ScreenAndFile $"case_number" "FAILED" "Timeout"
        comment="Timeout"
        return 1
    fi
    
    # Do not check output for case 779
    # Instead check that if the API returns H264DEC_STRM_PROCESSED
    if [ "$case_number" == 779 ]
    then
        error_returned=`grep "TB: H264DecDecode returned: H264DEC_STRM_PROCESSED" case_${case_number}${TB_LOG_FILE_SUFFIX}`
        if [ -z "$error_returned" ]
        then
            comment="H264DEC_STRM_PROCESSED return value missing"
            return 2
        else
            comment="H264DEC_STRM_PROCESSED return value detected"
            return 3
        fi
    fi
    
    # Do not check output
    # Instead check that if the API returns H264DEC_STRM_ERROR
    if [ "$case_number" == 6575 ] || [ "$case_number" == 6578 ] || [ "$case_number" == 6581 ] || [ "$case_number" == 6582 ] || [ "$case_number" == 6587 ] || [ "$case_number" == 6589 ] || [ "$case_number" == 6592 ] || [ "$case_number" == 6593 ] || [ "$case_number" == 6595 ]
    then
        error_returned=`grep "TB: H264DecDecode returned: Other -2" case_${case_number}${TB_LOG_FILE_SUFFIX}`
        if [ -z "$error_returned" ]
        then
            comment="H264DEC_STRM_ERROR return value missing"
            return 2
        else
            comment="H264DEC_STRM_ERROR return value detected"
            return 3
        fi
    fi
    
    if [ "$case_number" == 6601 ]
    then
        error_returned=`grep "TB: H264DecDecode returned: Other -2" case_${case_number}${TB_LOG_FILE_SUFFIX}`
        if [ -z "$error_returned" ]
        then
            comment="H264DEC_STRM_ERROR return value missing"
            return 2
        else
            comment="H264DEC_STRM_ERROR return value detected"
            return 3
        fi
    fi
    
    # Do not check output for jpeg_error cases
    # Instead check if the API returns JPEG_STRM_ERROR
    if [ "$category" == "jpeg_420_error" ] && [ $case_number != 1024 ]
    then
        error_returned=`grep "TB: jpeg API returned : JPEGDEC_STRM_ERROR" case_${case_number}${TB_LOG_FILE_SUFFIX}`
        if [ -z "$error_returned" ]
        then
            comment="JPEGDEC_STRM_ERROR return value missing"
            return 2
        else
            comment="JPEGDEC_STRM_ERROR return value detected"
            return 3
        fi
        
    elif [ $case_number == 1024 ]
    then
        frame_ready=`grep "JPEG: JPEGDEC_FRAME_READY" case_${case_number}${TB_LOG_FILE_SUFFIX}`
        if [ -z "$frame_ready" ]
        then
            comment="JPEGDEC_FRAME_READY return value missing"
            return 2
        else
            comment="JPEGDEC_FRAME_READY return value detected"
            return 3
        fi
    fi
    
    # Do not check output for case 4405, 4416 and 4417
    # Instead check if the API returns VC1DEC_STRM_ERROR
    if [ "$case_number" == 4405 ] || [ "$case_number" == 4416 ]  || [ "$case_number" == 4417 ] 
    then
        error_returned=`grep "VC1DecDecode returned: VC1DEC_STRM_ERROR" case_${case_number}${TB_LOG_FILE_SUFFIX}`
        if [ -z "$error_returned" ]
        then
            comment="VC1DEC_STRM_ERROR return value missing"
            return 2
        else
            comment="VC1DEC_STRM_ERROR return value detected"
            return 3
        fi
    fi
    
    # Check that right picture fails for Allegro case
    getCategory "$case_number"
    if [ "$category" == "h264_bp_allegro" ] && [ $DEC_RLC_MODE_FORCED == "DISABLED" ]
    then
        checkH264Case "$case_number"
        #failing_pictures_vs_cases="6000:55 6001:55 6042:1 6043:1 6044:1 6045:1 6046:1 6047:1 6048:2 6049:2 6050:1 6051:1 6052:1 6053:1 6054:55 6068:1 6069:1 6070:1 6071:2 6072:55 6086:1 6087:1 6088:1 6089:2"
        failing_pictures_vs_cases="6000:53 6001:53 6042:1 6043:1 6044:1 6045:1 6046:1 6047:1 6048:2 6049:2 6050:1 6051:1 6052:1 6053:1 6054:54 6068:1 6069:1 6070:1 6071:2 6072:54 6086:1 6087:1 6088:1 6089:2"
        for failing_picture_vs_case in $failing_pictures_vs_cases
        do
            test_case=`echo $failing_picture_vs_case | awk -F : '{print $1}'`
            failing_picture=`echo $failing_picture_vs_case | awk -F : '{print $2}'`
            if [ "$test_case" == "$case_number" ]
            then
                if [ "$comment" == "Picture ${failing_picture} not bit exact" ]
                then
                    comment="${comment} (as expected)"
                    return 3
                else
                    return 2
                fi
            fi
        done
    fi
    
    ret_val=""
    
    if [ "$post_processor" == "pp" ]
    then
        checkPpCase "$case_number"
        ret_val=$?
    fi

    if [ "$decoder" == "h264" ] && [ "$post_processor" != "pp" ]
    then
        checkH264Case "$case_number"
        ret_val=$?
    fi

    if [ "$decoder" == "mpeg4" ]  && [ "$post_processor" != "pp" ]
    then
        checkMpeg4Case "$case_number"
        ret_val=$?
    fi

    if [ "$decoder" == "vc1" ]  && [ "$post_processor" != "pp" ]
    then
        checkVc1Case "$case_number"
        ret_val=$?
    fi

    if [ "$decoder" == "jpeg" ]  && [ "$post_processor" != "pp" ]
    then
        checkJpegCase "$case_number"
        ret_val=$?
    fi
    
    if [ "$decoder" == "mpeg2" ]  && [ "$post_processor" != "pp" ]
    then
        checkMpeg2Case "$case_number"
        ret_val=$?
    fi
    
    if [ "$ret_val" == 0 ]
    then
        #result2ScreenAndFile "$case_number" "NOT_OK"
        comment="Output data is bit exact with reference data"
        return 2
    
    # reference model is not configurable
    elif [ "$ret_val" == 3 ]
    then
        return 2
    
    else
        #result2ScreenAndFile "$case_number" "NOT_FAILED"
        return 3
    fi
}

#-------------------------------------------------------------------------------
#-                                                                            --
#- Checks the result of a test case that should not be bit exact, i.e., check --
#- whether case was crashed or timeout occured, and the result is not bit     -- 
#- exact.                                                                     --
#-                                                                            --
#- Parameters                                                                 --
#- 1 : Test case number                                                       --
#- 2 : The decoding format in question                                        --
#- 3 : Possible post-processor                                                --
#-                                                                            --
#- Returns                                                                    --
#- 3 : Otherwise                                                              --
#- 2 : - Decoder/post-processor output data does match with reference         --
#-       data                                                                 --
#- 1 : - Core dumped and SW crashed                                           --
#-     - Timeout occured                                                      --
#-                                                                            --
#-------------------------------------------------------------------------------
checkNegativeCase8190()
{
    case_number=$1
    decoder=$2
    post_processor=$3
    
    not_supported_tool_in_strm="0"
    if [ "$decoder" == "h264" ] && [ "$TB_H264_TOOLS_NOT_SUPPORTED" != "DISABLED" ]
    then
        tools_not_supported=`echo "$TB_H264_TOOLS_NOT_SUPPORTED" | tr " " "_"`
        tools_not_supported=`echo "$tools_not_supported" | tr "," " "`
        for tool_not_supported in $tools_not_supported
        do
            tool_not_supported=`echo "$tool_not_supported" | tr "_" " "`
            if [ -e "case_${case_number}/decoding_tools.trc" ]
            then
                not_supported_tool_in_strm_tmp=`grep -i "$tool_not_supported" case_${case_number}/decoding_tools.trc | grep 1`
                if [ ! -z "$not_supported_tool_in_strm_tmp" ]
                then
                    not_supported_tool_in_strm="1"
                    comment="Case not supported due to ${tool_not_supported}"
                    break
                fi
            fi
        done
    fi
    
    getCategory "$case_number"
    
    isCrashed "$case_number"
    if [ $? == 1 ]
    then
        #result2ScreenAndFile "$case_number" "FAILED" "Core dumped"
        comment="Core dumped"
        return 1
    fi
    
    isTBTimeout "$case_number"
    if [ $? == 1 ]
    then
        comment="TB timeout"
        return 1
    fi
    
    if [ "$not_supported_tool_in_strm" == "1" ]
    then
        return 3
    fi
    
    isHWTimeout "$case_number"
    if [ $? == 1 ]
    then
        comment="HW timeout"
        return 1
    fi
    
    if [ "$case_number" == 10057 ] || [ "$case_number" == 10058 ]
    then
        return 3
    fi
    
    # Do not check output
    # Instead check that if the API returns H264DEC_STRM_PROCESSED
    if [ "$case_number" == 779 ] || [ "$case_number" == 7514 ] || [ "$case_number" == 7512 ] || [ "$case_number" == 7513 ] || [ "$case_number" == 7415 ] \
             || [ "$case_number" == 7409 ] || [ "$case_number" == 7411 ] || [ "$case_number" == 7412 ]
    then
        error_returned=`grep "TB: H264DecDecode returned: H264DEC_STRM_PROCESSED" case_${case_number}${TB_LOG_FILE_SUFFIX}`
        if [ -z "$error_returned" ]
        then
            comment="H264DEC_STRM_PROCESSED return value missing"
            return 2
        else
            comment="H264DEC_STRM_PROCESSED return value detected"
            return 3
        fi
    fi
    
    # Do not check output
    # Instead check that if the API returns H264DEC_STRM_ERROR
    if [ "$case_number" == 6575 ] || [ "$case_number" == 6581 ] || [ "$case_number" == 6582 ] || [ "$case_number" == 6587 ] || [ "$case_number" == 6589 ] \
            || [ "$case_number" == 6592 ] || [ "$case_number" == 6593 ] || [ "$case_number" == 6595 ] || [ "$case_number" == "7508" ] || [ "$case_number" == "7515" ] \
            || [ "$case_number" == "7500" ] || [ "$case_number" == "7507" ] || [ "$case_number" == "7511" ] || [ "$case_number" == "7509" ] || [ "$case_number" == "7510" ] \
            || [ "$case_number" == "7417" ] || [ "$case_number" == "7405" ] || [ "$case_number" == "7404" ] || [ "$case_number" == "7406" ] || [ "$case_number" == "7410" ] \
            || [ "$case_number" == "7413" ] || [ "$case_number" == "7414" ]
    then
        error_returned=`grep "TB: H264DecDecode returned: Other -2" case_${case_number}${TB_LOG_FILE_SUFFIX}`
        if [ -z "$error_returned" ]
        then
            comment="H264DEC_STRM_ERROR return value missing"
            return 2
        else
            comment="H264DEC_STRM_ERROR return value detected"
            return 3
        fi
    fi
    
    if [ "$case_number" == 9006 ]
    then
        baseline_configuration=`echo "$HWCONFIGTAG" | grep "h264bp"`
        if [ ! -z "$baseline_configuration" ]
        then
            error_returned=`grep "TB: H264DecDecode returned: H264DEC_STREAM_NOT_SUPPORTED" case_${case_number}${TB_LOG_FILE_SUFFIX}`
            if [ -z "$error_returned" ]
            then
                comment="H264DEC_STREAM_NOT_SUPPORTED return value missing"
                return 2
            else
                comment="H264DEC_STREAM_NOT_SUPPORTED return value detected"
                return 3
            fi
        fi
    fi
    
    # Do not check output for jpeg_error cases
    # Instead check if the API returns JPEG_STRM_ERROR
    if [ "$category" == "jpeg_420_error" ]  || [ "$category" == "jpeg_444_error" ] || [ "$category" == "jpeg_411_error" ] && [ $case_number != 1024 ]
    then
        error_returned=`grep "TB: jpeg API returned : JPEGDEC_STRM_ERROR" case_${case_number}${TB_LOG_FILE_SUFFIX}`
        if [ -z "$error_returned" ]
        then
            comment="JPEGDEC_STRM_ERROR return value missing"
            return 2
        else
            comment="JPEGDEC_STRM_ERROR return value detected"
            return 3
        fi
        
    elif [ $case_number == 1024 ]
    then
        frame_ready=`grep "JPEG: JPEGDEC_FRAME_READY" case_${case_number}${TB_LOG_FILE_SUFFIX}`
        if [ -z "$frame_ready" ]
        then
            comment="JPEGDEC_FRAME_READY return value missing"
            return 2
        else
            comment="JPEGDEC_FRAME_READY return value detected"
            return 3
        fi
    fi
    
    # Do not check output for case 4405, 4416 and 4417
    # Instead check if the API returns VC1DEC_STRM_ERROR
    if [ "$case_number" == 4405 ] || [ "$case_number" == 4416 ]  || [ "$case_number" == 4417 ] 
    then
        error_returned=`grep "VC1DecDecode returned: VC1DEC_STRM_ERROR" case_${case_number}${TB_LOG_FILE_SUFFIX}`
        if [ -z "$error_returned" ]
        then
            comment="VC1DEC_STRM_ERROR return value missing"
            return 2
        else
            comment="VC1DEC_STRM_ERROR return value detected"
            return 3
        fi
    fi
    
    ret_val=""
    
    if [ "$post_processor" == "pp" ]
    then
        checkPpCase "$case_number"
        ret_val=$?
    fi

    if [ "$decoder" == "h264" ] && [ "$post_processor" != "pp" ]
    then
        checkH264Case "$case_number"
        ret_val=$?
    fi

    if [ "$decoder" == "mpeg4" ]  && [ "$post_processor" != "pp" ]
    then
        checkMpeg4Case "$case_number"
        ret_val=$?
    fi

    if [ "$decoder" == "vc1" ]  && [ "$post_processor" != "pp" ]
    then
        checkVc1Case "$case_number"
        ret_val=$?
    fi

    if [ "$decoder" == "jpeg" ]  && [ "$post_processor" != "pp" ]
    then
        checkJpegCase "$case_number"
        ret_val=$?
    fi
    
    if [ "$decoder" == "mpeg2" ]  && [ "$post_processor" != "pp" ]
    then
        checkMpeg2Case "$case_number"
        ret_val=$?
    fi
    
    if [ "$decoder" == "real" ]  && [ "$post_processor" != "pp" ]
    then
        checkRealCase "$case_number"
        ret_val=$?
    fi
    
    if [ "$decoder" == "vp6" ]  && [ "$post_processor" != "pp" ]
    then
        checkVP6Case "$case_number"
        ret_val=$?
    fi
    
    if [ "$decoder" == "vp8" ]  && [ "$post_processor" != "pp" ]
    then
        checkVP8Case "$case_number"
        ret_val=$?
    fi
    
    if [ "$decoder" == "avs" ]  && [ "$post_processor" != "pp" ]
    then
        checkAVSCase "$case_number"
        ret_val=$?
    fi
    
    if [ "$ret_val" == 0 ]
    then
        #result2ScreenAndFile "$case_number" "NOT_OK"
        comment="Output data is bit exact with reference data"
        return 2
    
    # reference model is not configurable
    elif [ "$ret_val" == 3 ]
    then
        return 2
    
    else
        #result2ScreenAndFile "$case_number" "NOT_FAILED"
        return 3
    fi
}

#-------------------------------------------------------------------------------
#-                                                                            --
#- Checks the result of a test case by selecting correct                      --
#- decoder/post-processor for the test case. Generates csv report, if         --
#- enabled.                                                                   --
#-                                                                            --
#- Parameters                                                                 --
#- 1 : Test case number                                                       --
#- 2 : Flag to decide whether to generate csv report                          --
#-                                                                            --
#- Returns                                                                    --
#- 1 : - Decoding format or post processor cannot be decided for the case     --
#-     - Tags cannot be decided for the case                                  --
#-     - Category cannot be decided for the case                              --
#- 0 : Otherwise                                                              --
#-                                                                            --
#-------------------------------------------------------------------------------
checkCase()
{
    case_number=$1
    generate_csv_report=$2
    use_failed=$3
    
    getComponent "$case_number"
    if [ $? == 1 ]
    then
        # fatal error
        return 1
    fi
    
    # output the case information
    printCase "$case_number" "$decoder" "$post_processor"
    
    # the reference data is generated on the fly
    case_to_be_executed=`grep -w "$case_number" test_cases 2> /dev/null`
    if [ "$TB_TEST_DATA_SOURCE" == "SYSTEM" ] && [ ! -z "$case_to_be_executed" ]
    then
        # wait for configuration files
        while [ ! -e "case_${case_number}.cfg" ] || [ ! -e "case_${case_number}_tb_params" ]
        do
            sleep 1
        done
        prepareCheckCase "$case_number" "$decoder" "$post_processor"
    fi
    while [ ! -e "case_${case_number}.done" ] && [ ! -z "$case_to_be_executed" ]
    do
        sleep 1
    done
    
    # check if the test case was executed
    executed=1
    if [ -z "$case_to_be_executed" ]
    then
        executed=0
        result2ScreenAndFile "$case_number" "NOT_EXECUTED" ""
        return 0
    fi
    
    isOutOfSpace "$case_number"
    if [ $? == 1 ]
    then
        comment="Out of disk space"
        executed=0
    else
        comment=""
    fi
    
    if [ "$executed" == 1 ]
    then
        negative_case="0"
        if [ "$PRODUCT" == "8170" ]
        then
            isNegativeCase8170 "$case_number" "$decoder" "$post_processor"
            negative_case=$?
            
        elif [ "$PRODUCT" == "8190" ] || [ "$PRODUCT" == "9170" ] || [ "$PRODUCT" == "9190" ] || [ "$PRODUCT" == "g1" ]
        then
            isNegativeCase8190 "$case_number" "$decoder" "$post_processor"
            negative_case=$?
        fi
        
        if [ $negative_case == 0 ]
        then
            checkPositiveCase "$case_number" "$decoder" "$post_processor"
            cmp_result=$?
            
        elif [ "$negative_case" == 1 ]
        then
            if [ "$PRODUCT" == "8170" ]
            then
                checkNegativeCase8170 "$case_number" "$decoder" "$post_processor"
                cmp_result=$?
            
            elif [ "$PRODUCT" == "8190" ] || [ "$PRODUCT" == "9170" ] || [ "$PRODUCT" == "9190" ] || [ "$PRODUCT" == "g1" ]
            then
                checkNegativeCase8190 "$case_number" "$decoder" "$post_processor"
                cmp_result=$?
            fi
        fi
    fi
    
    if [ "$executed" == 0 ]
    then
        test_status="NOT_EXECUTED"
    
    else
        if [ "$cmp_result" -eq 0 ]
        then
            test_status="OK"
            
        elif [ "$cmp_result" -eq 1 ]
        then
            test_status="FAILED"
        
        elif [ "$cmp_result" -eq 2 ]
        then
            test_status="NOT_OK"
    
        elif [ "$cmp_result" -eq 3 ]
        then
            test_status="NOT_FAILED"
            ## reset the comment
            #comment=""
        fi
    fi
    
    result2ScreenAndFile "$case_number" "$test_status" "$comment"
    
    getCategory "$case_number"
    if [ $? != 0 ]
    then
        # fatal error
        return 1
    fi
    
    getStatusDate "$case_number"
    getConfiguration "$case_number"
    getHwPerformance "$case_number"
    getSwPerformance "$case_number"
    getPpCaseWidth "$case_number"
    getExecutionTime "$case_number"
    if [ $pp_case_width == 0 ]
    then
        pp_case_width="N/A"
    fi
    getDecCaseWidthWithAwk "$case_number"
    if [ $dec_case_width == 0 ]
    then
        dec_case_width="N/A"
    fi
    ## if not executed
    #if [ "$executed" == 0 ]
    #then
    #    echo "case $case_number;$category;$TB_MAX_NUM_PICS;$dec_case_width;$pp_case_width;N/A;N/A;"$test_status";$HWCONFIGTAG;$SWTAG;N/A;N/A;N/A;N/A;N/A;$comment;" >> "report.csv"
    #    return 0
    #fi
    
    if [ "$use_failed" == 0 ]
    then
        #echo "case $case_number;$category;$TB_MAX_NUM_PICS;$dec_case_width;$pp_case_width;$timeform1 $timeform2;$execution_time;"$test_status";$HWCONFIGTAG;$SWTAG;$configuration;$hw_performance;$sw_performance;$comment;" >> "report.csv"
        sed "/case ${case_number};/d" report.csv > tmp.report
        mv tmp.report report.csv
        echo "case $case_number;$category;$TB_MAX_NUM_PICS;$dec_case_width;$pp_case_width;$timeform1 $timeform2;$execution_time;"$test_status";$HWCONFIGTAG;$SWTAG;$configuration;$hw_performance;$sw_performance;$comment;" >> "report.csv"
        if [ "$generate_csv_report" == 1 ]
        then
            if [ "$TB_TEST_DB" == "ENABLED" ]
            then
                addResult2TestDB "$case_number" "$test_status" "${timeform1} ${timeform2}" "$execution_time" "$configuration" "$comment" "$TB_MAX_NUM_PICS"
            fi
            #NOT_FAILED->OK->NOT_OK->FAILED
            cd ../test_reports
            merged_report="integrationreport_${HWCONFIGTAG}_${SWTAG}_merged.csv"
            previous_test_status=`grep -w "case ${case_number}" "$merged_report" | awk -F \; '{print $8}'`
            update_test_status=0
            if [ "$test_status" == "NOT_FAILED" ] && [[ "$previous_test_status" != "NOT_FAILED" && "$previous_test_status" != "OK" && "$previous_test_status" != "NOT_OK" && "$previous_test_status" != "FAILED" ]]
            then
                update_test_status=1
            fi
            if [ "$test_status" == "OK" ] && [[ "$previous_test_status" != "OK" && "$previous_test_status" != "NOT_OK" && "$previous_test_status" != "FAILED" ]]
            then
                update_test_status=1
            fi
            if [ "$test_status" == "NOT_OK" ] && [[ "$previous_test_status" != "NOT_OK" && "$previous_test_status" != "FAILED" ]]
            then
                update_test_status=1
            fi
            if [ "$test_status" == "FAILED" ] && [ "$previous_test_status" != "FAILED" ]
            then
                update_test_status=1
            fi
            if [ "$update_test_status" == 1 ]
            then
                sed "/case ${case_number};/d" "$merged_report" > tmp.report
                mv tmp.report "$merged_report"
                echo "case $case_number;$category;$TB_MAX_NUM_PICS;$dec_case_width;$pp_case_width;$timeform1 $timeform2;$execution_time;"$test_status";$HWCONFIGTAG;$SWTAG;$configuration;$hw_performance;$sw_performance;$comment;" >> "$merged_report"
            fi
            cd - >> /dev/null
        fi
    else
        sed "/case ${case_number};/d" report.csv > tmp.report
        mv tmp.report report.csv
        echo "case $case_number;$category;$TB_MAX_NUM_PICS;$dec_case_width;$pp_case_width;$timeform1 $timeform2;$execution_time;"$test_status";$HWCONFIGTAG;$SWTAG;$configuration;$hw_performance;$sw_performance;$comment;The case was rerun" >> "report.csv"
    fi
            
    # Clean-up results
    if [ "$test_status" == "OK" ] || [ "$test_status" == "NOT_FAILED" ] && [ "$TB_REMOVE_RESULTS" == "ENABLED" ]
    then
        while [ 1 ]
        do
            if [ ! -e test_exec_status ]
            then
                break
            fi
            script_done=`grep -w "${case_number} DONE" test_exec_status `
            if [ ! -z "$script_done" ]
            then
                break
            fi
            sleep 1
        done
        rm -rf case_${case_number}
        rm -f case_${case_number}.*
        rm -f case_${case_number}_*
        rm -f repeat_${case_number}.sh
        rm -f swreg_${case_number}.trc
        rm -f "2nd_chroma_case_${case_number}.yuv"
        test_cases=`grep -vw "$case_number" test_cases`
        echo "$test_cases" > test_cases
        is_empty=`cat test_cases`
        if [ -z "$is_empty" ]
        then
            rm -f test_cases
            rm -f test_cases_printed
        fi
    fi
    
    if [ "$test_status" == "FAILED" ] || [ "$test_status" == "NOT_OK" ] && [ "$use_failed" == 0 ]
    then
        echo -n "${test_case} " >> test_cases_failed
    fi
    
    return 0
}


#-------------------------------------------------------------------------------
#-                                                                            --
#- Checks the results of all the test cases in the provided test case set.    --
#- Generates csv report, if enabled.                                          --
#-                                                                            --
#- Parameters                                                                 --
#- 1 : Test cases                                                             --
#- 2 : Flag to decide whether to generate csv report                          --
#-                                                                            --
#-------------------------------------------------------------------------------
checkCases()
{
    test_cases=$1
    generate_csv_report=$2
    use_failed=$3
    
    # always create report locally
    if [ "$use_failed" == 0 ]
    then
        rm -f report.csv
        
        if [ -e ../report_cache.csv ]
        then
            cp ../report_cache.csv report.csv
        else
            echo "Internal error: report cache missing"
            exit 1
            #echo "Test Case;Category;Pictures;Decoder Picture Width;PP Picture Width;Status Date;Execution Time;Status;HW Tag;SW Tag;Configuration;Decoder HW Performance (frame average 10^-5s);PP HW Performance (frame average 10^-5s);Decoder SW Performance (full decode);PP SW Performance (full decode);Comments;" >> "report.csv"
        fi
        if [ "$generate_csv_report" == 1 ]
        then
            merged_report="integrationreport_${HWCONFIGTAG}_${SWTAG}_merged.csv"
            cd ../test_reports
            if [ ! -e "$merged_report" ]
            then
                cp ../report_cache.csv "$merged_report"
            fi
            cd - >> /dev/null
        fi
    fi
    
    # wait for run script
    if [ "$use_failed" == 0 ]
    then
        echo -n "Waiting test cases to be run... "
        while [ ! -e test_cases_printed ] 
        do
            sleep 1
        done
        echo "OK"
    else
        echo -n "Waiting failed test cases to be run... "
        while [ ! -e test_cases_printed_failed ] 
        do
            sleep 1
        done
        echo "OK"
    fi
    
    if [ "$TB_RND_TC" == "ENABLED" ] && [ -e test_cases_rnd ] && [ "$use_failed" == 0 ]
    then
        test_cases_rnd=`head -n 1 test_cases_rnd`
        test_cases_all="$test_cases"
        test_cases_all_arr=($test_cases_all)
        test_cases=""
        for test_case_rnd in $test_cases_rnd
        do
            let 'index=0'
            for test_case_all in $test_cases_all
            do
                # test case is to be checked
                if [ "$test_case_rnd" == "$test_case_all" ]
                then
                    test_cases="${test_cases} ${test_case_rnd}"
                    unset test_cases_all_arr[$index]
                    test_cases_all_arr=(${test_cases_all_arr[*]})
                    test_cases_all=${test_cases_all_arr[*]}
                    break
                fi
                let 'index+=1'
            done
        done
        # add not executed test cases at the end
        test_cases="${test_cases} ${test_cases_all}"
        #echo "test_cases: ${test_cases}"
    fi
    
    # check the test cases
    for test_case in $test_cases
    do
        checkCase "$test_case" "$generate_csv_report" "$use_failed"
        if [ $? != 0 ]
        then
            # fatal error
            exit 1
        fi
    done

    # if csv reporting is enabled, copy the report to destination file and make link
    # back from it
    if [ "$generate_csv_report" == 1 ] && [ "$use_failed" == 0 ]
    then
        cp report.csv "$CSV_FILE"
    fi
    if [ "$generate_csv_report" == 1 ] && [ "$use_failed" == 1 ]
    then
        file_prefix=`echo ${CSV_FILE} | awk -F \. '{print $1}'`
        file_prefix="${file_prefix}_failed_rerun"
        file_suffix=`echo ${CSV_FILE} | awk -F \. '{print $2}'`
        cp report.csv "${file_prefix}.${file_suffix}"
    fi
    if [ "$use_failed" == 0 ]
    then
        cp report.csv report_repeat.csv
    fi
}
