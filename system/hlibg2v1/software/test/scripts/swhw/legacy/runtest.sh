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
#--   Abstract     : Script for running the SW/HW verification test cases     --
#--                                                                           --
#-------------------------------------------------------------------------------

#=====--------------------------------------------------------------------=====#

# imports
. ./config.sh
. ./f_run.sh
. ./f_utils.sh "run"
. $TEST_CASE_LIST_RUN

printUsage()
{
    echo "./runtest.sh [options]"
    echo "Options:"
    echo "case_nbr            Test cases/part of category name to be included in test execution, mandatory"
    echo "                        Use case_nbr == --repeat_seq to repeat the randomized test case execution order"
    echo "                        Use case_nbr == --failed to (re)run the failed cases"
    echo "-n filters          Test cases/part of category name to be excluded in test execution, optional"
    echo "-cfg                Use settings from previous execution"
}

use_previous_cfg=0
# get command line arguments, show help if wrong number, otherwise parse options 
if [ $# -lt 1 ]
then
    printUsage
    exit 1

## case by number and only one case -> avoid test case parsing
#elif [ $# -eq 1 ] && (isDigit $1)
#then
#    test_cases="$1"    

else
    test_start_time=$(date +%s)
    # case number or category name
    case_number=$1
    
    # maximum width in pixels, set to a value large enough
    max_width=10000
  
    # test case filter, initialise to null string
    case_filter="" 
    
    # cycle through all command line options
    while [ $# -gt 0 ]
    do   
        # test case filter switch found
        if [ "$1" == "-n" ]
        then
            case_filter="$2"
            
        elif [ "$1" == "-cfg" ]
        then
            use_previous_cfg=1
        fi
    
        # change to next argument
        shift
    done
    
    if [ "$case_number" != "--failed" ]
    then
        rm -f test_cases
        rm -f test_cases_printed
    else
        rm -f test_cases_printed_failed
    fi
    
    # get valid cases
    if [ "$case_number" != "--repeat_seq" ] && [ "$case_number" != "--failed" ]
    then
        getCases "$category_list_swhw" "$case_number" "$case_filter" "1" "0"
    fi
    
    # if enabled, randomize test case execution order
    if [ "$TB_RND_TC" == "ENABLED" ] && [ "$case_number" != "--failed" ]
    then
        if [ "$case_number" == "--repeat_seq" ]
        then
            if [ -e test_cases_repeat  ]
            then
                echo "Randomized test case order enabled (repeating sequence)"
                test_cases=`head -n 1 test_cases_repeat`
            
            else
                echo "Cannot find previous randomized sequence"
                exit 1
            fi
        
        elif [ ! -e test_cases_rnd ]
        then
            echo "Randomized test case order enabled"
            randomizeCases "$test_cases"
            test_cases="$rnd_cases"
        
            rm -f test_cases_repeat
            # print out the randomized cases for later restore
            for case_number in $test_cases
            do
                echo -n "${case_number} " >> test_cases_rnd
                echo -n "${case_number} " >> test_cases_repeat
            done
        
        # read the previously randomized cases
        else
            echo "Randomized test case order enabled (reading case order from file)"
            test_cases=`head -n 1 test_cases_rnd`
        fi
    fi
    
    if [ "$case_number" == "--failed" ]
    then
        echo -n "Waiting check to complete... "
        while [ ! -e check_completed ]
        do
            sleep 1
        done
        test_cases=`head -n 1 test_cases_failed`
        # previous configuration is enabled
        use_previous_cfg=1
        echo "OK"
    fi
fi

# run the cases
runCases "$test_cases" "$test_start_time" "$use_previous_cfg"
rm -f test_exec_status

# randomized cases were finalized
if [ "$TB_RND_TC" == "ENABLED" ]
then
    rm -f test_cases_rnd
fi

exit 0
