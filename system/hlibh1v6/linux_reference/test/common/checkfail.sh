#!/bin/bash

. ./f_check.sh

case=case_$1
dir=random_parameter_fails

#diff $dir/${case}.log*

echo gvim -d $dir/${case}.log*
gvim -d $dir/${case}.log* &

if [ -s $dir/${case}.ivf ]; then
    cmp $dir/${case}.ivf*

    res=`grep Init $dir/${case}.log | awk '{print $4}'`
    picWidth=`echo $res | awk -F x '{print $1}'`
    picHeight=`echo $res | awk -F x '{print $2}'`

    echo case dimensions $res

    ./vp8x170dec_pclinux -P $dir/${case}.ivf >> decode.log
    mv out.yuv case.yuv
    ./vp8x170dec_pclinux -P $dir/${case}.ivf.ref >> decode.log
    mv out.yuv case.yuv.ref
    
    findFailingPicture case.yuv case.yuv.ref
    echo

    echo viewyuv -w$picWidth -h$picHeight -c case.yuv case.yuv.ref
    viewyuv -w$picWidth -h$picHeight -c case.yuv case.yuv.ref &
fi

if [ -s $dir/${case}.h264 ]; then
    cmp $dir/${case}.h264*

    res=`grep Init $dir/${case}.log | awk '{print $4}'`
    picWidth=`echo $res | awk -F x '{print $1}'`
    picHeight=`echo $res | awk -F x '{print $2}'`

    echo case dimensions $res

    ./ldecod.exe -o case.yuv -i $dir/${case}.h264 >> decode.log
    ./ldecod.exe -o case.yuv.ref -i $dir/${case}.h264.ref >> decode.log
    
    findFailingPicture case.yuv case.yuv.ref
    echo

    echo viewyuv -w$picWidth -h$picHeight -c case.yuv case.yuv.ref
    viewyuv -w$picWidth -h$picHeight -c case.yuv case.yuv.ref &
fi
