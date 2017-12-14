killall hostapd
/usr/sbin/hostapd /tmp/hostapd.conf &
ifconfig wlan1 192.168.155.1 netmask 255.255.255.0
hostapd_cli wps_config IPC_INFOTM WPA2PSK CCMP admin888
touch /var/lib/misc/udhcpd.leases
udhcpd -cf /etc/dhcp/udhcpd.conf wlan1 &
