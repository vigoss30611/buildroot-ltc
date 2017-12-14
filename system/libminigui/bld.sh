export PATH=$PATH:${PWD}/../../../prebuilts/linaro-arm-linux-gnueabihf-4.7/bin
export arm_prefix=arm-linux-gnueabihf-

#./configure  --prefix=$PWD/_install  --build=i386-linux  --host=arm-linux  --target=arm-linux --with-ttfsupport=ft2 --with-ft2-includes=../libfreetype/include/freetype2/ 

make -j9


cp ./src/.libs/libminigui_ths.so  ../libra/libminigui_ths-3.0.so.12
$PWD/../../../prebuilts/uclibc/./arm-buildroot-linux-uclibcgnueabihf/bin/strip ../libra/libminigui_ths-3.0.so.12

