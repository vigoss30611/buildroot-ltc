#!/bin/bash

make clean
make mrproper
make imap_wwe10b_config
make > mk_log
svn export . __release

OPENSRC="linux.c pic.c wince.c udc.c"
RESVOBJ="font.o func.o graphic.o logo.o nand.o progress.o system.o logo-gzip.o android.o wrapup.o"

#Remove app file
rm -rf __release/oem/*
cd oem
cp ${OPENSRC} ../__release/oem
cd ..
rm __release/include/configs/imap*
cp include/configs/imapx.h __release/include/configs
cp include/configs/imap_dev.h __release/include/configs
cp include/configs/imap7901_wince.h __release/include/configs
cp include/configs/imap7901_linux.h __release/include/configs
cp include/configs/imap_wwe10b.h __release/include/configs
cp include/configs/imap_wwe8b.h __release/include/configs
cp tools/release/mk.main __release/Makefile -f
cp tools/release/mk.oem __release/oem/Makefile -f
mkdir __release/tools/oemlib/oem -p
cd oem
cp ${RESVOBJ} ../__release/tools/oemlib/oem
cd ..
cd __release/tools/oemlib
tar cf oem_pack oem
cd ../../..
rm -rf __release/tools/oemlib/oem
rm -rf __release/tools/release
tar czf release.tgz __release
rm __release -rf

