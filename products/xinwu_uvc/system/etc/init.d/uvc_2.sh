#! /bin/sh

echo 0 > /sys/class/infotm_usb/infotm0/enable
echo 0001 > /sys/class/infotm_usb/infotm0/idProduct
echo "uvc" > /sys/class/infotm_usb/infotm0/functions
echo 1 > /sys/class/infotm_usb/infotm0/enable
# usb reconnct
#echo 0 > /sys/kernel/debug/dwc_otg/link
#sleep 1
#echo 1 > /sys/kernel/debug/dwc_otg/link
