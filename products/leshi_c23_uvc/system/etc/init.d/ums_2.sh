#! /bin/sh

#echo 0 > /sys/kernel/debug/dwc_otg/link
#echo bbbbb
#sleep 3
#echo 1 > /sys/kernel/debug/dwc_otg/link 
echo "/dev/mmcblk0p1" >  /sys/devices/platform/dwc_otg/gadget/lun0/file
