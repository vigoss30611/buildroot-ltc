#!/bin/sh -xe
mount /dev/mmcblk0p1 /boot
if [ ! -f /boot/uEnv.txt.orig ]; then
	cp -p /boot/uEnv.txt /boot/uEnv.txt.orig
fi
echo "optargs=capemgr.disable_partno=BB-BONELT-HDMI,BB-BONELT-HDMIN" >>/boot/uEnv.txt
umount /boot
cp BB-* /lib/firmware/
ln -sf $PWD/../rc.local /etc/
sudo ln -sf $PWD/lcd-ifupdown.sh /etc/network/if-up.d/lcd
sudo ln -sf $PWD/lcd-ifupdown.sh /etc/network/if-post-down.d/lcd
sudo ln -sf $PWD/../lcd-message /usr/bin/
sudo ln -sf $PWD/../asic /usr/bin/
sudo ln -sf $PWD/../../spi-test/spi-test /usr/bin/
sudo ln -sf $PWD/../waas/waas /usr/bin/
sudo ln -sf $PWD/../knc-serial /usr/bin
sudo ln -sf $PWD/../knc-led /usr/bin
sudo mkdir -p /config
#cp rc-local.service /etc/systemd/system/
#systemctl enable rc-local
