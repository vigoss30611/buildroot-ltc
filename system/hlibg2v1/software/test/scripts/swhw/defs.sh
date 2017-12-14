#!/bin/bash
#Copyright 2013 Google Inc. All Rights Reserved.
#Author: mkarki@google.com (Matti Kärki)
#
#Description: Common definitions and functions between FPGA testing scripts.

LOCAL_HOST_EXPORT_PREFIX="/tmp"
LOCAL_HOST_EXPORT_DIR="${LOCAL_HOST_EXPORT_PREFIX}/export"
TEST_DATA_AUTO_DIR="/auto/hantro-testing/sequence/stream/decodertest"
TEST_DATA_EXPORT_DIR="/export/work/testdata/g2_testdata"
TEST_CASE_TIMES_FILE="test_case_times.txt"
TEST_CASE_LIST="testcase_list_g2"

HEVC_FILE_EXTENSIONS="hevc"
VP9_FILE_EXTENSIONS="vp9 webm ivf"

SCRIPTS_DIR="software/test/scripts/swhw"
SETUP_INFO="setup.info"
EXECUTION_LIST="execution_list"
TEST_RESULTS_FILE="test_report.csv"
TEST_CASE_FRAMES_FILE="test_case_frames.txt"
DECODER_BIN="g2dec"
MODEL_BIN="g2model"
DB_PASSWD="laboratorio"
DB_USER="db_user"
DB_HOST="172.28.107.116"
DB_NAME="testDB"
MYSQL_OPTIONS="--skip-column-names"
DB_CMD="mysql ${MYSQL_OPTIONS} -h${DB_HOST} -u${DB_USER} -p${DB_PASSWD} -e"
DB_QUERY_ERROR="0"

g_function_return=""

printErrorAndExit()
{
    local error_msg=$1

    #echo -e '\E[31m'
    echo "$error_msg" 1>&2
    #tput sgr0
    exit 1
}

isNumber()
{
    case $1 in
        ''|*[!0-9]*) return 0 ;;
        *) return 1 ;;
    esac
}

dbQuery()
{
    local query=$1
    DB_QUERY_ERROR=0
    local db_result=`$DB_CMD "use $DB_NAME; ${query}"`
    DB_QUERY_ERROR="$?"
    g_function_return="$db_result"
}

getFrames()
{
    dbQuery "SELECT ptc.testcase_id,tc.frames FROM product_testcase ptc INNER JOIN testcase tc ON ptc.testcase_id = tc.id INNER JOIN product p ON ptc.product_id = p.id WHERE p.name = \"g2\" ORDER BY tc.frames ASC;"
    rm -f $TEST_CASE_FRAMES_FILE 
    if [ $DB_QUERY_ERROR -eq 0 ]
    then
        echo "$g_function_return" > $TEST_CASE_FRAMES_FILE 
    fi
}

mountExportDir()
{
    local export_dir=$1

    if [ ! -d $export_dir ]
    then
        if [ ! -d $LOCAL_HOST_EXPORT_DIR ]
        then
            mkdir $LOCAL_HOST_EXPORT_DIR
            if [ $? -ne 0 ]
            then
                printErrorAndExit "Cannot create ${LOCAL_HOST_EXPORT_DIR}!"
            fi
        fi
        echo -n "Cannot find ${export_dir} -> mounting export... "
        sudo timeout 5m mount 172.28.107.69:/export/ $LOCAL_HOST_EXPORT_DIR
        retval=$?
        if [ $retval -ne 0 ]
        then
            echo "FAILED"
            printErrorAndExit "mount exit status was ${retval}"
        fi
        if [ ! -d $export_dir ]
        then
            echo "FAILED"
            printErrorAndExit "Cannot find ${export_dir}"
        fi
        echo "OK"
    fi
}

getCategory()
{
    local test_case=$1
    
    g_function_return=""
    for category in $category_list_swhw
    do
        eval category_cases=\$$category 
        for category_case in $category_cases
        do
            if [ "$category_case" == "$test_case" ]
            then
                g_function_return="$category"
                return
            fi
        done  
    done
}
