#!/bin/bash

# This script runs a single test case or all test cases

#Imports

.  ./f_run.sh
.  ./f_check.sh

importFiles

mount_point=/export # this is for versatile
if [ ! -e $mount_point ]; then
    mount_point=/mnt/export # this is for integrator
fi

#SET THIS
# HW burst size 0..63
burst=16


# Output file when running all cases
# .log and .txt are created
resultfile=results_vs

# Current date
#date=`date +"%d.%m.%y %k:%M"`

# Check parameter
if [ $# -eq 0 ]; then
    echo "Usage: $0 <test_case_number>"
    echo "   or: $0 <all>"
    echo "   or: $0 <first_case> <last_case>"
    exit -1
fi

vs_bin=./videostabtest

if [ ! -e $vs_bin ]; then
    echo "Executable $vs_bin not found. Compile with 'make'"
    exit -1
fi

if [ ! -e $test_case_list_dir/test_data_parameter_h264.sh ]; then
    echo "<test_data_parameter_h264.sh> not found"
    echo "$test_case_list_dir/test_data_parameter_h264.sh" 
    exit -1
fi

process() { 

    # run only stab cases */    
    if [ ${stabilization[${set_nro}]} -eq 0 ]; then
        return        
    fi
    
    ${vs_bin} \
    -i${input[${set_nro}]} \
    -a${firstPic[${set_nro}]} \
    -b${lastPic[${set_nro}]} \
    -w${lumWidthSrc[${set_nro}]} \
    -h${lumHeightSrc[${set_nro}]} \
    -W${width[${set_nro}]} \
    -H${height[${set_nro}]} \
    -l${inputFormat[${set_nro}]} \
    -T \
    -N${burst} \
    -P$trigger
    
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
            echo "Run stabilization case $case_nro"

            if [ ! -e $TESTDIR/vs_case_${case_nro} ]
            then
                mkdir $TESTDIR/vs_case_${case_nro}
                chmod 777 $TESTDIR/vs_case_${case_nro}
            else
                rm -f $TESTDIR/vs_case_${case_nro}/status
            fi

            # Run stabilization

            startTimeCounter $TESTDIR/vs_case_${case_nro}

            process > vs.log
            echo "$?" > status

            cat vs.log

            endTimeCounter $TESTDIR/vs_case_${case_nro}

            mv -f vs.log ${TESTDIR}/vs_case_${case_nro}/vs_case_$case_nro.log
            if [ -e video_stab_result.log ]
            then
                mv -f video_stab_result.log ${TESTDIR}/vs_case_${case_nro}/vs_case_$case_nro.trc
            fi

            if ( [ "$TRACE_SW_REGISTER" == "y" ] ) && [ -e sw_reg.trc ]
            then
                mv -f sw_reg.trc $TESTDIR/vs_case_${case_nro}
            fi
            if ( [ "$TRACE_EWL" == "y" ] ) && [ -e ewl.trc ]
            then
                mv -f ewl.trc $TESTDIR/vs_case_${case_nro}
            fi

            # Make script run.sh for this testcase
            vs_bin="echo $vs_bin "
            process >  $TESTDIR/vs_case_${case_nro}/run_vs.sh

            mv status $TESTDIR/vs_case_${case_nro}/status

            )
    fi
}

run_cases() {    
    echo "Running test cases $first_case..$last_case"
    echo "This will take several minutes"
    echo "Output is stored in $resultfile.log"
    rm -f $resultfile.log
    for ((case_nro=$first_case; case_nro<=$last_case; case_nro++))
    do
        . $test_case_list_dir/test_data_parameter_h264.sh "$case_nro"
        echo -en "\rCase $case_nro\t"
        test_set >> $resultfile.log
    done
    echo -e "\nAll done!"
}

run_case() {
    . $test_case_list_dir/test_data_parameter_h264.sh "$case_nro"
    test_set
}

# vs test set.
trigger=-1 # NO Logic analyzer trigger by default

case "$1" in
    all)
    first_case=1750;
    last_case=1799;
    run_cases
    ;;
    *)
    if [ $# -eq 3 ]; then trigger=$3; fi
    if [ $# -eq 1 ]; then
        case_nro=$1;
        run_case;
    else
        first_case=$1;
        last_case=$2;
        run_cases;
    fi
    ;;
esac


