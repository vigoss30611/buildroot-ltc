#!/bin/bash

# Check that the dir is correct
if [ ! -e runall.sh ]
then
    echo "This script must be run in masterscript dir!"
    exit
fi

if [ ! -e test_h264.sh ]
then
    echo "test_h264.sh not found!"
    exit
fi

if [ ! -e test_vp8.sh ]
then
    echo "test_vp8.sh not found!"
    exit
fi

if [ ! -e test_jpeg.sh ]
then
    echo "test_jpeg.sh not found!"
    exit
fi

if [ ! -e test_vs.sh ]
then
    echo "test_vs.sh not found!"
    exit
fi

/sbin/rmmod memalloc
./memalloc_load.sh alloc_method=6

/sbin/rmmod hx280enc
./driver_load.sh

rm -rf run_done
rm -rf random_start

#Execute smoketests if not done already
if [ ! -e smoketest_done ]
then
    echo "Executing smoketests..."

    ./test_h264.sh 1000
    ./test_jpeg.sh 2000
    ./test_vp8.sh  3000
    ./test_vs.sh   1751

    touch smoketest_done

    echo "Smoketests done."
    echo "Now start checkall.sh."
    echo -n "Waiting for checkall to signal smoketest OK"
fi

while [ 1 ]
do

    echo -n "."
    sleep 10

    if [ -e smoketest_ok ]
    then

        if [ -e list ]
        then
            ./test_vp8.sh  -list
            ./test_h264.sh -list
            ./test_jpeg.sh -list
        fi

        echo ""
        echo "Executing test cases..."

        # First jpeg cases since they are only taking some minutes
        # H.264 cases takes hours
        ./test_jpeg.sh all
        ./test_vp8.sh all
        ./test_h264.sh all
        ./test_vs.sh all

        echo "Executing random parameter test cases... or abort with Ctrl-C"
        sleep 10

        ./test_random_parameter.sh

        echo "Next will be test cases in random order, you can abort with Ctrl-C."
        echo "Waiting for checker to signal random start..."
        sleep 20

        while [ ! -e random_start ]
        do
            sleep 10
        done

        echo "Executing test cases in random order..."

        ./test_random_cases.sh

        echo "Test cases finished!"

        touch run_done

        exit

    elif [ -e smoketest_not_ok ]
    then
        echo "Smoketest FAILED!"
        exit
    fi

done
