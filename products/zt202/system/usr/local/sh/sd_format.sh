#!/bin/sh

umount /mnt/mmc/ 2>&- 1>&-
if [ $? != 0 ]; then
	echo "umount /mnt/mmc/ $?"
fi

mkfs.fat /dev/mmcblk0p1 2>&- 1>&-
if [ $? != 0 ]; then
	echo "mkfs.fat $?"
fi

mount /dev/mmcblk0p1 /mnt/mmc 2>&- 1>&-
if [ $? != 0 ]; then
	echo "mount $?"
fi
#file end

