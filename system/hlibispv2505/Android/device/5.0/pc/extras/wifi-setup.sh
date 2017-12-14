#!/system/bin/sh
setprop wifi.interface.mac `cat /sys/class/net/eth0/address`

