#!/bin/bash

#Imports

.  ./f_check.sh

importFiles

trigger=-1
first_trace_pic=0

if [ ! -e $test_case_list_dir/test_data_parameter_vp8.sh ]; then
    echo "$test_case_list_dir/test_data_parameter_vp8.sh not found"
    exit -1
fi

if [ ! -e $test_case_list_dir/test_data_parameter_h264.sh ]; then
    echo "$test_case_list_dir/test_data_parameter_h264.sh not found"
    exit -1
fi

# Give lab board access to write this dir
chmod 777 .

# Create new random cases with system model
sh $test_case_list_dir/test_random.sh scripts

# Encode command
enc_bin=./vp8_testenc
touch $enc_bin
. $test_case_list_dir/encode_vp8.sh

# Create task dir
if [ ! -e task ]; then
    mkdir task
fi

# Create task/run_xxxx.sh script for every VP8 test case
first_case=3000;
last_case=3999;
for ((case_nro=$first_case; case_nro<=$last_case; case_nro++))
do
    let "set_nro=${case_nro}-${case_nro}/5*5"
    . $test_case_list_dir/test_data_parameter_vp8.sh "$case_nro"
    if [ ${valid[${set_nro}]} -eq 0 ]; then
        nice_bin="echo"
        encode > task/run_${case_nro}.sh
    fi
done

first_case=5000;
last_case=5999;
for ((case_nro=$first_case; case_nro<=$last_case; case_nro++))
do
    if [ -e $test_case_list_dir/run_${case_nro}.sh ]; then
        cp $test_case_list_dir/run_${case_nro}.sh task
    fi
done

# Encode command
enc_bin=./h264_testenc
touch $enc_bin
. $test_case_list_dir/encode_h264.sh

# Create task/run_xxxx.sh script for every H264 test case
first_case=1000;
last_case=1999;
for ((case_nro=$first_case; case_nro<=$last_case; case_nro++))
do
    let "set_nro=${case_nro}-${case_nro}/5*5"
    . $test_case_list_dir/test_data_parameter_h264.sh "$case_nro"
    if [ ${valid[${set_nro}]} -eq 0 ]; then
        nice_bin="echo"
        encode > task/run_${case_nro}.sh
    fi
done

first_case=6000;
last_case=6999;
for ((case_nro=$first_case; case_nro<=$last_case; case_nro++))
do
    if [ -e $test_case_list_dir/run_${case_nro}.sh ]; then
        cp $test_case_list_dir/run_${case_nro}.sh task
    fi
done

# Encode command
enc_bin=./jpeg_testenc
touch $enc_bin
. $test_case_list_dir/encode_jpeg.sh

# Create task/run_xxxx.sh script for every JPEG test case
first_case=2000;
last_case=2999;
for ((case_nro=$first_case; case_nro<=$last_case; case_nro++))
do
    let "set_nro=${case_nro}-${case_nro}/5*5"
    . $test_case_list_dir/test_data_parameter_jpeg.sh "$case_nro"
    if [ ${valid[${set_nro}]} -eq 0 ]; then
        nice_bin="echo"
        encode > task/run_${case_nro}.sh
    fi
done

first_case=7000;
last_case=7999;
for ((case_nro=$first_case; case_nro<=$last_case; case_nro++))
do
    if [ -e $test_case_list_dir/run_${case_nro}.sh ]; then
        cp $test_case_list_dir/run_${case_nro}.sh task
    fi
done

chmod a+rx task/*sh

