#! /bin/sh
# Fill logo with rgb data from bmp file
# Will generate logo_rgb.h containing unsigned char logo_rgb[] for use in logo_bmp.c
# Every time, after replacement of "kernel_logo.bmp", please run "fill_logo_rgb" script manually. And then make again.
# Refer to README
echo "[*.bmp] [*.h]"
xxd -i $1 $2
#cat $2 | sed 's/unsigned/const unsigned/' > kernel_logo.h


