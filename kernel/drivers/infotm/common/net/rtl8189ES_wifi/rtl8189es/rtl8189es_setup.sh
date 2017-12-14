#!/bin/sh

AP_PATH="/mnt1/wifi/rtl8189es"

if [ ""$1 = "CTO" ]; then

	status=`${AP_PATH}/wpa_cli -i wlan0 status`
	if [ `echo $status | grep COMPLETED | wc -l` -eq 1 ];then
		echo $status | awk -F '=| ' '{print $4}'         
	fi
	return 0; 
fi


if [ ""$1 = "STA" ]; then

    rmmod 8189es
    killall -9 dhcpcd
    killall -9 wpa_supplicant

    killall -9 hostapd
    killall -9 udhcpd
	
    insmod /mnt1/ko/8189es.ko
    sleep 3
    echo 1 > /sys/class/rfkill/rfkill1/state
    for i in {1.. 5}; do
    	killall -9 dhcpcd
    	killall -9 wpa_supplicant

    	killall -9 hostapd
    	killall -9 udhcpd
            
        busybox ifconfig wlan0 down

        rm -f /var/run/dhcpcd.pid
    
        busybox ifconfig wlan0 up

        cd $AP_PATH
        sleep 2

        if [ "$2" = "" ]; then
            ssid="TP-LINK_LIANG"
        else
            ssid=$2
        fi

        if [ "$3" = "" ]; then
            key_mgmt="WPA-PSK"
        else
            key_mgmt=$3
        fi

        if [ "$4" = "" ]; then
            psk="12345678"
        else
            psk=$4
        fi

            cat > /tmp/wpa_supplicant.conf << EOF

            ctrl_interface=/var/run/wpa_supplicant

            network={
                ssid="$ssid"
                key_mgmt=$key_mgmt                                   
                psk="$psk"                                           
            }
EOF
	if [ "$key_mgmt" = "NONE" ] && [ -n $4 ]; then

	    if [ "$5" = "" ]; then
		idx=0
	    else
		idx=$5
	    fi

	    if [ ${#psk} -eq 5 ] || [ ${#psk} -eq 13 ] || [ ${#psk} -eq 16 ]; then
		cat > /tmp/wpa_supplicant.conf << EOF

		ctrl_interface=/var/run/wpa_supplicant

		network={
		    ssid="$ssid"
		    key_mgmt=$key_mgmt                                   
		    wep_key0="$psk"                               
		    wep_key1="$psk"                               
		    wep_key2="$psk"                               
		    wep_key3="$psk"                               
		    wep_tx_keyidx=$idx
		}
EOF
	    else
		cat > /tmp/wpa_supplicant.conf << EOF

		ctrl_interface=/var/run/wpa_supplicant

		network={
		    ssid="$ssid"
		    key_mgmt=$key_mgmt                                   
		    wep_key0=$psk                               
		    wep_key1=$psk                               
		    wep_key2=$psk                               
		    wep_key3=$psk                               
		    wep_tx_keyidx=$idx
		}
EOF

	    fi
	fi

	if [ "$key_mgmt" = "NONE" ] && [ -z $4 ]; then

	    cat > /tmp/wpa_supplicant.conf << EOF

	    ctrl_interface=/var/run/wpa_supplicant

	    network={
		ssid="$ssid"
		key_mgmt=$key_mgmt                                   
	    }
EOF

	fi

        ${AP_PATH}/wpa_supplicant  -Dnl80211 -iwlan0  -c/tmp/wpa_supplicant.conf & 
        ${AP_PATH}/dhcpcd -t 0 wlan0 &          

        if [ `${AP_PATH}/wpa_cli -iwlan0 status | grep COMPLETED | wc -l` -eq 1 ]; then

            return 0     
        fi

    done
    return 0
fi

if [ ""$1 = "AP" ]; then        

	killall -9 dhcpcd
	killall -9 wpa_supplicant

        killall -9 hostapd
	killall -9 udhcpd

	busybox ifconfig wlan0 down
        rmmod 8189es
        #insmod /ko/bcmdhd.ko "firmware_path=$AP_PATH/firmware/fw_bcm43438a0_apsta.bin nvram_path=$AP_PATH/firmware/nvram_ap6212.txt iface_name=wlan0"
        insmod /mnt1/ko/8189es.ko
    		sleep 3
        echo 1 > /sys/class/rfkill/rfkill1/state
        cd $AP_PATH

        busybox ifconfig wlan0 up

	rm -f /tmp/hostapd.conf

	#cp /etc/hostapd.conf  /tmp/hostapd.conf

	if [ "$2" = "" ]; then
	    ssid="INFOTM_IPC_"`busybox ifconfig |grep wlan0| awk '{print $5}'| awk -F':' '{print $4 $5 $6}'`
	else
	    ssid=$2
	fi

	if [ "$3" = "" ]; then
	    key_mgmt=WPA-PSK
	else
	    key_mgmt=$3
	fi

	if [ "$4" = "" ]; then
	    psk=admin888
	else
	    psk=$4
	fi

        cat > /tmp/hostapd.conf << EOF
interface=wlan0
driver=nl80211
ctrl_interface=/var/run/hostapd
ctrl_interface_group=0
hw_mode=g
channel=1
wps_state=2
ssid=$ssid
wpa=2
wpa_key_mgmt=$key_mgmt
wpa_pairwise=CCMP
wpa_passphrase=$psk
auth_algs=1
EOF
	sleep 3
	${AP_PATH}/hostapd /tmp/hostapd.conf &

	${AP_PATH}/hostapd_cli wps_config $ssid $key_mgmt CCMP $psk

	busybox route add -net 10.42.0.0 netmask 255.255.255.0 dev wlan0
	busybox ifconfig wlan0 10.42.0.1 netmask 255.255.255.0 up
	echo 1 > /proc/sys/net/ipv4/ip_forward

	touch /var/lib/misc/udhcpd.leases

	mkdir -p /var/run/dhcp-server

	${AP_PATH}/udhcpd  $AP_PATH/udhcpd.conf wlan0 &
	return 0
fi
