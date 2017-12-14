#!/bin/sh

r=1
echo -e "\nkill videobox"
killall videoboxd
while [ -n "`ps | grep -v grep | grep videoboxd`" ]; do
	usleep 200000;
done

videoboxd /nfs/p1.json > /nfs/torment_repath.log 2>&1
while true; do

	echo -en "\r########### ROUND: $((r++))"
	vbctrl repath /nfs/p2.json
	vbctrl repath /nfs/p1.json
	usleep 200000

done

