#!/bin/sh

killall -9 udhcpd

ifconfig p2p0 down
sleep 1
ifconfig p2p0 192.168.155.1 up
#iwpriv p2p0 set p2pLinkDown=1
#iwpriv p2p0 set p2pReset=1

iwpriv p2p0 set p2pOpCh=1
iwpriv p2p0 set softapEnable=1
iwpriv p2p0 set P2pOpMode=1
iwpriv p2p0 set SSID=$1
ifconfig p2p0 up
iwpriv p2p0 set WPAPSK=admin888
iwpriv p2p0 set AuthMode=WPA2PSK
iwpriv p2p0 set EncrypType=AES
iwpriv p2p0 set SSID=$1
iwpriv p2p0 set p2pDevName=Direct-OTT

echo SSID=$1

#custom your ap mode's ip address here !!!
#ifconfig p2p0 192.168.155.1 up
#iwpriv p2p0 set AuthMode=WPA2PSK
#iwpriv p2p0 set EncrypType=AES
#iwpriv p2p0 set WPAPSK=admin888

route add -net 192.168.155.0 netmask 255.255.255.0 dev p2p0
ifconfig p2p0 192.168.155.1 netmask 255.255.255.0 up
echo 1 > /proc/sys/net/ipv4/ip_forward

#touch /var/lib/dhcp/dhcpd.leases
touch /var/lib/misc/udhcpd.leases
mkdir /var/run/dhcp-server
#/opt/ipnc/dhcpd -d -f -pf /var/run/dhcp-server/dhcpd.pid -cf /etc/dhcp/dhcpd_mt.conf p2p0 &
/bin/udhcpd -f /etc/dhcp/udhcpd.conf p2p0 &
