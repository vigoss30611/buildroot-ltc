#!/bin/sh

r=1
while true; do
echo -e "\nkill videobox"
killall videoboxd
while [ -n "`ps | grep -v grep | grep videoboxd`" ]; do
	usleep 200000;
done

echo -e "\n\n########### BIG ROUND: $((r++))\n"
/nfs/videoboxd /nfs/d304.json

for i in 1 2 3 4 5 6 7 8 9 0; do
	echo -e "\nround: $i"
	for o in ispost0-ss0 ispost0-ss1 ispost0-dn isp-out; do
		vbctrl rebind display-osd0 $o
		usleep 200000
	done
done
done

