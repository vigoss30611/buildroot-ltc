#!/bin/bash
#Copyright 2013 Google Inc. All Rights Reserved.
#Author: mkarki@google.com (Matti Kärki)
#
#Description: Updates testdata to "export", where it can be accessed by HW decoder FPGA tests.
# Creates frame based md5sums, as well.

. ./defs.sh

TEST_CASE_START_ID="12000"
GET_TEST_CASE_LIST="../../../../common/scripts/test_data/database/testcase_list.pl g2"
MAKE_PARAMS_MODEL="-j g2dec ENV=x86_linux USE_ASIC_TRACE=n USE_MODEL_SIMULATION=y RELEASE=y WEBM_ENABLED=y NESTEGG=${LOCAL_HOST_EXPORT_DIR}/work/sw_tools/nestegg/i386"

exit_status=0

# create mount to export
mountExportDir ${LOCAL_HOST_EXPORT_PREFIX}/${TEST_DATA_EXPORT_DIR}

# get the test case directories from network drive
all_cases=`ls -d $TEST_DATA_AUTO_DIR/case_* |awk -F\_ '{print $NF}'`

# get the test case list and import
$GET_TEST_CASE_LIST
if [ ! -e $TEST_CASE_LIST ]
then
    printErrorAndExit "Cannot get ${TEST_CASE_LIST}!"
fi
. $TEST_CASE_LIST

# compile the model
make_dir=`echo $PWD | awk -Fg2 '{print $1}'`
cd $make_dir/g2
make clean > make.log 2>&1
make $MAKE_PARAMS_MODEL >> make.log 2>&1
decoder_bin=`grep "$DECODER_BIN" make.log |grep LINK |awk '{print $NF}'`
decoder_bin="${PWD}/${decoder_bin}"
if [ ! -e $decoder_bin ]
then
    printErrorAndExit "Cannot compile model, see make.log!"
fi
cd - >> /dev/null
cp $decoder_bin . >> /dev/null 2>&1
if [ ! -e $DECODER_BIN ]
then
    printErrorAndExit "Cannot copy $DECODER_BIN"
fi

# update test data
rm -f decoder.log
for test_case in $all_cases
do
    # update only g2 test data
    if [ $test_case -ge $TEST_CASE_START_ID ]
    then
        # check the validity of test case list (i.e., the database) and the network drive
        exists=${case_array[$test_case]}
        if [ -z "$exists" ]
        then
            echo "Test case ${test_case} not in the database!" 1>&2
            if [ $exit_status -ne 1 ]
            then
                exit_status=2
            fi
            continue
        else
            case_dir="${LOCAL_HOST_EXPORT_PREFIX}/${TEST_DATA_EXPORT_DIR}/case_${test_case}"
            if [ ! -d $case_dir ]
            then
                mkdir $case_dir
                if [ $? -ne 0 ]
                then
                     echo "Cannot create $case_dir!" 1>&2
                     exit_status=1
                     continue
                fi
            fi
            cp -vu ${TEST_DATA_AUTO_DIR}/case_${test_case}/stream.* $case_dir
            if [ $? -ne 0 ]
            then
                echo "Test case ${test_case} copy failed!" 1>&2
                exit_status=1
                continue
            fi
            # generate frame by frame md5sum
            stream=`ls $case_dir/stream.* |grep -m 1 stream |awk -F\/ '{print $NF}' | awk -F\. '{print $1"."$2}'`
            if [ ! -e ${case_dir}/${stream}.md5.frames ] || [ ${case_dir}/${stream}.md5.frames -ot ${case_dir}/${stream} ]
            then
                if [ -e ${case_dir}/${stream}.md5 ]
                then
                    rm -f ${case_dir}/${stream}.md5.frames
                    echo -n "Verifying model for ${case_dir}/${stream}... "
                    decoder_parameters="-C -P -M -O${stream}.yuv ${case_dir}/${stream}"
                    echo "${case_dir}" >> decoder.log
                    ./${DECODER_BIN} ${decoder_parameters} >> decoder.log 2>&1
                    cmp -s ${case_dir}/${stream}.md5 ${stream}.yuv >> /dev/null 2>&1
                    if [ $? -ne 0 ]
                    then
                        rm -f ${stream}.yuv
                        echo "FAILED"
                        echo "Model verification failed for ${case_dir}/${stream}" 1>&2
                        if [ $exit_status -ne 1 ]
                        then
                            exit_status=2
                        fi
                        continue
                    else
                        rm -f ${stream}.yuv
                        echo "OK"
                        echo -n "Generating frame by frame md5sum for ${case_dir}/${stream}... "
                        decoder_parameters="-C -P -m -O${stream}.md5.frames ${case_dir}/${stream}"
                        ./${DECODER_BIN} ${decoder_parameters} >> decoder.log 2>&1
                        if [ -e ${stream}.md5.frames ]
                        then
                            mv ${stream}.md5.frames ${case_dir}
                            if [ $? -ne 0 ]
                            then
                                echo "FAILED"
                                echo "Test case ${test_case} move failed (frame md5sum)!" 1>&2
                                exit_status=1
                            else
                                echo "OK"
                            fi
                        else
                            echo "FAILED"
                            echo "Cannot find ${PWD}/${stream}.md5.frames!" 1>&2
                            exit_status=1
                            continue
                        fi
                    fi
                else
                    echo "${case_dir} missing sequence md5sum -> not generating frame by frame md5sum" 1>&2
                    if [ $exit_status -ne 1 ]
                    then
                        exit_status=2
                    fi
                fi
            fi
        fi
    fi
done
rm -f $DECODER_BIN

# update the test case list on "export"
mv $TEST_CASE_LIST ${LOCAL_HOST_EXPORT_PREFIX}/${TEST_DATA_EXPORT_DIR}
if [ $? -ne 0 ]
then
    echo "Cannot mv $TEST_CASE_LIST!" 1>&2
    exit_status=1
fi

chmod 777 -R ${LOCAL_HOST_EXPORT_PREFIX}/${TEST_DATA_EXPORT_DIR}
if [ $? -ne 0 ]
then
    echo "Cannot change permissions in ${LOCAL_HOST_EXPORT_PREFIX}/${TEST_DATA_EXPORT_DIR}"
    exit_status=1
fi

exit $exit_status
