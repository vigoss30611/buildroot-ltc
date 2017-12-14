#!/bin/bash

#Imports
.  ./f_check.sh

importFiles

sum_ok=0
sum_failed=0
sum_notrun=0
sum_noref=0

# Dir to store failed cases
if [ ! -e random_parameter_fails ]
then
    mkdir random_parameter_fails
fi

check_case()
{
    echo -en "\rCase $1   "
    
    if [ -e case_$1/stream.$2 ]
    then
        if [ -e case_$1/case_$1.$2 ]
        then
            # Compare output to reference
            if (cmp -s case_$1/stream.$2 case_$1/case_$1.$2)
            then
                # Stream matches reference
                let "sum_ok++"
                echo -n "OK         "
            else
                # Stream doesn't match reference
                let "sum_failed++"
                echo -n "FAILED      "
                # Store the failed case and reference
                saveFileToDir case_$1/run_$1.sh random_parameter_fails run_$1.sh
                saveFileToDir case_$1/case_$1.$2 random_parameter_fails case_$1.$2
                saveFileToDir case_$1/case_$1.log random_parameter_fails case_$1.log
                saveFileToDir case_$1/stream.$2 random_parameter_fails case_$1.$2.ref
                saveFileToDir case_$1/encode_$1.log random_parameter_fails case_$1.log.ref
            fi
        else
            let "sum_notrun++"
            echo -n "NOT_RUN     "
        fi
    else
        let "sum_noref++"
        echo -n "NO_REF      "
    fi
}

echo ""
echo "Creating random parameter cases..."
echo "Run test_random_parameter.sh to run the tests on board."

# Use system model script to create random parameter cases
export TEST_DATA_HOME=$PWD
cd $SYSTEM_MODEL_HOME
./test_data/test_random.sh all
cd -

echo "All random parameter cases created."
echo "Checking and copying failed cases to 'random_parameter_fails'..."

# VP8 cases with random parameters
first_case=5000
let "last_case=first_case+RANDOM_PARAMETER_CASE_AMOUNT"
for ((i=$first_case; i<$last_case; i++))
do
    check_case $i ivf
done

# H.264 cases with random parameters
first_case=6000
let "last_case=first_case+RANDOM_PARAMETER_CASE_AMOUNT"
for ((i=$first_case; i<$last_case; i++))
do
    check_case $i h264
done

# JPEG cases with random parameters
first_case=7000
let "last_case=first_case+RANDOM_PARAMETER_CASE_AMOUNT"
for ((i=$first_case; i<$last_case; i++))
do
    check_case $i jpg
done

echo ""
echo "Totals: $sum_ok OK   $sum_noref NO_REF   $sum_notrun NOT_RUN   $sum_failed FAILED"

