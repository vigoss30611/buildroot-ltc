#!/bin/sh -xe
sudo ln -sf $PWD/raspi-blacklist.conf /etc/modprobe.d/raspi-blacklist.conf
sudo ln -sf $PWD/modules /etc/modules
sudo ln -sf $PWD/../program-fpga /usr/bin/
sudo ln -sf $PWD/../lcd-message /usr/bin/
sudo ln -sf $PWD/../asic /usr/bin/
sudo ln -sf $PWD/../waas/waas /usr/bin/
sudo ln -sf $PWD/../knc-serial /usr/bin
sudo ln -sf $PWD/../knc-led /usr/bin
sudo ln -sf $PWD/../io-pwr /usr/bin
sudo ln -sf $PWD/../../spi-test/spi-test /usr/bin/
sudo ln -sf $PWD/board-init.sh /usr/bin/
sudo ln -sf $PWD/factory_setup /usr/bin/
sudo ln -sf $PWD/default-config.sh /etc/init.d/
sudo ln -sf $PWD/ioboard.sh /etc/init.d/
sudo ln -sf $PWD/factory-setup.sh /etc/init.d/
sudo ln -sf $PWD/bfgminer.sh /etc/init.d/
sudo ln -sf $PWD/repartition.sh /etc/init.d/
sudo ln -sf $PWD/lcd-loop.sh /etc/init.d/
sudo ln -sf $PWD/lcd-loop /usr/bin/
sudo ln -sf $PWD/get_asic_stats.awk /usr/bin/
sudo ln -sf $PWD/monitordcdc.ericsson /sbin/
sudo ln -sf $PWD/../spimux-titan.rbf /etc/spimux.rbf
sudo ln -sf $PWD/i2c-loop /usr/bin/
sudo ln -sf $PWD/lcd-print_stopping.sh /etc/init.d/lcd-print_stopping.sh
sudo ln -sf $PWD/lcd-print_rebooting.sh /etc/init.d/lcd-print_rebooting.sh
sudo ln -sf $PWD/asic_data_cache.awk /usr/bin/

sudo cp inittab /etc/
sudo cp $PWD/monitordcdc.logrotate /etc/logrotate.d/monitordcdc

sudo update-rc.d default-config.sh start 11 S .
sudo update-rc.d ioboard.sh defaults
sudo update-rc.d factory-setup.sh defaults
sudo update-rc.d bfgminer.sh defaults
sudo update-rc.d repartition.sh start 11 S .
sudo update-rc.d lcd-loop.sh defaults
sudo update-rc.d lcd-print_stopping.sh stop 02 0
sudo update-rc.d lcd-print_rebooting.sh stop 02 6
