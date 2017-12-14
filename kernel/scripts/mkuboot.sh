#!/bin/bash

#
# Build U-Boot image when `mkimage' tool is available.
#

MKIMAGE=$(type -path "${CROSS_COMPILE}mkimage")
echo ${MKIMAGE}
if [ -z "${MKIMAGE}" ]; then
	MKIMAGE=$(type -path mkimage)
	echo ${MKIMAGE}
	if [ -z "${MKIMAGE}" ]; then
        MKIMAGE=$(pwd | awk  -F'/linux' '{print $1}')/../../prebuilts/uclibc/bin/mkimage
	echo ${MKIMAGE}
	if [ -z "${MKIMAGE}" ]; then
            # Doesn't exist
            echo '"mkimage" command not found - U-Boot images will not be built' >&2
            exit 1;
        fi
	fi
fi
#echo $(pwd)
#MKIMAGE=./scripts/mkimage
# Call "mkimage" to create U-Boot image
${MKIMAGE} "$@"
