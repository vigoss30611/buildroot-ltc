#!/bin/sh

umount /mnt/sd0/ 2>&- 1>&-
if [ $? != 0 ]; then
	echo "umount /mnt/sd0/ $?"
fi

mkfs.fat /dev/mmcblk0p1 2>&- 1>&-
if [ $? != 0 ]; then
	echo "mkfs.fat $?"
fi

mount /dev/mmcblk0p1 /mnt/sd0 2>&- 1>&-
if [ $? != 0 ]; then
	echo "mount $?"
fi
#file end

