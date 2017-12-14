#!/bin/sh

ifconfig ra0 down

killall -9 dhcpcd
killall -9 wpa_supplicant
rm /var/run/dhcpcd-ra0.pid
#iwpriv p2p0 set P2pOpMode=2

ifconfig ra0 up
#custom your wpa_upplicant.conf file path here!

cat > /tmp/wpa_supplicant.conf << EOF
ctrl_interface=/var/run/wpa_supplicant
update_config=1
ap_scan=1

network={
    ssid="$1"
    pairwise=TKIP CCMP
    group=TKIP CCMP
    key_mgmt=$2
    psk="$3"
} 
EOF

wpa_supplicant -B -i ra0 -D wext -c /tmp/wpa_supplicant.conf -d &
sleep 5
dhcpcd ra0 &

#sleep 2
#iwpriv p2p0 set P2pOpMode=1
