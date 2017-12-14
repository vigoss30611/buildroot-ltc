#!/bin/sh

module="hx280enc"
device="/tmp/dev/hx280"
mode="666"

echo

if [ ! -e /tmp/dev ]
then
    mkdir /tmp/dev/
fi

#insert module
insmod $module.ko $* || exit 1

echo "module $module inserted"

#remove old nod
rm -f $device

#read the major asigned at loading time
major=`cat /proc/devices | grep $module | cut -c1-3`

echo "$module major = $major"

#create dev node
mknod $device c $major 0

echo "node $device created"

#give all 'rw' access
chmod $mode $device

echo "set node access to $mode"

#the end
echo
