#!/bin/bash
#Copyright 2013 Google Inc. All Rights Reserved.
#Author: mkarki@google.com (Matti Kärki)
#
#Description: Set-up test cases for HW decoder FPGA testing.

. ./defs.sh

# nalunit is variant of packet (it could also be variant of frame)
HEVC_SUPPORTED_TEST_PARAMS_DECODE="frame packet nalunit full"
VP9_SUPPORTED_TEST_PARAMS_DECODE="frame"
SUPPORTED_TEST_PARAMS_OUTPUT="tiled raster"
SUPPORTED_TEST_PARAMS_OUTPUT_WITHOUT_PP="tiled"

WIDTH_1080p=1920
HEIGHT_1080p=1088
WIDTH_2160p=4096
HEIGHT_2160p=2304
MIN_WIDTH=180
MIN_HEIGHT=180
MIN_MAX_BURST_LENGTH=1
MAX_MAX_BURST_LENGTH=128

g_decode_modes="frame"
g_output_formats="tiled"
g_burst_values="default"
g_error_params=""
g_run_without_error="1"
g_test_cases=""
g_frame_limit="0"
g_exclude_error_size=0

printHelp()
{
    echo -ne '\E[32m'
    echo ""
    echo "Command"
    tput sgr0
    echo "${0} [options]"
    echo ""
    echo -ne '\E[32m'
    echo "Options"
    tput sgr0
    echo "  -c \"<test_cases>\""
    echo "    in=<included_cases>"
    echo "      . test cases to be included, comma separated"
    echo "      . category name or part of it, and/or case number"
    echo "      . or directory on export"
    echo "    ex=<excluded_cases>"
    echo "      . test cases to be excluded, comma separated"
    echo "      . category name or part of it, and/or case number"
    echo "    li=<limits>"
    echo "      . limits of test cases, comma separated"
    echo "      . limit test cases by width: add '_w' after value"
    echo "      . limit test cases by frames: add '_f' after value"
    echo "    so=<sort_order>"
    echo "      . sort order of test cases, comma separated"
    echo "      . size: sort by size (frames x width x height)"
    echo "      . dimension: sort by frame dimension (width x height)"
    echo "      . frames: sort by frame count"
    echo "      . time: sort by previous execution time"
    echo "      . random: random order of test cases"
    echo "  -p \"<test_params>\""
    echo "    dm=<decode_mode>"
    echo "      . decoding mode, comma separated"
    echo "      . frame: frame by frame decode"
    echo "      . packet: packetize input bitstream"
    echo "      . nalunit: packet without start codes"
    echo "      . full: full-stream in single buffer"
    echo "      . random: random for each test case"
    echo "    of=<output_format>"
    echo "      . format of the output picture, comma separated"
    echo "      . tiled"
    echo "      . raster"
    echo "      . random: random for each test case"
    echo "    dc=<device_configuration>"
    echo "      . device configuration values, comma separated"
    echo "      . max burst length: add '_b' after value"
    echo "      . valid values are: 1-128, or random"
    echo "      . range can be given (changed after each case), e.g., 1-28_b"
    echo "  -e \"<error_params>\""
    echo "    packet here is defined by dm, e.g., if dm=frame, then packet here is a frame"
    echo "    random can be also given (i.e., random for each test case)"
    echo "    sr=<seed_for_random>"
    echo "      . seed for error randomization"
    echo "    pl=<packet_loss>"
    echo "      . odds to lose a packet, e.g., 1:5"
    echo "    bs=<bit_swap>"
    echo "      . odds to corrupt a bit in a packet, e.g., 1:100"
    echo "    st=<stream_truncate>"
    echo "      . truncate a packet"
    echo "      . 1 for enable, 0 for disable"
    echo "    re=<run_without_error>"
    echo "      . run case without errors after error injection run"
    echo "      . 1 for enable (default), 0 for disable"
    echo "      . no random can be given"
    echo "  -h print this help"
    echo ""
    echo -ne '\E[32m'
    echo "Examples"
    tput sgr0
    echo "${0} -c \"in=vp9,hevc ex=tiles_error,error_size li=1088_w,100_f so=time\" -p \"dm=frame,full of=tiled,raster dc=random_b\""
    #exit 0
}

parseTestParams()
{
    local test_params=$1
    for test_param in $test_params
    do
        local key=`echo $test_param |awk -F\= '{print $1}'`
        local values=`echo $test_param |awk -F\= '{print $2}' |tr "," " "`
        if [ "$key" == "dm" ]
        then
            g_decode_modes=""
            for value in $values
            do
                if [ "$value" != "frame" ] && [ "$value" != "full" ] && [ "$value" != "packet" ] && [ "$value" != "nalunit" ] && [ "$value" != "random" ]
                then
                    printErrorAndExit "Not valid test parameter value: ${value}"
                fi
                g_decode_modes="${g_decode_modes}${value} "
            done
            if [ -z "$g_decode_modes" ]
            then
                printErrorAndExit "No value for test parameter \"dm\""
            fi
            
        elif [ "$key" == "of" ]
        then
            g_output_formats=""
            for value in $values
            do
                if [ "$value" != "tiled" ] && [ "$value" != "raster" ] && [ "$value" != "random" ]
                then
                    printErrorAndExit "Not valid test parameter value: ${value}"
                fi
                g_output_formats="${g_output_formats}${value} "
            done
            if [ -z "$g_output_formats" ]
            then
                printErrorAndExit "No value for test parameter \"of\""
            fi
            
        elif [ "$key" == "dc" ]
        then
            declare -a values_arr=($values)
            local values_size=${#values_arr[@]}
            if [ $values_size -ne 1 ]
            then
                printErrorAndExit "No multiple values for \"dc\" test parameter"
            fi
            local is_burst=`echo "$values" |grep -c "_b"`
            if [ $is_burst -ne 1 ]
            then
                printErrorAndExit "Not valid test parameter value for dc: ${values}"
            fi
            local burst_value=`echo "$values" |tr -d "_b"`
            g_burst_values=""
            if [ "$burst_value" == "random" ]
            then
                g_burst_values="$burst_value"
            else
                local is_burst_range=`echo "$burst_value" |grep -c "-"`
                local min_burst=""
                local maxburst=""
                if [ $is_burst_range -eq 0 ]
                then
                    min_burst=$burst_value
                    max_burst=$burst_value
                elif [ $is_burst_range -eq 1 ]
                then
                    min_burst=`echo $burst_value |awk -F\- '{print $1}'`
                    max_burst=`echo $burst_value |awk -F\- '{print $2}'`
                else
                    printErrorAndExit "Not valid test parameter value for dc: ${values}"
                fi
                isNumber $min_burst
                if [ $? -eq 0 ]
                then
                    printErrorAndExit "Not valid test parameter value for dc: ${values}"
                fi
                isNumber $max_burst
                if [ $? -eq 0 ]
                then
                    printErrorAndExit "Not valid test parameter value for dc: ${values}"
                fi
                if [ $min_burst -lt $MIN_MAX_BURST_LENGTH ] || [ $min_burst -gt $MAX_MAX_BURST_LENGTH ]
                then
                    printErrorAndExit "Not valid test parameter value for dc: ${values}"
                fi
                if [ $max_burst -lt $MIN_MAX_BURST_LENGTH ] || [ $max_burst -gt $MAX_MAX_BURST_LENGTH ] || [ $max_burst -lt $min_burst ]
                then
                    printErrorAndExit "Not valid test parameter value for dc: ${values}"
                fi
                while [ $min_burst -le $max_burst ]
                do
                    g_burst_values="${g_burst_values}${min_burst} "
                    let 'min_burst=min_burst+1'
                done
            fi
           
        else
            printErrorAndExit "Not valid test parameter: ${key}"
        fi
    done
}

getTestCases()
{
    local test_case=$1
    
    g_function_return=""
    
    # directory mode
    if [ -d ${LOCAL_HOST_EXPORT_PREFIX}/${test_case} ]
    then
        file_extensions="${HEVC_FILE_EXTENSIONS} ${VP9_FILE_EXTENSIONS}"
        for file_ext in $file_extensions 
        do
            files=`ls ${LOCAL_HOST_EXPORT_PREFIX}/${test_case}/*.${file_ext} 2>> /dev/null`
            g_function_return="${g_function_return}${files} "
        done
        return
    fi
    
    # number mode
    isNumber "$test_case"
    if [ $? -eq 1 ]
    then
        g_function_return="$test_case"
        return
    fi
    
    # category mode
    local categories=""
    for category in $category_list_swhw
    do
        local include=`echo "$category" |grep "$test_case"`
        if [ ! -z "$include" ] 
        then
            eval test_case_numbers=\$$category 
            g_function_return="${g_function_return}${test_case_numbers} "
        fi
    done
}

includeTestCases()
{
    local test_cases=$1
    
    for test_case in $test_cases
    do
         getTestCases "$test_case"
         g_test_cases="${g_test_cases}${g_function_return} "
    done
}

excludeTestCases()
{
    local test_cases=$1
    
    if [ -z "$test_cases" ]
    then
        return
    fi
    
    local error_size_categories=`echo $test_cases |tr " " "\n" |grep -c error_size`
    if [ $error_size_categories -ge 1 ]
    then
        g_exclude_error_size=1
    fi
    
    local exclude_cases=""
    for test_case in $test_cases
    do
         getTestCases "$test_case"
         exclude_cases="${exclude_cases}${g_function_return} "
    done
    local final_cases=""    
    for test_case_number in $g_test_cases
    do
        exclude=0
        for exclude_case in $exclude_cases
        do
            if [ $test_case_number -eq $exclude_case ]
            then
                exclude=1
                break
            fi
        done
        if [ $exclude -eq 0 ]
        then
            final_cases="${final_cases}${test_case_number} "
        fi
    done

    g_test_cases="$final_cases"
}

limitTestCases()
{
    local limit=$1
    
    if [ -z "$limit" ]
    then
        return
    fi
    
    local final_cases=""
    for test_case in $g_test_cases
    do
        local test_case_width=${case_array[$test_case]}
        local test_case_width=`echo "$test_case_width" |awk -F\; '{print $2}' |awk -F\x '{print $1}'`
        if [ $test_case_width -le $limit ]
        then
            final_cases="${final_cases}${test_case} "
        fi
    done
    
    g_test_cases="$final_cases"
}

addSortKeySize()
{
    local test_cases=""
    for test_case in $g_test_cases
    do
        # test_case might be suffixed with _
        local test_case_id=`echo "$test_case" |awk -F\_ '{print $1}'`
        local width=`echo ${case_array[$test_case_id]} |awk -F\; '{print $2}' |awk -F\x '{print $1}'`
        local height=`echo ${case_array[$test_case_id]} |awk -F\; '{print $2}' |awk -F\x '{print $2}'`
        local frames=`grep -w $test_case_id $TEST_CASE_FRAMES_FILE |awk '{print $2}'`
        if [ -z "$width" ] || [ -z "$height" ]
        then
             printErrorAndExit "Sorting by size not possible: dimension missing for test case ${test_case}"
        fi
        if [ -z "$frames" ]
        then
             printErrorAndExit "Sorting by size not possible: frames missing for test case ${test_case}"
        fi
        local size=""
        let 'size=frames*width*height'
        test_cases="${test_cases}${test_case}_${size} "
    done
    g_test_cases="$test_cases"
}

addSortKeyDimension()
{
    local test_cases=""
    for test_case in $g_test_cases
    do
        # test_case might be suffixed with _
        local test_case_id=`echo "$test_case" |awk -F\_ '{print $1}'`
        local width=`echo ${case_array[$test_case_id]} |awk -F\; '{print $2}' |awk -F\x '{print $1}'`
        local height=`echo ${case_array[$test_case_id]} |awk -F\; '{print $2}' |awk -F\x '{print $2}'`
        if [ -z "$width" ] || [ -z "$height" ]
        then
             printErrorAndExit "Sorting by dimension not possible: dimension missing for test case ${test_case}"
        fi
        local dimension=""
        let 'dimension=width*height'
        test_cases="${test_cases}${test_case}_${dimension} "
    done
    g_test_cases="$test_cases"
}

addSortKeyFrames()
{
    local test_cases=""
    for test_case in $g_test_cases
    do
        # test_case might be suffixed with _
        local test_case_id=`echo "$test_case" |awk -F\_ '{print $1}'`
        local frames=`grep -w $test_case_id $TEST_CASE_FRAMES_FILE |awk '{print $2}'`
        if [ -z "$frames" ]
        then
             printErrorAndExit "Sorting by frames not possible: frames missing for test case ${test_case}"
        fi
        test_cases="${test_cases}${test_case}_${frames} "
    done
    g_test_cases="$test_cases"
}

addSortKeyTime()
{
    local test_cases=""
    for test_case in $g_test_cases
    do
        # test_case might be suffixed with _
        local test_case_id=`echo "$test_case" |awk -F\_ '{print $1}'`
        local time=`grep -w $test_case_id ${LOCAL_HOST_EXPORT_PREFIX}/${TEST_DATA_EXPORT_DIR}/${TEST_CASE_TIMES_FILE} |awk '{print $2}'`
        # if missing, set to zer0 to execute first
        if [ -z "$time" ]
        then
            time=0
        else
            # add one that missing cases are executed first
            let 'time=time+1'            
        fi
        test_cases="${test_cases}${test_case}_${time} "
    done
    g_test_cases="$test_cases"
}

sortTestCases()
{
    local sort_orders=$1
    
    local sort_keys=""
    local keys=0
    for sort_order in $sort_orders
    do
        if [ "$sort_order" != "size" ] && [ "$sort_order" != "frames" ] && [ "$sort_order" != "dimension" ] && [ "$sort_order" != "time" ]
        then
            printErrorAndExit "Not implemented sort order: ${sort_order}"
        fi
        if [ "$sort_order" == "size" ]
        then
            addSortKeySize
            
        elif [ "$sort_order" == "dimension" ]
        then
             addSortKeyDimension
             
        elif [ "$sort_order" == "frames" ]
        then
             addSortKeyFrames
             
        elif [ "$sort_order" == "time" ]
        then
             addSortKeyTime
        fi
        
        let 'keys=keys+1'
        if [ $keys -eq 1 ]
        then
            sort_keys="-k 2"
        else
            let key=keys+1
            sort_keys="${sort_keys} -k ${key}"
        fi
    done
    if [ $keys -eq 0 ]
    then
        return
    fi
    g_test_cases=`echo "$g_test_cases" |tr " " "\n" |sort -t _ ${sort_keys} -g |awk -F\_ '{print $1}'`
}

parseTestCases()
{
    local test_cases=$1
    local include_cases=""
    local exclude_cases=""
    local width_limit=""
    local sort_order=""
    
    for test_case in $test_cases
    do
        local key=`echo $test_case |awk -F\= '{print $1}'`
        local values=`echo $test_case |awk -F\= '{print $2}' |tr "," " "`
        if [ "$key" == "in" ]
        then
            for value in $values
            do
               include_cases="${include_cases}${value} "
            done
            
        elif [ "$key" == "ex" ]
        then
            for value in $values
            do
                exclude_cases="${exclude_cases}${value} "
            done
        
        elif [ "$key" == "li" ]
        then
            #declare -a values_arr=($values)
            #local values_size=${#values_arr[@]}
            #if [ $values_size -ne 2 ]
            #then
            #    printErrorAndExit "Not multiple values for \"li\" test parameter"
            #fi
            for value in $values
            do
               local is_width=`echo "$value" |grep -c "_w"`
               local is_frames=`echo "$value" |grep -c "_f"`
               if [ $is_width -eq 1 ]
               then
                   width_limit=`echo $value |awk -F_ '{print $1}'`
                   isNumber "$width_limit"
                   if [ $? -eq 0 ]
                   then
                       printErrorAndExit "Not valid test parameter value for li: ${value}"
                   fi
               elif [ $is_frames -eq 1 ]
               then
                   g_frame_limit=`echo $value |awk -F_ '{print $1}'`
                   isNumber "$g_frame_limit"
                   if [ $? -eq 0 ]
                   then
                       printErrorAndExit "Not valid test parameter value for li: ${value}"
                   fi
               else
                   printErrorAndExit "Not valid test parameter value for li: ${value}"
               fi
            done
            
        elif [ "$key" == "so" ]
        then
            for value in $values
            do
                if [ "$value" != "size" ] && [ "$value" != "frames" ] && [ "$value" != "dimension" ] && [ "$value" != "time" ] && [ "$value" != "random" ]
                then
                    printErrorAndExit "Not valid test parameter value for so: ${value}"
                fi
                sort_orders="${sort_orders}${value} "
            done
        else
            printErrorAndExit "Not valid test case parameter: ${key}"
        fi
    done
    
    if [ -z "$include_cases" ]
    then
        printErrorAndExit "Test cases must be given!"
    fi
    includeTestCases "$include_cases"
    if [ -z "$g_test_cases" ] || [ "$g_test_cases" == " " ]
    then
        printErrorAndExit "No test cases found!"
    fi
    excludeTestCases "$exclude_cases"
    limitTestCases "$width_limit"
    sortTestCases "$sort_orders"
}

parseErrorParams()
{
    local error_params=$1
    #local seed=""
    #local packet_loss=""
    #local bit_swap=""
    #local stream_truncate=""
    
    for error_param in $error_params
    do
        local key=`echo $error_param |awk -F\= '{print $1}'`
        if [ "$key" != "sr" ] && [ "$key" != "pl" ] && [ "$key" != "bs" ] && [ "$key" != "st" ] && [ "$key" != "re" ]
        then
            printErrorAndExit "Not a valid error parameter: $key"
        fi
        local values=`echo $error_param |awk -F\= '{print $2}' |tr "," " "`
        declare -a values_arr=($values)
        local values_size=${#values_arr[@]}
        if [ $values_size -ne 1 ]
        then
            printErrorAndExit "\"$key\" error parameter must have one value"
        fi
        if [ "$key" == "sr" ]
        then
            isNumber $values
            if [ $? -eq 0 ] && [ "$values" != "random" ]
            then
                printErrorAndExit "Not valid error parameter value for sr: ${values}"
            fi
            
        elif [ "$key" == "pl" ]
        then
            if [ "$values" == "random" ]
            then
                echo "" >> /dev/null
            else
                local odds_values=`echo "$values" |tr ":" " "`
                declare -a odds_values_arr=($odds_values)
                local odds_values_size=${#odds_values_arr[@]}
                if [ $odds_values_size -ne 2 ]
                then
                    printErrorAndExit "Not valid error parameter value for pl: ${values}"
                fi
                for odds_value in $odds_values
                do
                    isNumber $odds_value
                    if [ $? -eq 0 ]
                    then
                        printErrorAndExit "Not valid error parameter value for pl: ${values}"
                    fi  
                done
            fi
        
        elif [ "$key" == "bs" ]
        then
            if [ "$values" == "random" ]
            then
                echo "" >> /dev/null
            else
                local odds_values=`echo "$values" |tr ":" " "`
                declare -a odds_values_arr=($odds_values)
                local odds_values_size=${#odds_values_arr[@]}
                if [ $odds_values_size -ne 2 ]
                then
                    printErrorAndExit "Not valid error parameter value for bs: ${values}"
                fi
                for odds_value in $odds_values
                do
                    isNumber $odds_value
                    if [ $? -eq 0 ] && [ "$values" != "random" ]
                    then
                        printErrorAndExit "Not valid error parameter value for bs: ${values}"
                    fi  
                done
            fi
            
        elif [ "$key" == "st" ]
        then
            isNumber $values
            if [ $? -eq 0 ] && [ "$values" != "random" ]
            then
                printErrorAndExit "Not valid error parameter value for st: ${values}"
            fi
            if  [ "$values" != "random" ] && [ $values -ne 0 ] && [ $values -ne 1 ]
            then
                printErrorAndExit "Not valid error parameter value for st: ${values}"
            fi
            
        elif [ "$key" == "re" ]
        then
            isNumber $values
            if [ $? -eq 0 ]
            then
                printErrorAndExit "Not valid error parameter value for re: ${values}"
            fi
            if [ $values -ne 0 ] && [ $values -ne 1 ]
            then
                printErrorAndExit "Not valid error parameter value for re: ${values}"
            fi
            g_run_without_error="$values"
            
        else
            printErrorAndExit "Not valid error parameter: ${key}"
        fi
    done
    g_error_params="$error_params"
}


randomValueFromList()
{
    declare -a values_arr=($1)
    local values_size=${#values_arr[@]}
    local random_value=$RANDOM; let 'random_value%=values_size'
    g_function_return="${values_arr[$random_value]}"
}

setUpExecutionLists()
{
    local wdirs="$2"
    declare -a board_ips_arr=($1)
    declare -a wdirs_arr=($wdirs)
    declare -a features_arr=($3)
    local board_ips_size=${#board_ips_arr[@]}
    local wdirs_size=${#wdirs_arr[@]}
    local features_size=${#features_arr[@]}
    local max_burst_length_index=0
    
    if [[ $board_ips_size -ne $wdirs_size || $board_ips_size -ne $features_size ]]
    then
        printErrorAndExit "Internal error @ setUpExecutionLists(): $board_ips_size -ne $wdirs_size || $board_ips_size -ne $features_size"
    fi
    
    rm -f $EXECUTION_LIST.*
    for test_case in $g_test_cases
    do
        local category=""
        local test_case_width=""
        local test_case_height=""
        local format=""
        if [ -e $test_case ]
        then
            # assume supported stream    
            test_case_width="$MIN_WIDTH"
            test_case_height="$MIN_HEIGHT"
            # remove LOCAL_HOST_EXPORT_PREFIX from stream directory
            test_case=`echo $test_case |awk -F\${LOCAL_HOST_EXPORT_PREFIX} '{print $NF}'`
            local file_ext=`echo $test_case |awk -F\/ '{print $NF}' |awk -F\. '{print $NF}'`
            local is_hevc=`echo $HEVC_FILE_EXTENSIONS |tr " " "\n" |grep -w -c $file_ext`
            if [ $is_hevc -ne 0 ]
            then
                category="hevc_external"
            else
                local is_vp9=`echo $VP9_FILE_EXTENSIONS |tr " " "\n" |grep -w -c $file_ext`
                if [ $is_vp9 -ne 0 ]
                then
                    category="vp9_external"
                fi
            fi
            if [ -z "$category" ]
            then
                printErrorAndExit "Internal error @ setUpExecutionLists(): Cannot decide format!"
            fi
        else
            getCategory "$test_case"
            category="$g_function_return"
            local test_case_info=${case_array[$test_case]}
            test_case_width=`echo "$test_case_info" |awk -F\; '{print $2}' |awk -F\x '{print $1}'`
            test_case_height=`echo "$test_case_info" |awk -F\; '{print $2}' |awk -F\x '{print $2}'`
        fi
        format=`echo "$category" |awk -F_ '{print $1}'`
        local supported_test_parameters=""
        if [ "$format" == "hevc" ]
        then
             supported_test_parameters="$HEVC_SUPPORTED_TEST_PARAMS_DECODE $SUPPORTED_TEST_PARAMS_OUTPUT"
        elif [ "$format" == "vp9" ]
        then
             supported_test_parameters="$VP9_SUPPORTED_TEST_PARAMS_DECODE $SUPPORTED_TEST_PARAMS_OUTPUT"
        else
            printErrorAndExit "Internal error @ setUpExecutionLists(): unknown format ${format}"
        fi
        for decode_mode in $g_decode_modes
        do
            if [ "$decode_mode" == "random" ]
            then
                if [ "$format" == "hevc" ]
                then
                      randomValueFromList "$HEVC_SUPPORTED_TEST_PARAMS_DECODE"
                      decode_mode="$g_function_return"
                elif [ "$format" == "vp9" ]
                then
                      randomValueFromList "$VP9_SUPPORTED_TEST_PARAMS_DECODE"
                      decode_mode="$g_function_return"
                fi
            fi
            for output_format in $g_output_formats
            do
                if [ "$output_format" == "random" ]
                then
                    # check if we can randomize with all output formats
                    local current_tester=0
                    local valid_tester=0
                    while [ $current_tester -lt $board_ips_size ]
                    do
                        local features="${features_arr[$current_tester]}"
                        local is_pp=`echo $features |tr "," "\n" |grep -c -w "pp"`
                        if [ $is_pp -gt 1 ]
                        then
                            printErrorAndExit "Internal error @ test.sh:setUpExecutionLists(): multiple PPs found!"
                        fi
                        local is_2160p=`echo $features |tr "," "\n" |grep -c -w "2160p"`
                        local width_limit=""
                        local height_limit=""
                        if [ $is_2160p -eq 0 ]
                        then
                            local is_1080p=`echo $features |tr "," "\n" |grep -c -w "1080p"`
                            if [ $is_1080p -eq 0 ]
                            then
                                printErrorAndExit "Internal error @ test.sh:setUpExecutionLists(): no width configuration found!"
                            fi
                            width_limit=$WIDTH_1080p
                        else
                            width_limit=$WIDTH_2160p
                        fi
                        # check both pp and hw width limit
                        if [ $is_pp -eq 1 ] && [ $test_case_width -le $width_limit ] 
                        then
                            valid_tester=1
                            break
                        fi
                        let 'current_tester=current_tester+1'
                    done
                    if [ $valid_tester -eq 1 ]
                    then
                        randomValueFromList "$SUPPORTED_TEST_PARAMS_OUTPUT"
                    else
                        # there is no full support for this test case
                        randomValueFromList "$SUPPORTED_TEST_PARAMS_OUTPUT_WITHOUT_PP"
                    fi
                    output_format="$g_function_return"
                fi
                supported=`echo "$supported_test_parameters" |grep "$decode_mode" |grep "$output_format"`
                # decode mode and output format is supported by this format
                if [ ! -z "$supported" ]
                then
                    local current_tester=0
                    local valid_testers=""
                    local invalid_due_to_size=0
                    # find testers that are able to execute this test case
                    while [ $current_tester -lt $board_ips_size ]
                    do
                        local valid_tester=1
                        local features="${features_arr[$current_tester]}"
                        local is_pp=`echo $features |tr "," "\n" |grep -c -w "pp"`
                        if [ $is_pp -gt 1 ]
                        then
                            printErrorAndExit "Internal error @ test.sh:setUpExecutionLists(): multiple PPs found!"
                        fi
                        local is_hevc=`echo $features |tr "," "\n" |grep -c -w "hevc"`
                        if [ $is_hevc -gt 1 ]
                        then
                            printErrorAndExit "Internal error @ test.sh:setUpExecutionLists(): multiple HEVCs found!"
                        fi
                        local is_vp9=`echo $features |tr "," "\n" |grep -c -w "vp9"`
                        if [ $is_vp9 -gt 1 ]
                        then
                            printErrorAndExit "Internal error @ test.sh:setUpExecutionLists(): multiple VP9s found!"
                        fi
                        local is_2160p=`echo $features |tr "," "\n" |grep -c -w "2160p"`
                        local width_limit=""
                        local height_limit=""
                        if [ $is_2160p -eq 0 ]
                        then
                            local is_1080p=`echo $features |tr "," "\n" |grep -c -w "1080p"`
                            if [ $is_1080p -eq 0 ]
                            then
                                printErrorAndExit "Internal error @ test.sh:setUpExecutionLists(): no width configuration found!"
                            fi
                            width_limit=$WIDTH_1080p
                            height_limit=$HEIGHT_1080p
                        else
                            width_limit=$WIDTH_2160p
                            height_limit=$HEIGHT_2160p
                        fi
                        
                        # format check
                        if [[ "$format" == "hevc" && $is_hevc -eq 0 ]] || [[ "$format" == "vp9" && $is_vp9 -eq 0 ]]
                        then
                            valid_tester=0
                        
                        # pp checks
                        elif [[ "$output_format" == "raster" && $is_pp -eq 0 ]]
                        then
                            valid_tester=0
                        
                        # dimension checks
                        elif [[ $test_case_width -gt $width_limit || $test_case_height -gt $height_limit || $test_case_width -lt $MIN_WIDTH || $test_case_height -lt $MIN_HEIGHT ]]
                        then
                            valid_tester=0
                            invalid_due_to_size=1
                        fi
                        
                        if [ $valid_tester -eq 1 ]
                        then
                            valid_testers="${valid_testers}${current_tester} "
                        fi
                        let 'current_tester=current_tester+1'
                    done
                    if [ ! -z "$valid_testers" ]
                    then
                        # add the test case to the executions list where there is fewest test cases
                        local current_tester=0
                        local previous_lines=0
                        for valid_tester in $valid_testers
                        do
                            local board_ip="${board_ips_arr[$valid_tester]}"
                            local execution_list="${EXECUTION_LIST}.${board_ip}"
                            if [ ! -e "$execution_list" ]
                            then
                                 current_tester=$valid_tester
                                 break
                            fi
                            local lines=`cat $execution_list |wc -l`
                            if [ $previous_lines -eq 0 ] || [ $lines -lt $previous_lines ]
                            then
                                current_tester=$valid_tester
                                previous_lines=$lines
                            fi
                        done
                        local board_ip="${board_ips_arr[$current_tester]}"
                        local execution_list="${EXECUTION_LIST}.${board_ip}"
                        local features="${features_arr[$current_tester]}"
                        local hw_bus_width=""
                        local is_64b=`echo $features |tr "," "\n" |grep -c -w "64b"`
                        if [ $is_64b -eq 1 ]
                        then
                            hw_bus_width="64b"
                        else
                            local is_128b=`echo $features |tr "," "\n" |grep -c -w "128b"`
                            if [ $is_128b -eq 1 ]
                            then
                                hw_bus_width="128b"
                            else
                                printErrorAndExit "Internal error @ test.sh:setUpExecutionLists(): no bus width configuration found!"
                            fi
                        fi
                        # set max burst length
                        local max_burst_length=""
                        if [ "$g_burst_values" == "default" ]
                        then
                            max_burst_length="default"
                        elif [ "$g_burst_values" == "random" ]
                        then
                            local burst_values=""
                            local next_burst=$MIN_MAX_BURST_LENGTH
                            while [ $next_burst -le $MAX_MAX_BURST_LENGTH ]
                            do
                                burst_values="${burst_values}${next_burst} "
                                let 'next_burst=next_burst+1'
                            done
                            randomValueFromList "$burst_values"
                            max_burst_length="$g_function_return"
                        else
                            declare -a burst_values_arr=($g_burst_values)
                            max_burst_length=${burst_values_arr[$max_burst_length_index]}
                            let 'max_burst_length_index=max_burst_length_index+1'
                            local burst_values_size=${#burst_values_arr[@]}
                            if [ $max_burst_length_index -eq $burst_values_size ]
                            then
                                max_burst_length_index=0
                            fi
                        fi
                        if [ ! -z "$g_error_params" ] && [ $g_run_without_error -eq 1 ]
                        then
                            echo "${test_case},${decode_mode},${output_format},${hw_bus_width},${g_frame_limit},${max_burst_length},${g_error_params}" >> ${execution_list}
                            echo "${test_case},${decode_mode},${output_format},${hw_bus_width},${g_frame_limit},${max_burst_length}," >> ${execution_list}
                        else
                            echo "${test_case},${decode_mode},${output_format},${hw_bus_width},${g_frame_limit},${max_burst_length},${g_error_params}" >> ${execution_list}
                        fi
                    else
                        # there is no tester for this case
                        if [ $g_exclude_error_size -eq 1 ] && [ $invalid_due_to_size -eq 1 ]
                        then
                            # error_size cases are excluded and this case invalid only due to size -> exclude here
                            echo "" >> /dev/null
                            #echo "No tester for test case ${test_case} (${category}, ${test_case_width}x${test_case_height}) with output format ${output_format}!"
                        else
                            printErrorAndExit "No tester for test case ${test_case} (${category}, ${test_case_width}x${test_case_height}) with output format ${output_format}!"
                        fi
                    fi
                fi
            done
        done
    done
    for wdir in $wdirs
    do
        rm -f ${LOCAL_HOST_EXPORT_PREFIX}/${wdir}/g2/${SCRIPTS_DIR}/${EXECUTION_LIST}.*
        cp $EXECUTION_LIST.* ${LOCAL_HOST_EXPORT_PREFIX}/${wdir}/g2/${SCRIPTS_DIR}
        if [ $? -ne 0 ]
        then
            printErrorAndExit "Cannot copy $EXECUTION_LIST.*!"
        fi
        chmod 777 ${LOCAL_HOST_EXPORT_PREFIX}/${wdir}/g2/${SCRIPTS_DIR}/${EXECUTION_LIST}.*
        cp  $TEST_CASE_FRAMES_FILE ${LOCAL_HOST_EXPORT_PREFIX}/${wdir}/g2/${SCRIPTS_DIR}
        if [ $? -ne 0 ]
        then
            printErrorAndExit "Cannot copy $TEST_CASE_FRAMES_FILE!"
        fi
        chmod 777 ${LOCAL_HOST_EXPORT_PREFIX}/${wdir}/g2/${SCRIPTS_DIR}/${TEST_CASE_FRAMES_FILE}
    done
}

verify()
{
    local wdirs=$2
    
    declare -a board_ips_arr=($1)
    declare -a wdirs_arr=($2)
    declare -a features_arr=($3)
    local board_ips_size=${#board_ips_arr[@]}
    local wdirs_size=${#wdirs_arr[@]}
    local features_size=${#features_arr[@]}
    if [[ $board_ips_size -ne $wdirs_size || $board_ips_size -ne $features_size ]]
    then
        printErrorAndExit "Internal error @ verify(): $board_ips_size -ne $wdirs_size || $board_ips_size -ne $features_size"
    fi
    
    rm -f ${TEST_RESULTS_FILE}*
    
    # distribute the tests
    let 'tester_index=0'
    for wdir in $wdirs
    do
        rm -f ${LOCAL_HOST_EXPORT_PREFIX}/${wdir}/g2/${SCRIPTS_DIR}/${TEST_RESULTS_FILE}*
        local board_ip="${board_ips_arr[$tester_index]}"
        echo "Starting tests @ ${board_ip}"
        ssh ${USER}@$board_ip "cd ${wdir}/g2/${SCRIPTS_DIR} && sudo ./verify.sh" &
        let 'tester_index=tester_index+1'
    done
    wait
    
    # collect the results
    echo "Test case;Category;Test result;Description;Width;Height;Frames;Execution time;Date;Command line;Board IP address;HW Config;TB Config" > $TEST_RESULTS_FILE
    let 'tester_index=0'
    for wdir in $wdirs
    do
         local board_ip="${board_ips_arr[$tester_index]}"
         echo "Collecting test results @ ${board_ip}"
         if [ ! -e ${LOCAL_HOST_EXPORT_PREFIX}/${wdir}/g2/${SCRIPTS_DIR}/${TEST_RESULTS_FILE}.${board_ip} ]
         then
             features="${features_arr[$tester_index]}"
             echo "ALL;N/A;ERROR;Test results not found;N/A;N/A;N/A;N/A;N/A;N/A;${board_ip};${features};N/A" >> $TEST_RESULTS_FILE
         else
             cat ${LOCAL_HOST_EXPORT_PREFIX}/${wdir}/g2/${SCRIPTS_DIR}/${TEST_RESULTS_FILE}.${board_ip} >> $TEST_RESULTS_FILE
         fi
         let 'tester_index=tester_index+1'
    done
    echo "Final test report created to ${TEST_RESULTS_FILE}"
}

if [ ! -e $SETUP_INFO ]
then
    printErrorAndExit "No set-up information found!"
fi

mountExportDir ${LOCAL_HOST_EXPORT_PREFIX}/${TEST_DATA_EXPORT_DIR}

if [ ! -e ${LOCAL_HOST_EXPORT_PREFIX}/${TEST_DATA_EXPORT_DIR}/${TEST_CASE_LIST} ]
then
    printErrorAndExit "Cannot find test case list!"
fi
. ${LOCAL_HOST_EXPORT_PREFIX}/${TEST_DATA_EXPORT_DIR}/${TEST_CASE_LIST}

board_ips=`cat $SETUP_INFO |awk -F\: '{print $1}'`
wdirs=`cat $SETUP_INFO |awk -F\: '{print $2}'`
features=`cat $SETUP_INFO |awk -F\: '{print $3}'`
test_cases=""
test_params=""
error_params=""
while [ $# -gt 0 ]
do   
    if [ "$1" == "-c" ]
    then
        test_cases="$2"
        shift
        shift
    
    elif [ "$1" == "-p" ]
    then
        test_params="$2"
        shift
        shift
    
    elif [ "$1" == "-e" ]
    then
        error_params="$2"
        shift
        shift
        
    elif [ "$1" == "-h" ]
    then
        printHelp
        exit 0
        
    else
        printErrorAndExit "Unknown option: ${1}"
    fi
done
if [ -z "$test_cases" ]
then
    printHelp
    echo ""
    printErrorAndExit "At least -c \"<test_cases>\" must be given!"
fi
#if [ -z "$test_params" ]
#then
#    printErrorAndExit "Test parameters must be given!"
#fi

getFrames
if [ ! -e $TEST_CASE_FRAMES_FILE ]
then
    printErrorAndExit "Cannot get number of frames!"
fi

if [ ! -e ${LOCAL_HOST_EXPORT_PREFIX}/${TEST_DATA_EXPORT_DIR}/${TEST_CASE_TIMES_FILE} ]
then
    printErrorAndExit "Cannot find ${LOCAL_HOST_EXPORT_PREFIX}/${TEST_DATA_EXPORT_DIR}/${TEST_CASE_TIMES_FILE}!"
fi

parseTestParams "$test_params"
parseTestCases "$test_cases"
parseErrorParams "$error_params"
setUpExecutionLists "$board_ips" "$wdirs" "$features"
verify "$board_ips" "$wdirs" "$features"
exit 0
