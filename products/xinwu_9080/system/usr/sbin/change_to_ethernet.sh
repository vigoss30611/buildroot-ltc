#!/bin/sh

echo 0 > /sys/bus/platform/drivers/dwc_otg/is_host

echo 0 > /sys/class/infotm_usb/infotm0/enable
echo 0002 > /sys/class/infotm_usb/infotm0/idProduct
echo "rndis" > /sys/class/infotm_usb/infotm0/functions
echo 1 > /sys/class/infotm_usb/infotm0/enable

usleep 500000

#echo 124 > /sys/class/gpio/export
#echo out > /sys/class/gpio/gpio124/direction
#echo 1 > /sys/class/gpio/gpio124/value

echo 0 > /sys/kernel/debug/dwc_otg/link
echo 1 > /sys/kernel/debug/dwc_otg/link

ifconfig usb0 up
ifconfig usb0 172.3.0.2

start-stop-daemon -S -q -p /var/run/lighttpd.pid --exec /usr/sbin/lighttpd -- -f /etc/lighttpd/lighttpd.conf
