#!/bin/bash

# your IMAGE path
image_path=output/images
sd_dir=${1}
sd_name=${sd_dir##*/}
check_dir=/sys/block/${sd_name}/removable
sd_size=/sys/block/${sd_name}/size
max_sd_size=64000000
set +x
check_sd_node(){
    if [ ! -f "$check_dir" ];then
        echo "device is not a removable device-> $sd_dev"
        exit 1
    fi
    removable=`cat $check_dir`
    echo "remove status ->$removable"
    if [ z"$removable" != z"1" ];then
        echo "sd node is not a remove device ->$sd_dev ,abort"
        exit 1
    fi
    if [ ! -f "$sd_size" ];then
        echo "device is not a removable device-> $sd_dev"
        exit 1
    fi
    size=`cat $sd_size`
    #echo "sd size ->$size"
    if [ $size -gt $max_sd_size ]; then
        echo "warning !!!!! target sd too large $size ,ctrl+c to abort"
        sleep 2
        echo "warning !!!!! target sd too large $size ,ctrl+c to abort"
        sleep 2
        echo "warning !!!!! target sd too large $size ,ctrl+c to abort"

    fi
}
# check args
if [ z"$#" != z"1" ]
then
    echo -e "Usage :\n\t$0 [absolute BLOCKDEV] "
	echo
    echo -e "example $0 /dev/sdb"
    exit 1
fi

check_sd_node
if [ ! -b "$1" ]; then
	echo "Error: node $1 is not a block device"
	exit 1
fi

if [ ! -f "$check_dir" ];then
	echo -e "Error: dir $check_dir not exit"
	exit 1
fi

value=$(cat ${check_dir})

if [ z"$value" != z"1" ];then
	echo -e "Error: not burn card insert, please insert burn card and try again"
	exit 1
fi

umount ${1}1  > /dev/null  2>&1

echo "install ius..."
sudo dd if=${image_path}/burn.ius of=$1 bs=1M seek=16
sudo sync
echo -e "done\n"

# partition disk
echo "part disk..."
sudo umount $1* > /dev/null 2>&1
sudo fdisk $1 << EOF > /dev/null 2>&1
d
1
d
2
d
3
d
4
n
p
1
262144

w
EOF
echo -e "done\n"

# format disk
echo "format disk..."
sudo mkdosfs -F 32 ${1}1
sudo sync
echo -e "done\n"

