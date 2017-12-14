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
#--   Abstract     : Script for the execution of the SW/HW  multiinstance     --
#--                  test cases                                               --
#--                                                                           --
#-------------------------------------------------------------------------------

#=====--------------------------------------------------------------------=====#

. ./testcase_list

#-------------------------------------------------------------------------------
#-                                                                            --
#- Runs a test case                                                           --
#-                                                                            --
#- Parameters                                                                 --
#- 1 : Directory where the test case is run                                   --
#- 2 : Test case number                                                       --
#-                                                                            --
#-------------------------------------------------------------------------------
runCase()
{
    wdir=$1
    case_to_be_run=$2

    mkdir $wdir
    chmod 777 $wdir
    cd $wdir
    ln -s ../*.sh .
    ln -s ../testcase_list .
    ln -s ../tags .
    ./runtest.sh $case_to_be_run &
    chmod 777 * 2> /dev/null
    cd ..
}

## Set-up kernel modules
#wdir=$PWD
#/sbin/rmmod memalloc
#cd ../../../../linux/memalloc && ./memalloc_load.sh
#/sbin/rmmod hx170dec
#cd ../ldriver/kernel_26x/ && ./driver_load.sh
#cd $wdir


# Get the multi-instance cases
declare -a mi_cases
number_mi_cases=0

for category in $category_list_swhw
do
    eval cases=\$$category
    for case_number in $cases
    do
        mi_cases[$counter]=$case_number
        let 'counter+=1'
    done
done

# Get the first case if the previous execution was halted
first_case=0
first_case_2=0
if [ -e test_exec_status ]
then
    first_case=`tail -n 1 test_exec_status`
    case_done=`echo "$first_case" | grep "DONE"`
    i=`expr index "$first_case" ' '`
    let 'i-=1' # remove trailing ' '
    first_case=${first_case:0:$i}
    # check if the last case was executed completely
    if [ ! -z "$case_done" ]
    then
        first_case_2=`tail -n 1 test_exec_status`
        j=`expr index "$first_case_2" 'D'`
        let 'j-=2' # remove trailing ' D'
        let 'i+=1' # first_case_2 start from i+1
        let 'j-=i' # remove trailing first_case
        first_case_2=${first_case_2:$i:$j}
    
    else
        first_case_2=`tail -n 1 test_exec_status`
        j=`expr index "$first_case_2" ' '`
        first_case_2=${first_case_2:$j}
    fi
    
fi

# Run each multi-instance case with each other
let counter=0
let counter2=0
for case_number in ${mi_cases[*]}
do
    for case_number_2 in ${mi_cases[*]}
    do
        if [ $counter2 -ge $counter ]
        then
            if [[ "$case_number" == "$first_case" && "$case_number_2" == "$first_case_2" ]] || [ "$first_case" == 0 ]
            then
                echo "Run: ${case_number} ${case_number_2}"
                echo -n "${case_number} ${case_number_2}" >> test_exec_status
                runCase "${case_number}_${case_number_2}_1" "${case_number}"
                runCase "${case_number}_${case_number_2}_2" "${case_number_2}"
                wait
                echo  " DONE" >> test_exec_status
                first_case=0
                first_case_2=0
                echo
            fi
        fi
        let 'counter2=counter2+1'
    done
    let 'counter=counter+1'
    let counter2=0
done
