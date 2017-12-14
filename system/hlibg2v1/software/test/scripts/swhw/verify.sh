#!/bin/bash
#Copyright 2013 Google Inc. All Rights Reserved.
#Author: mkarki@google.com (Matti Kärki)
#
#Description: Verifies HW decode on FPGA.

. ./defs.sh

# we need to run tests on local since export slows when multiple instances
USE_LOCAL_DIR=1

# The testing environment does not support  over 100M files
STREAM_SIZE_LIMIT=100

# add 30% extra to time
TIMEOUT_EXTRA="1.3"
# time out is 10s at least
TIMEOUT_MIN="10"

MODEL_DIR="model_run"

. ${TEST_DATA_EXPORT_DIR}/${TEST_CASE_LIST}

g_this_ip=""
g_test_results_file=""
g_frames=""

addResult()
{
    local test_case=$1
    local category=$2
    local result=$3
    local description=$4
    local time=$5
    local command_line=$6
    
    # TODO: parse frame dimensions and count in file mode
    local width=""
    local height=""
    if [ ! -e $test_case ]
    then
        width=`echo ${case_array[$test_case]} |awk -F\; '{print $2}' |awk -F\x '{print $1}'`
        height=`echo ${case_array[$test_case]} |awk -F\; '{print $2}' |awk -F\x '{print $2}'`
    fi
    local frames=$g_frames
    if [ $frames -eq 0 ]
    then
        frames=`grep -w $test_case $TEST_CASE_FRAMES_FILE |awk '{print $2}'`
    fi
    local now=`date +"%T %d.%m.%Y"`
    local hw_config=`grep "$g_this_ip" $SETUP_INFO |awk -F\: '{print $3}'`
    local tb_cfg="N/A"
    if [ -e tb.cfg ]
    then
        tb_cfg=`cat tb.cfg |tr "\n" " " |tr -d " "`
        #tb_cfg=`cat tb.cfg |tr "\n" " "`
    fi
    
    echo "Test case @ $g_this_ip: ${test_case}, ${category}, ${result} (${description}), ${command_line}"
    echo "${test_case};${category};${result};${description};${width};${height};${frames};${time};${now};${command_line};${g_this_ip};${hw_config};\"${tb_cfg}\"" >> $g_test_results_file
}

writeErrorParams()
{
    local error_params=$1
    local seed=""
    local packet_loss=""
    local bit_swap=""
    local stream_truncate=""
    
    if [ -z "$error_params" ]
    then
        return 0
    fi
    
    for error_param in $error_params
    do
        local key=`echo $error_param |awk -F\= '{print $1}'`
        if [ "$key" != "sr" ] && [ "$key" != "pl" ] && [ "$key" != "bs" ] && [ "$key" != "st" ] && [ "$key" != "re" ]
        then
            return 1
        fi
        local values=`echo $error_param |awk -F\= '{print $2}' |tr "," " "`
        declare -a values_arr=($values)
        local values_size=${#values_arr[@]}
        if [ $values_size -ne 1 ]
        then
            return 1
        fi
        if [ "$key" == "sr" ]
        then
            isNumber $values
            if [ $? -eq 0 ] && [ "$values" != "random" ]
            then
                return 1
            fi
            if [ "$values" == "random" ]
            then
                 values=$RANDOM
            fi
            seed="SeedRnd = ${values};"
            
        elif [ "$key" == "pl" ]
        then
            local odds_1=""
            local odds_2=""
            if [ "$values" == "random" ]
            then
                odds_1=1
                let 'odds_2=RANDOM%4'
                if [ $odds_2 -eq 0 ]
                then
                    odds_2=10
                elif [ $odds_2 -eq 1 ]
                then
                    odds_2=50
                elif [ $odds_2 -eq 2 ]
                then
                    odds_2=100
                elif [ $odds_2 -eq 3 ]
                then
                    odds_2=500
                fi
            else
                local odds_values=`echo "$values" |tr ":" " "`
                declare -a odds_values_arr=($odds_values)
                local odds_values_size=${#odds_values_arr[@]}
                if [ $odds_values_size -ne 2 ]
                then
                    return 1
                fi
                for odds_value in $odds_values
                do
                    isNumber $odds_value
                    if [ $? -eq 0 ]
                    then
                        return 1
                    fi
                    if [ -z "$odds_1" ]
                    then
                        odds_1=${odds_value}
                    else
                        odds_2=${odds_value}
                    fi
                done
            fi
            packet_loss="StreamPacketLoss = ${odds_1} : ${odds_2};"
        
        elif [ "$key" == "bs" ]
        then
            local odds_1=""
            local odds_2=""
            if [ "$values" == "random" ]
            then
                odds_1=1
                let 'odds_2=RANDOM%4'
                if [ $odds_2 -eq 0 ]
                then
                    odds_2=10
                elif [ $odds_2 -eq 1 ]
                then
                    odds_2=100
                elif [ $odds_2 -eq 2 ]
                then
                    odds_2=1000
                elif [ $odds_2 -eq 3 ]
                then
                    odds_2=10000
                fi
            else
                local odds_values=`echo "$values" |tr ":" " "`
                declare -a odds_values_arr=($odds_values)
                local odds_values_size=${#odds_values_arr[@]}
                if [ $odds_values_size -ne 2 ]
                then
                    return 1
                fi
                for odds_value in $odds_values
                do
                    isNumber $odds_value
                    if [ $? -eq 0 ] && [ "$values" != "random" ]
                    then
                        return 1
                    fi
                    if [ -z "$odds_1" ]
                    then
                        odds_1=${odds_value}
                    else
                        odds_2=${odds_value}
                    fi
                done
            fi
            bit_swap="StreamBitSwap = ${odds_1} : ${odds_2};"
            
        elif [ "$key" == "st" ]
        then
            isNumber $values
            if [ $? -eq 0 ] && [ "$values" != "random" ]
            then
                return 1
            fi
            if [ "$values" != "random" ] && [ $values -ne 0 ] && [ $values -ne 1 ]
            then
                return 1
            fi
            if [ "$values" == "random" ]
            then
                 let 'values=RANDOM%2'
            fi
            if [ $values -eq 0 ]
            then
                stream_truncate="StreamTruncate = DISABLED;"
            else
                stream_truncate="StreamTruncate = ENABLED;"
            fi
        elif [ "$key" == "re" ]
        then
            # dummy
            echo "" >> /dev/null
        else
            return 1
        fi
    done
    echo "TbParams {"  >> tb.cfg
    echo "    ${seed}"  >> tb.cfg
    echo "    ${packet_loss}"  >> tb.cfg
    echo "    ${bit_swap}"  >> tb.cfg
    echo "    ${stream_truncate}"  >> tb.cfg
    echo "}"  >> tb.cfg
    return 0
}

# get this local machine IP address and use it to decide execution list and results file
g_this_ip=`ifconfig |grep -A 1 eth0 |grep "inet addr" |awk -F\: '{print $2}' |awk '{print $1}'`
g_test_results_file="${TEST_RESULTS_FILE}.${g_this_ip}"
rm -f $g_test_results_file
execution_list="${EXECUTION_LIST}.${g_this_ip}"
if [ ! -e "$execution_list" ]
then
    printErrorAndExit "Cannot find ${execution_list}"
fi

# create workind directory for this machine
wdir="exec_${g_this_ip}"
rm -rf $wdir 
mkdir $wdir
if [ ! -d "$wdir" ]
then
    printErrorAndExit "Cannot create ${wdir}"
fi
cd $wdir
ln -s ../$DECODER_BIN .
ln -s ../$MODEL_BIN .
ln -s ../$execution_list .
ln -s ../$TEST_CASE_FRAMES_FILE .
ln -s ../$SETUP_INFO .

export_dir=$PWD
tmp_dir=""
if [ $USE_LOCAL_DIR -eq 1 ]
then
    # copy the stuff to local directory and run testes from there
    setup_dir=`grep "$g_this_ip" $SETUP_INFO |awk -F\: '{print $2}'`
    tmp_dir=$(mktemp -d -p ~)
    if [ $? -ne 0 ]
    then
        printErrorAndExit "Cannot create temp directory"
    fi
    cp -r $setup_dir $tmp_dir
    if [ $? -ne 0 ]
    then
        printErrorAndExit "Cannot copy ${setup_dir}"
    fi
    chmod -R 777 $tmp_dir
    setup_dir_wo_path=`echo $setup_dir |awk -F \/ '{print $NF}'`
    sw_dir=`echo $export_dir |awk -F${setup_dir_wo_path}/ '{print $2}'`
    cd ${tmp_dir}/${setup_dir_wo_path}/${sw_dir}
    echo "Running tests @ ${g_this_ip}:${tmp_dir}/${setup_dir_wo_path}"
fi

# loop through every test case set for this machine
ulimit -c 2097151
while read line
do
    rm -f core
    rm -f stream.*
    rm -f *.log
    rm -f tb.cfg
    decoder_parameters="-C -P"
    
    test_case=`echo "$line" |awk -F\, '{print $1}'`
    category=""
    file_mode=""
    # file mode
    if [ -e $test_case ]
    then
        file_ext=`echo $test_case |awk -F\/ '{print $NF}' |awk -F\. '{print $NF}'`
        is_hevc=`echo $HEVC_FILE_EXTENSIONS |tr " " "\n" |grep -w -c $file_ext`
        if [ $is_hevc -ne 0 ]
        then
            category="hevc_external"
        else
            is_vp9=`echo $VP9_FILE_EXTENSIONS |tr " " "\n" |grep -w -c $file_ext`
            if [ $is_vp9 -ne 0 ]
            then
                category="vp9_external"
            fi
        fi
        file_mode=1
    else
        getCategory "$test_case"
        category="$g_function_return"
        file_mode=0
    fi
    
    format=`echo "$category" |awk -F_ '{print $1}'`
    g_frames=`echo "$line" |awk -F\, '{print $5}'`
    
    input_strm=""
    output_md5=""
    reference_md5_frames=""
    if [ $file_mode -eq 0 ]
    then
        input_strm="${TEST_DATA_EXPORT_DIR}/case_${test_case}/stream.${format}"
        if [ ! -e $input_strm ]
        then
             if [ "$format" == "vp9" ]
             then
                 input_strm="${TEST_DATA_EXPORT_DIR}/case_${test_case}/stream.webm"
                 if [ ! -e $input_strm ]
                 then
                     addResult "$test_case" "$category" "ERROR" "Cannot find stream file" "N/A" "N/A"
                     continue
                 fi
             else
                 addResult "$test_case" "$category" "ERROR" "Cannot find stream file" "N/A" "N/A"
                 continue
             fi
        fi
        reference_md5="${TEST_DATA_EXPORT_DIR}/case_${test_case}/stream.${format}.md5"
        if [ ! -e $reference_md5 ]
        then
             if [ "$format" == "vp9" ]
             then
                  reference_md5="${TEST_DATA_EXPORT_DIR}/case_${test_case}/stream.webm.md5"
                  if [ ! -e $reference_md5 ]
                  then
                     addResult "$test_case" "$category" "ERROR" "Cannot find reference md5sum" "N/A" "N/A"
                     continue
                  fi
             else
                 addResult "$test_case" "$category" "ERROR" "Cannot find reference md5sum" "N/A" "N/A"
                 continue
             fi
        fi
        reference_md5_frames="${reference_md5}.frames"
        if [ ! -e $reference_md5_frames ]
        then
            addResult "$test_case" "$category" "ERROR" "Cannot find reference md5sum (frames)" "N/A" "N/A"
            continue
        fi
        if [ -e ${TEST_DATA_EXPORT_DIR}/case_${test_case}/stream.webm.md5 ]
        then
            output_md5="stream.webm.yuv"
        else
            output_md5="stream.${format}.yuv"
        fi
    else
        input_strm="$test_case"
        file_name=`echo $input_strm |awk -F\/ '{print $NF}'`
        output_md5="${file_name}.md5"
    fi
    decoder_parameters="${decoder_parameters} -O${output_md5} ${input_strm}"
    # model_parameters is not updated anymore (i.e., run in default mode) (except for frame limits)
    model_parameters="${decoder_parameters}"
    
    decode_mode=`echo "$line" |awk -F\, '{print $2}'`
    if [ "$decode_mode" == "frame" ]
    then
        # dummy
        echo "" >> /dev/null
    elif [ "$decode_mode" == "full" ]
    then
        decoder_parameters="-F ${decoder_parameters}"
        stream_size=`du -m $input_strm |awk '{print $1}'`
        isNumber $stream_size
        if [ $? -ne 0 ]
        then
            if [ $stream_size -gt $STREAM_SIZE_LIMIT ]
            then
                addResult "$test_case" "$category" "ERROR" "Stream size greater than ${STREAM_SIZE_LIMIT}" "N/A" "N/A"
                continue
            fi
        else
            echo "Cannot get file size from ${input_strm} -> assume supported"
        fi
    elif [ "$decode_mode" == "packet" ]
    then
        decoder_parameters="-p ${decoder_parameters}"
    elif [ "$decode_mode" == "nalunit" ]
    then
        # NAL-mode is enabled through tb.cfg
        echo "TbParams {"  >> tb.cfg
        echo "    PacketByPacket = ENABLED;"  >> tb.cfg
        echo "    NalUnitStream = ENABLED;"  >> tb.cfg
        echo "}"  >> tb.cfg
        decoder_parameters="${decoder_parameters}"
    else
        addResult "$test_case" "$category" "ERROR" "Unknown decode mode: ${decode_mode}" "N/A" "N/A"
        continue
    fi
    
    output_format=`echo "$line" |awk -F\, '{print $3}'`
    if [ "$output_format" == "tiled" ]
    then
        decoder_parameters="${decoder_parameters}"
    elif [ "$output_format" == "raster" ]
    then
        decoder_parameters="-Ers ${decoder_parameters}"
    else
        addResult "$test_case" "$category" "ERROR" "Unknown output format: ${output_format}" "N/A" "N/A"
        continue
    fi
    
    # set tb.cfg values for bus width
    hw_bus_width=`echo "$line" |awk -F\, '{print $4}'`
    strmSwap=""
    picSwap=""
    dirmvSwap=""
    tab0Swap=""
    tab1Swap=""
    tab2Swap=""
    tab3Swap=""
    rscanSwap=""
    BusWidth=""
    if [ "$hw_bus_width" == "64b" ]
    then
        strmSwap=7
        picSwap=8
        dirmvSwap=8
        tab0Swap=8
        tab1Swap=8
        tab2Swap=8
        tab3Swap=8
	rscanSwap=8
        BusWidth=1
    elif [ "$hw_bus_width" == "128b" ]
    then
        printErrorAndExit "Swaps not defined for 128b bus!"
        strmSwap=15
        picSwap=0
        BusWidth=2
    fi
    
    # we need to run model when frames are limited to get the reference md5, or
    # when in file mode
    if [ $g_frames -ne 0 ] || [ $file_mode -eq 1 ]
    then
        if [ $g_frames -ne 0 ]
        then
            decoder_parameters="-N${g_frames} ${decoder_parameters}"
            model_parameters="-N${g_frames} ${model_parameters}"
        fi
        if [ ! -d $MODEL_DIR ]
        then
            mkdir $MODEL_DIR
            if [ $? -ne 0 ]
            then
                addResult "$test_case" "$category" "ERROR" "Cannot setup model execution" "N/A" "N/A"
                continue
            fi
            cd $MODEL_DIR
            ln -s ../$MODEL_BIN .
            cd ..
        fi
        cd $MODEL_DIR
        ./${MODEL_BIN} -m ${model_parameters} > ${test_case}.model.log 2>&1 &
        #echo "Model started: ./${MODEL_BIN} -m ${model_parameters}"
        cd ..
    fi
    
    # set tb.cfg values for max burst length
    maxBurst=`echo "$line" |awk -F\, '{print $6}'`
    if [ "$maxBurst" == "default" ]
    then
        maxBurst=""
    fi
    echo "DecParams {"  >> tb.cfg
    echo "   strmSwap = $strmSwap;"  >> tb.cfg
    echo "   picSwap = $picSwap;"  >> tb.cfg
    echo "   dirmvSwap = $dirmvSwap;"  >> tb.cfg
    echo "   tab0Swap = $tab0Swap;"  >> tb.cfg
    echo "   tab1Swap = $tab1Swap;"  >> tb.cfg
    echo "   tab2Swap = $tab2Swap;"  >> tb.cfg
    echo "   tab3Swap = $tab3Swap;"  >> tb.cfg
    echo "   rscanSwap = $rscanSwap;"  >> tb.cfg
    echo "   BusWidth = $BusWidth;"  >> tb.cfg
    if [ ! -z "$maxBurst" ]
    then
        echo "   maxBurst = $maxBurst;"  >> tb.cfg
    fi
    echo "}"  >> tb.cfg
    time_out=0
    if [ $file_mode -eq 0 ]
    then
        time_out=`grep -w $test_case ${TEST_DATA_EXPORT_DIR}/${TEST_CASE_TIMES_FILE} |awk '{print $2}'`
        if [ $? -ne 0 ] || [ -z "$time_out" ]
        then
            frames=`grep -w $test_case $TEST_CASE_FRAMES_FILE |awk '{print $2}'`
            let 'time_out=frames'
            if [ "$decode_mode" == "frame" ]
            then
                let 'time_out=time_out*6'
            fi
        fi
        time_out=`echo "$time_out*$TIMEOUT_EXTRA" |bc |awk -F\. '{print $1}'`
        if [ $time_out -lt $TIMEOUT_MIN ]
        then
            time_out=$TIMEOUT_MIN
        fi
    fi
    
    # set tb.cfg values for error parameters
    error_params=`echo "$line" |awk -F\, '{print $7}'`
    writeErrorParams "$error_params"
    if [ $? -ne 0 ]
    then
        # this should never happen
        addResult "$test_case" "$category" "ERROR" "Cannot set error parameters: ${error_params}" "N/A" "N/A"
    fi
    
    start_time=""
    end_time=""
    if [ $time_out -eq 0 ] || [ ! -z "$error_params" ]
    then
        command_line=`echo "./${DECODER_BIN} -m ${decoder_parameters}"`
        start_time=`date +%s`
        #echo "Decoder starting: ./${DECODER_BIN} -m ${decoder_parameters}"
        ./${DECODER_BIN} -m ${decoder_parameters} >> ${test_case}.log 2>&1
        exit_status=$?
        end_time=`date +%s`
    else
        command_line=`echo "timeout ${time_out} ./${DECODER_BIN} -m ${decoder_parameters}"`
        start_time=`date +%s`
        #echo "Decoder starting: imeout ${time_out} ./${DECODER_BIN} -m ${decoder_parameters}"
        timeout ${time_out} ./${DECODER_BIN} -m ${decoder_parameters} >> ${test_case}.log 2>&1
        exit_status=$?
        end_time=`date +%s`
        if [ $exit_status -eq 124 ]
        then
            addResult "$test_case" "$category" "FAILED" "${DECODER_BIN} timeout" "N/A" "$command_line"
            continue
        fi
    fi
        
    let 'execution_time=end_time-start_time'
    if [ $exit_status -ne 0 ]
    then
        if [ -e core ]
        then
            addResult "$test_case" "$category" "FAILED" "${DECODER_BIN} crashed" "N/A" "$command_line"
        else
            addResult "$test_case" "$category" "FAILED" "${DECODER_BIN} exit status is ${exit_status}" "N/A" "$command_line"
        fi
        continue
    fi
    if [ -e core ]
    then
        addResult "$test_case" "$category" "FAILED" "${DECODER_BIN} crashed" "N/A" "$command_line"
        continue
    fi

    # don't do any more checks with error injection
    if [ ! -z "$error_params" ]
    then
        addResult "$test_case" "$category" "OK" "${DECODER_BIN} behaved" "$execution_time" "$command_line"
        continue
    fi
    
    if [ ! -e $output_md5 ]
    then
        addResult "$test_case" "$category" "FAILED" "${DECODER_BIN} output missing" "N/A" "$command_line"
        continue
    fi
    #md5sum --strict -c $reference_md5 >> /dev/null 2>&1
    cmp_retval=""
    if [ $g_frames -eq 0 ] && [ $file_mode -eq 0 ]
    then
        cmp $reference_md5_frames $output_md5 > cmp.log 2>&1
        cmp_retval="$?"
    else
        #echo "Waiting..."
        wait
        cmp $MODEL_DIR/$output_md5 $output_md5 > cmp.log 2>&1
        cmp_retval="$?"
    fi
    if [ $cmp_retval -ne 0 ]
    then
        failing_picture=`cat cmp.log |awk -Fline '{print $2}' |tr -d " " |tr -d "\n"`
        let 'failing_picture=failing_picture-1'
        if [ $failing_picture -eq -1 ]
        then
            fail_reason=`cat cmp.log`
            if [ "$fail_reason" == "cmp: EOF on ${output_md5}" ]
            then
                last_picture=`cat ${output_md5} |wc -l`
                let 'last_picture=last_picture-1'
                fail_reason="Picture(s) missing, last output picture is ${last_picture}"
            fi
            addResult "$test_case" "$category" "FAILED" "${fail_reason}" "$execution_time" "$command_line"
            continue
        fi
        addResult "$test_case" "$category" "FAILED" "Picture ${failing_picture}" "$execution_time" "$command_line"
    else
        addResult "$test_case" "$category" "OK" "md5sum matches" "$execution_time" "$command_line"
    fi
    rm -f $MODEL_DIR/$output_md5
    rm -f ${output_md5}
done < $execution_list

if [ $USE_LOCAL_DIR -eq 1 ]
then
    cp $g_test_results_file ${export_dir}/..
    if [ $? -ne 0 ]
    then
        printErrorAndExit "Cannot cp ${g_test_results_file}"
    fi
    chmod 777 ${export_dir}/../${g_test_results_file}
    cd - >> /dev/null
    #rm -rf $tmp_dir
else
    mv $g_test_results_file ..
fi

exit 0


