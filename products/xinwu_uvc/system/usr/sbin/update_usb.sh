#!/bin/sh

echo "Going to get image from http://172.3.0.1/burn/burn.ius"

cd /dev/shm

while [ 1 ]
do
wget http://172.3.0.1/burn/burn.ius -T 2
if [ -f "./burn.ius" ];then
    break;
fi
echo "Get image failed, retry"
sleep 1
done

echo "Got image, just burn to spi"

cd /
/usr/sbin/update_arm.sh
