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
#--   Abstract     : Script for the checking of the SW/HW  multiinstance      --
#--                  test cases                                               --
#--                                                                           --
#-------------------------------------------------------------------------------

#=====--------------------------------------------------------------------=====#

. ./testcase_list

#-------------------------------------------------------------------------------
#-                                                                            --
#- Checks a test case.                                                        --
#-                                                                            --
#- Parameters                                                                 --
#- 1 : Directory where the test case is checked                               --
#- 2 : Test case number                                                       --
#-                                                                            --
#-------------------------------------------------------------------------------
checkCase()
{
    wdir=$1
    case_set=$2

    cd $wdir
    ./checktest.sh $case_set
    cd ..
}

# Get the multi-instance case
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

# Check each multi-instance case with each other
let counter=0
let counter2=0
for case_number in ${mi_cases[*]}
do
    for case_number_2 in ${mi_cases[*]}
    do
        if [ $counter2 -ge $counter ]
        then
            #if [ "$case_number" != 4007 ] 
            #then
            echo "Check: ${case_number} ${case_number_2}"
            checkCase "${case_number}_${case_number_2}_1" "${case_number}"
            checkCase "${case_number}_${case_number_2}_2" "${case_number_2}"
            wait
            echo
            #fi
        fi
        let 'counter2=counter2+1'
    done
    let 'counter=counter+1'
    let counter2=0
done
