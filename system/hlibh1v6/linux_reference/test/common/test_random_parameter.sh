#!/bin/bash

#Imports
.  ./f_check.sh

importFiles

# Output file when running all cases
resultfile=results_random_parameter

echo "Running test cases with random parameters."
echo "This will take several minutes."
echo "Output is stored in $resultfile.log"
rm -f $resultfile.log


run_case()
{
    echo -en "\rCase $1   "
    
    # case dirs are created by checkall_random_parameter.sh
    # case dir contains the run.sh script with random parameters
    while [ ! -e case_$1/run.sh ]
    do
        sleep 10
        echo -n "."
    done

    # replace system model created run_x.sh with correct executable
    sed "s|.*h264_testenc|./h264_testenc|" case_$1/run.sh > tmp1
    sed "s|.*jpeg_testenc|./jpeg_testenc|" tmp1 > tmp2
    sed "s|.*vp8_testenc|./vp8_testenc|" tmp2 > case_$1/run_$1.sh
    chmod a+x case_$1/run_$1.sh

    # encode with random parameters
    ./case_$1/run_$1.sh > case_$1/case_$1.log
    
    cat case_$1/case_$1.log >> $resultfile.log

    # Store the run log and stream in the case dir for check
    if [ -e stream.$2 ]
    then
        mv -f stream.$2 case_$1/case_$1.$2
    fi
}

# VP8 cases with random parameters
first_case=5000
let "last_case=first_case+RANDOM_PARAMETER_CASE_AMOUNT"
for ((i=$first_case; i<$last_case; i++))
do
    run_case $i ivf
done

# H.264 cases with random parameters
first_case=6000
let "last_case=first_case+RANDOM_PARAMETER_CASE_AMOUNT"
for ((i=$first_case; i<$last_case; i++))
do
    run_case $i h264
done

# JPEG cases with random parameters
first_case=7000
let "last_case=first_case+RANDOM_PARAMETER_CASE_AMOUNT"
for ((i=$first_case; i<$last_case; i++))
do
    run_case $i jpg
done

