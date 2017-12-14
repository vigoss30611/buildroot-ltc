#! /bin/sh

#sleep 1
echo 0 > /sys/class/infotm_usb/infotm0/enable # close usb
echo "uvc,mass_storage" > /sys/class/infotm_usb/infotm0/functions
cat /sys/class/infotm_usb/infotm0/functions
echo 1 > /sys/class/infotm_usb/infotm0/enable

