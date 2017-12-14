#! /bin/sh

#sleep 2
echo "" >  /sys/devices/platform/dwc_otg/gadget/lun0/file
echo 0 > /sys/class/infotm_usb/infotm0/enable
echo "uvc" > /sys/class/infotm_usb/infotm0/functions
echo 1 > /sys/class/infotm_usb/infotm0/enable
#sleep 1
