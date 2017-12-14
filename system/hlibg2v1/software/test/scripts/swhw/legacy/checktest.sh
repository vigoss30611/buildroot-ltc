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
#--   Abstract     : Script for checking the results of the SW/HW             --
#--                  verification test cases                                  --
#--                                                                           --
#-------------------------------------------------------------------------------

#=====--------------------------------------------------------------------=====#

# imports
. ./config.sh
. ./f_check.sh
. ./f_utils.sh "check"
. $TEST_CASE_LIST_CHECK

printUsage()
{
    echo "./checktest.sh [options]"
    echo "Options:"
    echo "case_nbr            Test cases/part of category name to be included in test execution, mandatory"
    echo "                        Use case_nbr == --failed to (re)check the failed cases"
    echo "-n filters          Test cases/part of category name to be excluded in test execution, optional"
    echo "\"csv\"             Generate CSV report, optional"
}

generate_csv_report=0

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
    # case number or category name
    case_number=$1
    
    # test case filter, initialise to null string
    case_filter=""
     
    # Cycle through all command line options
    while [ $# -gt 0 ]
    do   
        # test case filter switch found
        if [ "$1" == "-n" ]
        then
            case_filter=$2
        fi
        
        # generate csv report switch found
        if [ "$1" == "csv" ]
        then
            generate_csv_report=1
        fi
    
        # change to next argument
        shift
    done
    
    # get valid cases
    use_failed=0
    if [ "$case_number" != "--failed" ]
    then
        getCases "$category_list_swhw" "$case_number" "$case_filter" "0" "0"
    else
        test_cases=`head -n 1 test_cases_failed`
        use_failed=1
    fi
fi

# Make sure that core dump is written
ulimit -c unlimited

# check the cases and generate csv report
checkCases "$test_cases" "$generate_csv_report" "$use_failed"
if [ "$case_number" != "--failed" ]
then
    touch check_completed
fi

exit 0
