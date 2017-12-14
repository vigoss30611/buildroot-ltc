#!/bin/bash

#Imports
.  ./f_check.sh

importFiles

singlecase=0

# Check parameter
if [ $# -eq 0 ]; then
    echo "Usage: $0 <task/list | casenum>"
    exit
fi

# Parse parameters
if [ ! -e $1 ]; then
    if [ ! -e task/run_$1.sh ]; then
        echo "Task list $1 nor task/run_$1.sh not found."
        exit
    else
        singlecase=$1
    fi
fi

casecount=0

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

echo "Create reference data in $TEST_DATA_HOME for $caseamount cases <$1>"

# Main loop
for casenum in $cases
do
    let "casecount++"

    # Print progress
    echo -en "\rREF [$casecount/$caseamount]  Case=$casenum                                        "

    # Reference test data dir
    casedir=$TEST_DATA_HOME/case_$casenum

    # If reference test data doesn't exist, generate it
    if [ ! -e $casedir ]
    then
        (
        cd $SYSTEM_MODEL_HOME
        ./test_data/test_data.sh $casenum >> /dev/null 2>&1
        )
    fi
done

echo ""
echo ""



