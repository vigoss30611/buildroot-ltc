#!/bin/bash

# Kill background tasks on interrupt
function cleanup {
	pkill -g $$
	pkill -9 -g $$
}
trap cleanup SIGINT SIGTERM

getsw=0
movietests=0
createlist=1
usage="Usage: $0 [--getsw] [--oldlist] [--movie]"

echo -e $usage
# Optional parameters
while [ $# -ge 1 ]; do
        case $1 in
        "--movie")  movietests=1;;
        "--getsw")  getsw=1;;
        "--oldlist")createlist=0;;
        "*")        exit 1 ;;
        esac

        shift
done

# import commonconfig
if [ ! -e commonconfig.sh ]; then
    echo "commonconfig.sh not found!"
    exit -1
else
    .  ./commonconfig.sh
fi

tasklist="task/list_$hwtag"
fpgaip="172.28.107.$testdeviceip"
fpgadir=$PWD

echo "Reading parameters from commonconfig.sh..."
echo "Running all FPGA tests on $testdeviceip, time is $reporttime"
echo "HW  tag $hwtag to be tested"
echo "HW  config tag $cfgtag to be tested"
echo "SYS tag $systag to be compared against"
if [ $getsw -eq 1 ]; then
    echo "SW  tag $swtag will be checked out"
else
    echo "SW in current HEAD will be used"
fi
if [ -e $tasklist ]; then
    echo "$tasklist exists"
fi
echo "Starting in 10 seconds..."
sleep 10

# --getsw Checkout SW tag from git
if [ $getsw -eq 1 ]; then
    echo "Checking out SW tag $swtag"

    # Save commonconfig.sh
    cp commonconfig.sh commonconfig.sh.new
    git checkout commonconfig.sh

    # Update SW to the tagged version
    git diff > gitdiff
    if [ -s gitdiff ]; then
        echo "Git differences found! Ctrl-c now or you will lose them! Press any key to continue. "
        read key
        git reset --hard
    fi

    git checkout master
    git pull
    git checkout -b branch_$swtag $swtag

    # Get saved commonconfig
    cp commonconfig.sh.new commonconfig.sh
fi

# Run set.sh to compile binaries and checkout system model
./set.sh
retval=$?
if [ $retval -ne 0 ]
then
    exit $retval
fi

# Get the HW config from FPGA
rm -f hwconfig
echo "cd $fpgadir" > runner.sh
echo "./driver_load.sh" >> runner.sh
echo "./vp8_testenc --config > hwconfig" >> runner.sh
chmod 777 runner.sh
echo Running testbench on FPGA: ssh -l root $fpgaip $fpgadir/runner.sh
ssh -l root $fpgaip $fpgadir/runner.sh

if [ -e hwconfig ]; then
    echo "Found HW with config:"
    cat hwconfig
fi

# Commit changes
echo "Testing $cfgtag on $testdeviceip, time is $reporttime" > commitmsg
echo "" >> commitmsg
cat hwconfig >> commitmsg
git commit -a -F commitmsg

# Create task
if [ $createlist -eq 1 ] || [ ! -e $tasklist ]; then
    echo "Creating tasklist $tasklist with params: $tasklistparams"

    # Create task/run scripts from system model
    ./testcaselist.sh

    # Create task list, cases are selected in commonconfig, hwconfig affects cases
    python create_tasklist.py $tasklist $tasklistparams
fi

firstcase=`head -1 $tasklist`
lastcase=`tail -1 $tasklist`
echo "First case is $firstcase"
echo "Last case is $lastcase"

# Reference creation on background
./create_reference.sh $tasklist &

# Sleep for a while to give a head start for reference creation
sleep 30

if [ ! -e case_${lastcase}_1 ]; then
    # Create runner script for fpga board
    echo "cd $fpgadir" > runner.sh
    echo "./run_task.sh $tasklist" >> runner.sh
    chmod 777 runner.sh

    # Start runner script on fpga
    echo Starting FPGA runner on background: ssh -l root $fpgaip $fpgadir/runner.sh
    ssh -l root $fpgaip $fpgadir/runner.sh &
else
    echo "All cases seem to be run previously, continue to checking..."
fi

# Wait for last case reference to finish
while [ ! -e ref/case_$lastcase ]; do
    sleep 10
done

echo "All references created..."

# Wait for reference and run processes to finish
wait

echo "Finished ref/case_$lastcase and case_${lastcase}_1, now check..."

db_check_param=""
if [ "$use_db" == "y" ]
then
    db_check_param="--db"
fi

# Final check for all the cases, results are not added to data base this time
#python check_task.py $tasklist $db_check_param
python check_task.py $tasklist

# If some cases failed, run more rounds for those
if [ -s task/failed ]; then
    # Create runner script for fpga board
    echo "cd $fpgadir" > runner.sh
    echo "./run_task.sh task/failed 2 5" >> runner.sh
    chmod 777 runner.sh

    # Start runner script on fpga
    echo Starting FPGA runner: ssh -l root $fpgaip $fpgadir/runner.sh
    ssh -l root $fpgaip $fpgadir/runner.sh
fi

# Second check for all the cases, includes failed cases second run; add cases to data base if enabled
python check_task.py $tasklist $db_check_param

if [ $movietests -eq 1 ]; then
    # Run the movie cases
    for i in 0 1 2 3;
    do
        let "j=i/2" # Input file index

        if [ ! -e movie_${i}_hw.log ]; then
            echo "Start movie test $i, input ${movieinput[$j]}, encoder log in movie_${i}_hw.log"
            # Start movie test decoder
            rm -f EOF
            ./vx170dec_pclinux_multifileoutput -P -L -Oout.yuv ${movieinput[$j]} > movie_${i}_dec.log &

            # Create runner script for fpga board
            echo "cd $fpgadir" > runner.sh
            echo "./movie_encode.sh $i hw" >> runner.sh
            chmod 777 runner.sh

            # Start movie test encoder
            sleep 100
            echo Starting FPGA runner on background: ssh -l root $fpgaip $fpgadir/runner.sh
            ssh -l root $fpgaip $fpgadir/runner.sh &

            # Wait for decoder and runner to finish
            wait
        fi
    done
fi
