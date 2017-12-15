KnCMiner ASIC helper code
====

This is a library of helper code and system support for using KnC ASICs, with support for both Jupiter and Neptune generation of ASICs.

Installation
===

Recommended environment for development is a minimal Debian system. Then log in and execute

<pre>
sudo apt-get update
sudo apt-get upgrade
sudo apt-get install build-essentials
sudo apt-get install autoconf automake libz-dev libcurl4-openssl-dev ncurses-dev libltdl-dev libtool
git clone git@github.com:KnCMiner/spi-test
cd spi-test
make
cd ..
git clone git@github.com:KnCMiner/knc-asic
cd knc-asic
make
cd system
sudo ./install.sh
</pre>

Now reboot the board to enable Beaglebone access to the KnCMiner I/O board

cgminer build instructions
===

<pre>
git clone -b knc2 git@github.com:KnCMiner/cgminer
cd cgminer
NOCONFIGURE=1 ./autogen.sh
./configure --enable-knc
make -j2
</pre>


Code library
===
knc-asic.[ch] is the main ASIC support functions

knc-transport.[ch] is a slight abstraction of the SPI transport, to ease use somewhat and allow for other transports to be developed without changing the asic support code.

asic reference command
===

asic.c is a small reference command allowing you to communicate directly with the ASIC.

<pre>
Usage: ./asic command arguments..
  status channel
        ASIC status
  led red green blue
        Controller LED color
  reset 
        Reset controller
  info channel die 
        ASIC version & info
  setwork channel die core slot(1-15) clean(0/1) midstate data
        Set work vector
  report channel die core
        Get nonce report
  halt channel die core
        Halt core
  freq channel die frequency
        Set core frequency
  raw channel die response_length request_data
        Send raw ASIC request
</pre>

system support commands & files
===

knc-serial.c reads serial numbers of all devices in a KnCMiner device

<pre>
Usage: ./knc-serial [-v] [command,..]
  -b --beaglebone  Beaglebone
  -i --ioboard     I/O Board
  -a --asic=N      ASIC card
</pre>

knc-led.c controls the I/O board RGB led. Each colour can be set to brighness level 0-15 where 0 is off and 15 is very very bright. Warning: don't look at the led at brightest level, might hurt your eyes.

<pre>
Usage: ./knc-led red green blue
</pre>


io-pwr.c controls I/O board power regulator. Needs to be initialized before the controller FPGA can be programmed.

<pre>
Usage: ./io-pwr [-v] [command,..]
  -v --verbose  Verbose operation
  init          Initialize I/O power
</pre>


program-fpga loads a bitstream image into the controller FPGA. By default spimux.rbf is loaded.  Need to be run as root or the Linux kernel won't notice the new devices available.

<pre>
Usage: ./program_fpga filename.rbf
</pre>

rc.local

Sample rc.local for initializing the system

system/

Debian support files


waas command
===
The waas command is responsible for detecting ASIC boards and starting them. Currently needs to be run as root.

First time you need to create it's config file by running

<pre>
sudo waas -d -o /config/advanced.conf
</pre>

To activate a new config (or after boot) run waas without any arguments

<pre>
sudo waas
</pre>

Other usage options are available. See below.

<pre>
WAAS = Work-Arounds and Advanced Settings
Command-line options:
    -c <config-file>
            Specify the config file name to read on start. If this
            option is not used, waas uses default config file:
             (see DEFAULT_CONFIG_FILE below)
    -g <info-request>
            Get data from device: voltage outputs, current outputs etc.
            Output is printed in JSON format. Valid info-request values:
        all-asic-info :
            Print outpotu voltage and current for each DC/DC. Print
            temperature for the ASIC board.
    -i <info-request>
            Print requested info and exit. Valid info-request values:
        valid-die-voltages :
            Print list of valid DC/DC voltage offsets
        valid-die-frequencies :
            Print list of valid die PLL frequencies
        valid-ranges :
            Print all of the above in JSON format
    -o <output-file>
            Specify the name of the file where to print running config info.
            If this option is not used, waas prints running config to the
            standard output.
    -d
            Do not read config file, use default values instead
    -r
            Read only. Do not read config file, do not change running values;
            waas will only print running config end exit
    -a      Auto-adjust PLL frequency based on the DC/DC temperatures.
            Works only for Ericsson devices.
</pre>
