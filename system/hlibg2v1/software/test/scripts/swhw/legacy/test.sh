#!/bin/bash
#-------------------------------------------------------------------------------
#-                                                                            --
#-       This software is confidential and proprietary and may be used        --
#-        only as expressly authorized by a licensing agreement from          --
#-                                                                            --
#-                            Hantro Products Oy.                             --
#-                                                                            --
#-                   (C) COPYRIGHT 2011 HANTRO PRODUCTS OY                    --
#-                            ALL RIGHTS RESERVED                             --
#-                                                                            --
#-                 The entire notice above must be reproduced                 --
#-                  on all copies and should not be removed.                  --
#-                                                                            --
#-------------------------------------------------------------------------------
#-
#--   Abstract     : Script for setting up configuration(s), compiling SW,and --
#=--                 running and checking the SW/HW verification test cases.  --
#--                                                                           --
#-------------------------------------------------------------------------------

#=====--------------------------------------------------------------------=====#

. ./commonconfig.sh

if [ "$TB_REMOTE_TEST_EXECUTION_IP" == "LOCAL_HOST" ]
then
    echo "test.sh requires remote execution: set TB_REMOTE_TEST_EXECUTION_IP @ commonconfig.sh"
    exit 1
fi

# prepare for test execution and compile time flags
if (! ./setupconfig.sh)
then
    exit 1
fi

# set compiler settings & build
. "$COMPILER_SETTINGS"
./make_all.sh

# check all the cases, generate test reports and summarize
xterm -e "cd $PWD && ./checkall.sh $1 >& check.log" &

echo -n "Running cases... "
reboot_number=1
while [ 1 ]
do
    # run all the cases
    rexec -l root -p xxx "$TB_REMOTE_TEST_EXECUTION_IP" "cd $PWD && ./runall.sh" > run.log.${reboot_number} 2>&1 &
    pid=$!
    
    # start monitor
    ./monitor.sh "$TB_REMOTE_TEST_EXECUTION_IP" 20 30 1800 > monitor.log 2>&1
    if [ $? == 2 ]
    then
        mv monitor.log "monitor.log.${reboot_number}"
        gzip "monitor.log.${reboot_number}"
        let 'reboot_number=reboot_number+1'
    fi
    kill -9 "$pid" >> /dev/null
    
    if [ -e "run_done" ]
    then
        echo "DONE"
        break
    fi
done

./summarize.sh
