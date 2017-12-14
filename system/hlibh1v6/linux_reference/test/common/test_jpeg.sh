#!/bin/bash

#Imports

.  ./f_run.sh
.  ./f_check.sh

importFiles


# This script runs a single test case or all test cases

# SET THIS
# Choose whether to run all test like defined in parameter.sh (0) or all cases run in "whole frame" mode (1).
encodeType=0

OVERWRITE_CASE=0

# Encoder bin
enc_bin=./jpeg_testenc
if [ ! -e $enc_bin ]; then
    echo "Executable $enc_bin not found."
    exit
fi

nice_bin=""

# Test case parameter definitions
if [ ! -e $test_case_list_dir/test_data_parameter_jpeg.sh ]; then
    echo "$test_case_list_dir/test_data_parameter_jpeg.sh not found"
    exit -1
fi

# Output file when running all cases
# .log and .txt are created
resultfile=results_jpeg

# Current date
#date=`date +"%d.%m.%y %k:%M"`

# Check parameter
if [ $# -eq 0 ]; then
    echo "Usage: test_jpeg.sh <test_case_number>"
    echo "   or: test_jpeg.sh <all>"
    echo "   or: test_jpeg.sh <first_case> <last_case>"
    exit
fi

# Encode command
. $test_case_list_dir/encode_jpeg.sh

encode_whole_mode() {
        codingtype=( 0 0 0 0 0 )
        encode
}


test_set() {
    # Test data dir
    let "set_nro=${case_nro}-${case_nro}/5*5"
    case_dir=$test_data_home/case_$case_nro

    # Do nothing if test case is not valid
    if [ ${valid[${set_nro}]} -eq -1 ]; then
        echo "case_$case_nro is not valid."
    else
            # Invoke new shell
            (
            echo "========================================================="
            echo "Run case $case_nro"

            if [ ! -e $TESTDIR/case_${case_nro} ]
            then
                mkdir $TESTDIR/case_${case_nro}
                chmod 777 $TESTDIR/case_${case_nro}
            else
                if [ $OVERWRITE_CASE -eq 0 ]
                then
                    echo "Case dir exists, don't overwrite."
                    return
                fi
                rm -f $TESTDIR/case_${case_nro}/status
            fi

            rm -f stream.jpg

            startTimeCounter $TESTDIR/case_${case_nro}

            # Run encoder
            if [ $encodeType -eq 0 ];
            then
                encode > $TESTDIR/case_${case_nro}/case_$case_nro.log
            else
                echo "NOTE! FORCING WHOLE FRAME MODE"

                encode_whole_mode > $TESTDIR/case_${case_nro}/case_$case_nro.log
            fi

            echo "$?" > status
            cat $TESTDIR/case_${case_nro}/case_$case_nro.log

            endTimeCounter $TESTDIR/case_${case_nro}

            if [ -e stream.jpg ]
            then
                mv -f stream.jpg ${TESTDIR}/case_${case_nro}/case_${case_nro}.jpg
            else
                # No output stream created, something went wrong
                # Make script run.sh for this testcase
                nice_bin="echo"
                encode > ${TESTDIR}/case_${case_nro}/run_jpeg.sh
            fi

            if ( [ "$TRACE_SW_REGISTER" == "y" ] ) && [ -e sw_reg.trc ]
            then
                mv -f sw_reg.trc $TESTDIR/case_${case_nro}
            fi
            if ( [ "$TRACE_EWL" == "y" ] ) && [ -e ewl.trc ]
            then
                mv -f ewl.trc $TESTDIR/case_${case_nro}
            fi

            mv status $TESTDIR/case_${case_nro}/status
            )
    fi
}

run_cases() {
    
    echo "Running test cases $first_case..$last_case"
    echo "This will take several minutes"
    echo "Output is stored in $resultfile.log and $resultfile.txt"
    rm -f $resultfile.log $resultfile.txt
    for ((case_nro=$first_case; case_nro<=$last_case; case_nro++))
    do 
        echo -en "\rCase $case_nro "  
        . $test_case_list_dir/test_data_parameter_jpeg.sh "$case_nro"
        test_set >> $resultfile.log
    done
    echo -e "\nDone\n" 
}

run_case() {

    . $test_case_list_dir/test_data_parameter_jpeg.sh "$case_nro"
    test_set
}

# Encoder test set.
trigger=-1 # NO Logic analyzer trigger by default

case "$1" in
    all)
    first_case=2000
    last_case=2999
    run_cases;;

    -list)
    echo "Running failed cases from file 'list':"
    list=`cat list`
    echo "$list"
    for case_nro in $list
    do
        echo -en "\rCase $case_nro "  
        run_case >> $resultfile.log
    done
    ;;

    *)
    if [ $# -eq 1 ]; then
        case_nro=$1
        run_case
    else
        first_case=$1
        last_case=$2
        run_cases
    fi;;
esac

