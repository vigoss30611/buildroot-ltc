#!/bin/sh

# gpio 24 = PWR_EN
echo 24 > /sys/class/gpio/export
# gpio 23 = DC/DC reset
echo 23 > /sys/class/gpio/export
echo high > /sys/class/gpio/gpio23/direction
# gpio 18 = red LED
echo 18 > /sys/class/gpio/export
# gpio 26 = CONF_DONE
echo 26 > /sys/class/gpio/export
echo in > /sys/class/gpio/gpio26/direction 
# gpio 13 = nCONFIG
echo 13 > /sys/class/gpio/export
echo high > /sys/class/gpio/gpio13/direction
# gpio 21 = nSTATUS
echo 21 > /sys/class/gpio/export
echo in > /sys/class/gpio/gpio21/direction
# gpio 17  = LED1
echo 17 > /sys/class/gpio/export
# gpio 27 = LED2
echo 27 > /sys/class/gpio/export
# gpio 22 = LED3
echo 22 > /sys/class/gpio/export
# gpio 5 = LED4
echo 5 > /sys/class/gpio/export
# gpio 6 = LED5
echo 6 > /sys/class/gpio/export
# gpio 20 = LED6
echo 20 > /sys/class/gpio/export
# gpio 12 = PB_IN
echo 12 > /sys/class/gpio/export
echo in > /sys/class/gpio/gpio12/direction
RPi_gpio_pud 12 up
# gpio 25 = DCDC_SCL
echo 25 > /sys/class/gpio/export
echo in > /sys/class/gpio/gpio25/direction
# gpio 16 = DCDC_SDA
echo 16 > /sys/class/gpio/export
echo in > /sys/class/gpio/gpio16/direction

# Turn ON red LED, turn ON LED-1, turn OFF LEDs(2,3,4,5,6)
echo low > /sys/class/gpio/gpio18/direction
echo low > /sys/class/gpio/gpio17/direction
echo high > /sys/class/gpio/gpio27/direction
echo high > /sys/class/gpio/gpio22/direction
echo high > /sys/class/gpio/gpio5/direction
echo high > /sys/class/gpio/gpio6/direction
echo high > /sys/class/gpio/gpio20/direction

chmod a+rw /sys/class/gpio/*/direction
chmod a+rw /sys/class/gpio/*/value

cd ..

# Enable IO-board power
io-pwr init
if [ "$?" = "0" ]; then
  # Turn ON LED-2
  echo low > /sys/class/gpio/gpio27/direction
fi

# Program FPGA
program-fpga /etc/spimux.rbf
if [ "$?" = "0" ]; then
  # Turn ON LED-3
  echo low > /sys/class/gpio/gpio22/direction
fi

knc-led 0 1 0

# Run waas
if [ -f /config/advanced.conf ]; then
	waas
else
	waas -d -o /config/advanced.conf
fi

lcd-message --init
