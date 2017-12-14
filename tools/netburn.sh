#!/bin/bash

# your IMAGE path
upgrade_tool=output/build/qlibupgrade-1.0.0/upgrade
image_path=output/images
local_addr=`ip addr | grep inet | grep eth | cut -f 6 -d ' ' | cut -f 1 -d '/'`
local_port=8080

if ! test -f ${upgrade_tool}; then
	echo "Cannot find upgrade tool, make sure **qlibupgrade** is compiled!"
	exit
fi

if test -z "`whereis python 2>/dev/null`"; then
	echo "Python is not installed!"
	exit
fi

cp -f tools/update_arm.sh  ${image_path}/update
cp -f ${upgrade_tool} ${image_path}
sed -i "s/__SERVER_IP__/${local_addr}/g" ${image_path}/update
sed -i "s/__SERVER_PORT__/${local_port}/g" ${image_path}/update
echo -e "\nPaste the commands below to device prompt to update:\n"
echo -e "  curl http://${local_addr}:${local_port}/update | sh\n"
echo -e "After updating, Ctrl-C to exit..."

cd ${image_path}
python -m SimpleHTTPServer ${local_port}

