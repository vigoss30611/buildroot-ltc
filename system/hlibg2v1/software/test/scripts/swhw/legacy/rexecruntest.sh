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
#-                                                                            --
#--   Abstract     :    Script for executing the decoding process             --
#--                                                                           --
#--                                                                           --
#-------------------------------------------------------------------------------

#=====--------------------------------------------------------------------=====#

# imports
. ./config.sh 

#-------------------------------------------------------------------------------
#-                                                                            --
#- Monitors the progress of the decoding process. If the decoding halts for   --
#- some reason, terminates the process and informs about the halt.            --
#-                                                                            --
#- Parameters                                                                 --
#- 1 : The process id which which will be terminated in case of halt.         --
#- 2 : The file which progress is monitored                                   --
#- 3 : Test case number                                                       --
#-                                                                            --
#-------------------------------------------------------------------------------
startWatchDog()
{
    pid=$1
    monitor=$2
    case_number=$3
    
    # the decoding progress is monitored by measuring the decoder out file size
    previous_file_size=0
    current_file_size=0
    
    let 'loop=1'
    let 'poll_time=1'
    let 'total_sleep_time=0'
    
    # start monitoring
    while [ "$loop" -eq 1 ]
    do
        # poll until the specified timeout is reached
        while [ "$total_sleep_time" -le "$TB_TIMEOUT" ]
        do
            # sleep only poll time
            sleep "$poll_time"
            
            echo -n "."
            let 'total_sleep_time=total_sleep_time+poll_time'
            
            # check the available disk space
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
                            #process=`ps -e | grep "\<${pid}\>"`
                            rm -f ps_e
                            ps -e > ps_e
                            while [ ! -e ps_e ]
                            do
                                ls -la >> /dev/null 2>&1
                            done
                            if [ ! -z "${process}" ]
                            then
                                `kill -9 $pid`
                            fi
                            touch case_${case_number}.out_of_space
                            return
                        fi
                        break
                    fi
                done
            fi
            
            # if the process has already been terminated 
            # -> exit to minimize the wait time
            #process=`ps -e | grep "\<${pid}\>"`
            # write ps output to file to avoid grep being listed in ps
            rm -f ps_e
            ps -e > ps_e
            while [ ! -e ps_e ]
            do
                ls -la >> /dev/null 2>&1
            done
            process=`grep "\<${pid}\>" ps_e`
            if [ -z "$process" ]
            then
                return
            fi
        done
    
        let total_sleep_time=0
        echo -n "!"
        
        # if file has not been created -> kill and exit
        ls_info=`ls -l $monitor 2> /dev/null`
        if [ -z "$ls_info" ]
        then
            `kill -9 $pid`
            touch case_${case_number}.timeout
            return
        fi
    
         # check the current file size and compare with the previous size
        let element_number=0
        for i in $ls_info
        do
            let 'element_number=element_number+1'
            if [ $element_number == 5 ]
            then
                current_file_size=$i
                
                # if no progress is made
                if [ $current_file_size -eq $previous_file_size ]
                then
                    
                    # if the process has not already been terminated 
                    # -> kill the process
                    #process=`ps -e | grep "\<${pid}\>"`
                    # write ps output to file to avoid grep being listed in ps
                    rm -f ps_e
                    ps -e > ps_e
                    while [ ! -e ps_e ]
                    do
                        ls -la >> /dev/null 2>&1
                    done
                    process=`grep "\<${pid}\>" ps_e`
                    if [ ! -z "${process}" ]
                    then
                        `kill -9 $pid`
                        
                        # if process has terminated
                        if [ $? != 0 ] 
                        then
                            return
                        fi
                    fi
                    
                    # stop polling and indicate timer has been expired
                    let loop=0
                    touch case_${case_number}.timeout
                else
                    previous_file_size=$current_file_size
                fi
                break
            fi
        done
    done
}

limitExecutionTime()
{
    pid=$1
    monitor=$2
    case_number=$3
    execution_time_limit=$4
    
    let 'loop=1'
    let 'poll_time=1'
    let 'total_sleep_time=0'
    
    # start monitoring
    while [ "$loop" -eq 1 ]
    do
        # if the process has already been terminated, exit
        #process=`ps -e | grep "\<${pid}\>"`
        # write ps output to file to avoid grep being listed in ps
        rm -f ps_e
        ps -e > ps_e
        while [ ! -e ps_e ]
        do
            ls -la >> /dev/null 2>&1
        done
        process=`grep "\<${pid}\>" ps_e`
        if [ -z "$process" ]
        then
            return
        fi
        
        sleep "$poll_time"
        
        let 'total_sleep_time=total_sleep_time+poll_time'
        # execution limit time has been reached
        if [ "$total_sleep_time" -ge "$execution_time_limit" ]
        then
            # get the current output file size
            ls_info=`ls -l $monitor 2> /dev/null`
            current_file_size=0
            #previous_file_size=0
            let element_number=0
            for i in $ls_info
            do
                let 'element_number=element_number+1'
                if [ $element_number == 5 ]
                then
                    current_file_size=$i
            #        previous_file_size=$current_file_size
                fi
            done
            ## wait until output has been written for the last time
            #while [ "$current_file_size" -eq "$previous_file_size" ] 
            while [ "$current_file_size" -le 0 ] 
            do
                ls_info=`ls -l $monitor 2> /dev/null`
                let element_number=0
                for i in $ls_info
                do
                    let 'element_number=element_number+1'
                    if [ $element_number == 5 ]
                    then
                        current_file_size=$i
                    fi
                done
                # if the process has been terminated, exit
                #process=`ps -e | grep "\<${pid}\>"`
                # write ps output to file to avoid grep being listed in ps
                rm -f ps_e
                ps -e > ps_e
                while [ ! -e ps_e ]
                do
                    ls -la >> /dev/null 2>&1
                done
                if [ -z "${process}" ]
                then
                    return
                fi
            done
            
            # if the process has not already been terminated, kill the process
            #process=`ps -e | grep "\<${pid}\>"`
            # write ps output to file to avoid grep being listed in ps
            rm -f ps_e
            ps -e > ps_e
            while [ ! -e ps_e ]
            do
                 ls -la >> /dev/null 2>&1
            done
            if [ ! -z "${process}" ]
            then
                `kill -9 $pid`
                touch case_${case_number}.time_limit
            fi 
        fi
    done
}

# store command line parameters
wdir=$1
decoder=$2
decoder_params=$3
decoder_log=$4
decoder_out=$5
case_number=$6
execution_time_limit=$7

# Make sure that core dump is written
ulimit -c unlimited

cd $wdir
echo "${decoder} ${decoder_params}" >> "$decoder_log"

start_time=$(date +%s)
#echo "START:${start_time}" >> "case_${case_number}.time"

if [ "$TB_TEST_EXECUTION_TIME" != "-1" ] && [ "$decoder" != "$JPEG_DECODER" ] && [ "$decoder" != "$PP_JPEG_PIPELINE" ]
then
    $decoder $decoder_params >> "$decoder_log" 2>&1 &
    pid=$!
    limitExecutionTime "$pid" "$decoder_out" "$case_number" "$execution_time_limit"
    
# run the decoder without the watch dog
elif [ "$TB_TIMEOUT" == "-1" ]
then
    $decoder $decoder_params >> "$decoder_log" 2>&1

    
# the decoding process is executed in background and 
# watch dog is enabled to monitor the progress
else
    $decoder $decoder_params >> "$decoder_log" 2>&1 &
    pid=$!
    #startWatchDog "$pid" "$decoder_out" "$case_number"
    startWatchDog "$pid" "$decoder_log" "$case_number"
fi
end_time=$(date +%s)
#echo "END:${end_time}" >> "case_${case_number}.time"
let 'execution_time=end_time-start_time'
echo "$execution_time" >> "case_${case_number}.time"
chmod 666 "case_${case_number}.time"

# rename the pp outputs
if [ -e $PP_OUT_YUV ]
then
    mv -f "$PP_OUT_YUV" "case_${case_number}.yuv" >> /dev/null 2>&1
fi
if [ -e $PP_OUT_RGB ]
then
    mv -f "$PP_OUT_RGB" "case_${case_number}.rgb" >> /dev/null 2>&1
fi
            
# rename the jpeg outputs
if [ -e $JPEG_OUT ]
then
    mv -f "$JPEG_OUT" "case_${case_number}.yuv" >> /dev/null 2>&1
fi
if [ -e $JPEG_OUT_THUMB ]
then
    mv -f "$JPEG_OUT_THUMB" "tn_case_${case_number}.yuv" >> /dev/null 2>&1
fi

if [ "$DEC_2ND_CHROMA_TABLE" == "ENABLED" ]
then
    mv -f "$H264_2ND_CHROMA_TABLE_OUTPUT" "2nd_chroma_case_${case_number}.yuv" >> /dev/null 2>&1
fi

# generate script for repeating the test cases with same params
echo "#!/bin/bash" > repeat_${case_number}.sh
echo "cp -f case_${case_number}.cfg tb.cfg" >> repeat_${case_number}.sh
echo "$decoder $decoder_params" >> repeat_${case_number}.sh
chmod 777 repeat_${case_number}.sh

# rename sw register frame
if [ -e swreg.trc ]
then
    mv swreg.trc case_${case_number}_swreg.trc
fi

# indicate whether the component has crashed
crash=`ls core* 2> /dev/null`
if [ ! -z "$crash" ]
then
    mv -f core* "case_${case_number}.crash"
fi

# rename performance log
if [ -e "$HW_PERFORMANCE_LOG" ]
then
    mv -f "$HW_PERFORMANCE_LOG" "case_${case_number}_${HW_PERFORMANCE_LOG}"
fi

chmod 666 "$decoder_log"

## indicate that the case is finalized
#echo "DONE" > "case_${case_number}.done"
#echo -n " DONE" >> test_exec_status

#touch "case_${case_number}.done"
echo "FLUSH" > "case_${case_number}.done"
chmod 666 "case_${case_number}.done"

echo "OK"
