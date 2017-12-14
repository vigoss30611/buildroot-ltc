#!/bin/sh

echo "Going to get customize shell"

cd /dev/shm
while [ 1 ]
do
wget http://172.3.0.1/burn/customize.sh -T 2
if [ -f "./customize.sh" ]; then
    break
fi
sleep 1
done

echo "Got shell, just excute it"

cd /
chmod +x /dev/shm/customize.sh
/dev/shm/customize.sh
