#!/bin/sh
echo "========make emmc xboot========"
make clean
make -j24 ERASE_SPIFLASH=y
