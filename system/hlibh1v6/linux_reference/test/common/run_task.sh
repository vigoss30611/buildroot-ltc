#!/bin/bash

#
# Run test cases from a task list
#

#Imports

.  ./f_run.sh
.  ./f_check.sh

importFiles

/sbin/rmmod hx280enc
./driver_load.sh

/sbin/rmmod memalloc
./memalloc_load.sh alloc_method=9

# Default parameters
min_rounds=1
max_rounds=3
singlecase=0
rerun=0
logfile=run_${hwtag}_$reporttime.log

# Check parameter
if [ $# -eq 0 ]; then
    echo "Usage: $0 <task/list | casenum> [min_rounds] [max_rounds] [re-run]"
    echo "  min_rounds - the minimum number of rounds to run each case (default=1)"
    echo "  max_rounds - the maximum number of rounds to repeat when a case fails (default=3)"
    echo "  re-run     - remove previous run output"
    exit
fi

# Parse parameters
if [ ! -s $1 ]; then
    if [ ! -s task/run_$1.sh ]; then
        echo "Task list $1 nor task/run_$1.sh not found."
        exit
    else
        singlecase=$1
    fi
fi
if [ $# -ge 2 ]; then
    min_rounds=$2
fi
if [ $# -ge 3 ]; then
    max_rounds=$3
fi
if [ $# -gt 3 ]; then
    rerun=1
fi


comparestream() {
    
    if [ ! -s $1 ]; then
        comparecomment="No stream."
    elif [ ! -s $2 ]; then
        comparecomment="No reference."
    else
        # Stream and reference exists, compare
        if (cmp -s $1 $2); then
            # Match
            return 0 # Compare success
        else
            error_byte=`cmp $1 $2 | awk '{print $5}' | awk 'BEGIN {FS=","} {print $1}'`
            size1=`ls -l $1 | awk '{print $5}'`
            size2=`ls -l $2 | awk '{print $5}'`
            comparecomment="Stream diff at $error_byte, sizes $size1 $size2."
            return 1 # Compare fails
        fi
    fi
    return 2 # Not possible to compare
}

casecount=0
casefailed=0
caseok=0
basedir=`pwd`

# Run single case or a task list of cases
if [ $singlecase -gt 0 ]; then
    caseamount=1
    cases=$singlecase
else
    caseamount=0
    cases=`cat $1`
    for casenum in $cases
    do
        let "caseamount++"
    done
fi

if [ $rerun -eq 1 ]; then
    echo "Re-running $caseamount cases, rounds [$min_rounds-$max_rounds], log file '$logfile'"
    echo "Re-running $caseamount cases, rounds [$min_rounds-$max_rounds]" > $logfile
else
    echo "Running $caseamount cases, rounds [$min_rounds-$max_rounds], log file '$logfile'"
    echo "Running $caseamount cases, rounds [$min_rounds-$max_rounds]" > $logfile
fi

# Create link run.log to latest run
if [ -e run.log ]; then rm run.log; fi
ln -s $logfile run.log

# Main loop
for casenum in $cases
do
    let "casecount++"

    roundfailed=0
    roundok=0
    for ((round=1; round<=$max_rounds; round++))
    do

        # Print progress
        echo -en "\rRUN [$casecount/$caseamount]  $caseok OK  $casefailed FAILED  Case=${casenum}_$round  "

        # Create case dir
        casedir=case_${casenum}_$round
        if [ ! -e $casedir ] ; then
            mkdir $casedir
            chmod 777 $casedir
        else
            # Case dir exists, check if re-run is needed
            if [ ! -s $casedir/status ] ; then
                echo -n " $casedir seems to be incomplete, re-running..."
                rm -f $casedir/*
            else
                if [ $rerun -eq 1 ] ; then
                    rm -f $casedir/*
                else
                    continue
                fi
            fi
        fi

        if [ ! -e $casedir ] ; then
            continue
        fi

        startTimeCounter $casedir

        comparecomment=""
        (
            # Run case in case dir
            cd $casedir
            ln -s ../vp8_testenc .
            ln -s ../h264_testenc .
            ln -s ../jpeg_testenc .
            if [ -s $basedir/task/run_${casenum}.sh ] ; then
                $basedir/task/run_${casenum}.sh &> log
                echo "$?" > status
            else
                echo "1" > status
                comparecomment="No run script."
            fi
        )

        endTimeCounter $casedir

        # Compare to reference stream if exists
        fail=2
        if [ -e ref/case_$casenum/stream.ivf ] ; then
            comparestream $casedir/stream.ivf ref/case_$casenum/stream.ivf
            fail=$?
        elif [ -e ref/case_$casenum/stream.h264 ] ; then
            comparestream $casedir/stream.h264 ref/case_$casenum/stream.h264
            fail=$?
        elif [ -e ref/case_$casenum/stream.jpg ] ; then
            comparestream $casedir/stream.jpg ref/case_$casenum/stream.jpg
            fail=$?
        fi

        # Print log file
        if [ $fail -eq 0 ]; then
            echo "Case $casenum Round $round OK" >> $logfile
            let "roundok++"
        elif [ $fail -eq 1 ]; then
            echo "Case $casenum Round $round FAILED: $comparecomment" >> $logfile
            let "roundfailed++"
        else
            echo "Case $casenum Round $round         $comparecomment" >> $logfile
        fi

        # Check if no more rounds is needed
        if [ $round -ge $min_rounds ] && [ $fail -ne 1 ]; then
            round=$max_rounds
        fi
    done

    # This case failed atleast once
    if [ $roundfailed -gt 0 ]; then
        let "casefailed++"
    elif [ $roundok -gt 0 ]; then
        let "caseok++"
    fi

done

echo ""
echo "Total of $casecount cases:  $caseok OK  $casefailed FAILED"
echo ""
