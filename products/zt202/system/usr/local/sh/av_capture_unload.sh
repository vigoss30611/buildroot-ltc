#!/bin/sh

killall -KILL av_server.out
killall wis-streamer

./csl_unload.sh
./drv_unload.sh

cd ../dvsdk/dm365

rmmod cmemk 
rmmod irqk 
rmmod edmak 

rm -f /dev/dm365mmap
rmmod dm365mmap.ko

cd -


