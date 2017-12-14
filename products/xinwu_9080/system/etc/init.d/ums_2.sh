#! /bin/sh

#echo 0 > /sys/kernel/debug/dwc_otg/link
#echo bbbbb
#sleep 3
#echo 1 > /sys/kernel/debug/dwc_otg/link 
echo 0 > /sys/class/infotm_usb/infotm0/enable # close usb
echo "uvc,mass_storage" > /sys/class/infotm_usb/infotm0/functions
echo 1 > /sys/class/infotm_usb/infotm0/enable
echo "/dev/mmcblk0p1" >  /sys/devices/platform/dwc_otg/gadget/lun0/file
