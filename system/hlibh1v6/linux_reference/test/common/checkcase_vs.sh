#!/bin/bash    

#Imports
. ./f_check.sh

importFiles

let "case_nro=$1"

# Dir where to save failed cases
FAILDIR=/export/tmp/$PRODUCT/${hwtag}_${reporttime}

# Test data dir
case_dir=$TEST_DATA_HOME/case_$case_nro

testcasedir=${TESTDIR}/vs_case_${case_nro}

# If random_run in progress, the TESTDIR will be in random_cases
if [ -e random_run ]
then
    echo -en "\rRandom Stabilization Case $case_nro   "

else
    echo -en "\rStabilization Case $case_nro   "
fi

if [ ! -e $test_case_list_dir/test_data_parameter_h264.sh ]
then
    echo "$test_case_list_dir/test_data_parameter_h264.sh doesn't exist!"
    exit -1
fi

. $test_case_list_dir/test_data_parameter_h264.sh "$case_nro"

set_nro=$((case_nro-case_nro/5*5))          
picWidth=$((width[set_nro]))
picHeight=$((height[set_nro]))
casetestId=$((testId[set_nro]))

# Create check.log that contains the messages of all checks.
rm -f ${testcasedir}/check.log

# run only stab cases */    
if [ ${stabilization[${set_nro}]} -eq 0 ]; then
    echo -n "NOT stabilization case!" >> ${testcasedir}/check.log
    exit 1
fi

# don't run scene change detection cases */
if [ ${lumWidthSrc[${set_nro}]} -eq ${width[${set_nro}]} ]; then
    echo -n "Scene change detection case!" >> ${testcasedir}/check.log
    exit 1
fi

if ( [ "$INTERNAL_TEST" == "n" ] )
then
    if [ $casetestId != "0" ] 
    then
        echo -n "Internal test, does not work with customer releases!" >> ${testcasedir}/check.log
        exit 1
    fi
fi

# If test data doesn't exist, generate it
if [ ! -e ${case_dir}/video_stab_result.trc ]
then
    (
    cd $SYSTEM_MODEL_HOME
    ./test_data/test_data.sh $case_nro >> /dev/null 2>&1
    )
fi

if [ -e ${case_dir}/video_stab_result.trc ]
then
    # Wait until the case has been run
    while [ ! -e ${testcasedir}/run_status ]
    do
        echo -n "."
        sleep 10
    done

    # Compare output to reference
    if (cmp -s ${testcasedir}/vs_case_$case_nro.trc ${case_dir}/video_stab_result.trc)
    then
        echo -n "OK" >> ${testcasedir}/check.log
        exit 0
    else
        echo -n "FAILED" >> ${testcasedir}/check.log
        saveFileToDir ${testcasedir}/vs_case_${case_nro}.trc $FAILDIR vs_case_${case_nro}.trc
        saveFileToDir ${case_dir}/video_stab_result.trc $FAILDIR vs_case_${case_nro}.trc.ref
        exit -1
    fi
else
    echo -n "Reference <video_stab_result.trc> missing" >> ${testcasedir}/check.log
    exit -1
fi

