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
#--   Abstract     : Merges together the integration summary reports
#--                                                                          
#-------------------------------------------------------------------------------

#. ./config.sh
. ./commonconfig.sh
. ./f_utils.sh "summary"

REPORTIME=`date +%d%m%y_%k%M | sed -e 's/ //'`
CSV_FILE="${CSV_FILE_PATH}integrationreport_${HWBASETAG}_${SWTAG}_${USER}_${REPORTIME}_merged.csv"

printUsage()
{
    echo "./reportmerge.sh [options]"
    echo "Options:"
    echo "\"csv\"             Generate merged CSV report, optional"
}

generate_csv_report=0

# get command line arguments, show help if wrong number, otherwise parse options 
if [ $# -gt 1 ]
then
    printUsage
    exit 1
    
elif [ $# -eq 1 ]
then
    if [ $1 == "csv" ]
    then
        generate_csv_report=1
    else
        printUsage
        exit 1   
    fi
fi

echo "Test Case;Category;Pictures;Decoder Picture Width;PP Picture Width;Status;HW Tag;SW Tag;FAILED summary;" > "merged_report.csv"

# get all summary reports
report_pattern="${CSV_FILE_PATH}*${HWBASETAG}*${SWTAG}*_summary.csv"
all_summary_reports=`ls $report_pattern`
echo "Found reports for merge:"
for summary_report in $all_summary_reports
do
    echo "    $summary_report"
done

# get all cases
case_filter=""
getCases "$category_list_swhw" "$CHECK_FORMATS" "$case_filter" "0" >> /dev/null 2>&1
all_cases=$test_cases

# loop through all cases and configurations
for test_case in $all_cases
do
    test_status="NOT_EXECUTED"
    previous_test_status="$test_status"
    failed_reports=""
    printCase "$test_case" "DUMMY" "DUMMY"
    for summary_report in $all_summary_reports
    do
        test_status=`cat $summary_report | grep -w "case ${test_case}" | awk -F \; '{print $6}'`
        if [ ! -z "$test_status" ]
        then
            # NOT_EXECUTED->NOT_FAILED->OK->NOT_OK->FAILED
            if [ "$test_status" == "NOT_EXECUTED" ] && [[ "$previous_test_status" != "NOT_FAILED" && "$previous_test_status" != "OK" && "$previous_test_status" != "NOT_OK" && "$previous_test_status" != "FAILED" ]]
            then
                previous_test_status=$test_status
            fi
            if [ "$test_status" == "NOT_FAILED" ] && [[ "$previous_test_status" != "OK" && "$previous_test_status" != "NOT_OK" && "$previous_test_status" != "FAILED" ]]
            then
                previous_test_status=$test_status
            fi
            if [ "$test_status" == "OK" ] && [[ "$previous_test_status" != "NOT_OK" && "$previous_test_status" != "FAILED" ]]
            then
                previous_test_status=$test_status
            fi
            if [ "$test_status" == "NOT_OK" ] && [[ "$previous_test_status" != "FAILED" ]]
            then
                previous_test_status=$test_status
                failed_reports="${failed_reports} ${summary_report}"
            fi
            if [ "$test_status" == "FAILED" ]
            then
                previous_test_status=$test_status
                failed_reports="${failed_reports} ${summary_report}"
            fi
        fi
    done
    getCategory "$test_case"
    if [ $? != 0 ]
    then
        category="UNKNOWN"
    fi
    getPpCaseWidth "$test_case"
    if [ $pp_case_width == 0 ]
    then
        pp_case_width="N/A"
    fi
    getDecCaseWidthWithAwk "$test_case"
    if [ $dec_case_width == 0 ]
    then
        dec_case_width="N/A"
    fi
    
    echo "$previous_test_status"
    echo "case $test_case;$category;$TB_MAX_NUM_PICS;$dec_case_width;$pp_case_width;$previous_test_status;$HWCONFIGTAG;$SWTAG;$failed_reports;" >> "merged_report.csv"
done

# if csv reporting is enabled, copy the report to destination file
if [ "$generate_csv_report" == 1 ]
then
    cp merged_report.csv "${CSV_FILE}"
fi
exit 0
