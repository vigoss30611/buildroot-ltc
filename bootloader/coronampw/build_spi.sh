#!/bin/sh
echo "========make spiflash xboot========"
make clean
make -j24 MODE=SIMPLE ERASE_SPIFLASH=n
#make -j24 MODE=SIMPLE
