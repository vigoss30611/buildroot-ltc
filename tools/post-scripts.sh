#!/bin/bash

test -d tools || (echo "Wrong path `pwd`"; exit 0)

image_add="\
	output/build/linux-local/vmlinux \
	"

initrd_add= \

install_add() {
if [ -n "$1" ]; then
	for i in $1; do
		echo "install $i to $2"
		cp -rf $i $2
	done
fi

}

make_ius() {
	version=$(cat output/product/system/root/version 2>/dev/null |  awk '{print $0}')
	if [ "$version" =  "" ]; then
		output/host/usr/bin/iuw mkius output/product/$1.ixl -o output/images/$1.ius
	else
		output/host/usr/bin/iuw mkius output/product/$1.ixl -s $version -o output/images/$1.ius
	fi
}
make_burn() {
	output/host/usr/bin/iuw mkburn output/images -o output/images/spi_burn -p 16 
}

install_add "${image_add}" output/images
tools/post-items.sh
install_add "${initrd_add}" output/initrd
make_ius ota
make_ius burn
make_burn

