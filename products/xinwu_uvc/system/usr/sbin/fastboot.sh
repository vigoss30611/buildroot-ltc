mkdir -p /dev/usb-ffs/fastboot
mount -t functionfs fastboot /dev/usb-ffs/fastboot
fastbootd &
killall sys-watchdog
killall videoboxd
echo 0 > /sys/class/infotm_usb/infotm0/enable
sleep 2
echo 0003 > /sys/class/infotm_usb/infotm0/idProduct
echo "ffs"  > /sys/class/infotm_usb/infotm0/functions
echo 1 > /sys/class/infotm_usb/infotm0/enable
echo 0 > /sys/kernel/debug/dwc_otg/link
echo 1 > /sys/kernel/debug/dwc_otg/link
