#!/bin/bash
#Copyright 2013 Google Inc. All Rights Reserved.
#Author: mkarki@google.com (Matti Kärki)
#
#Description: Kills the test scipts from PCIe boards.

. ./defs.sh

g_exit_status=0

killProcess()
{
    local process_name=$1
    
    local processes=`ssh ${USER}@$ip "ps -u root |grep ${process_name}" |awk '{print $1}' |tr "\n" " "`
    for process in $processes
    do
        echo -n "Killing ${process_name}:${process} @ ${ip}... "
        ssh ${USER}@$ip "echo ${process} |xargs sudo kill -9"
        if [ $? -ne 0 ]
        then
            echo "failed"
            g_exit_status=1
        else
            echo "OK"
        fi
    done
}

if [ ! -e $SETUP_INFO ]
then
    echo "No setup.info found -> exiting!"
    exit 0
fi
board_ips=`cat $SETUP_INFO |awk -F\: '{print $1}'`
for ip in $board_ips
do
    killProcess "verify.sh"
    killProcess "g2dec"
    killProcess "g2model"
done
exit $g_exit_status
