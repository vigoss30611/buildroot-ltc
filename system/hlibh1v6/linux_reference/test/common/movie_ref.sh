#!/bin/bash

# Get systag and movieinput from commonconfig
if [ ! -e commonconfig.sh ]; then
    echo "commonconfig.sh not found!"
    exit -1
else
    .  ./commonconfig.sh
fi

runtime=`date +%d%m%y | sed -e 's/ //'`

# Setup dir in tmp
BASEDIR=$PWD
TMPDIR=/tmp/h1_movie_$runtime

echo "Execute in $TMPDIR"
mkdir $TMPDIR
cd $TMPDIR
cp $BASEDIR/vx170dec_pclinux_multifileoutput .
cp $BASEDIR/movie_encode.sh .
cp $BASEDIR/movie_plot.sh .
cp $BASEDIR/movie_h264.gnu .
cp $BASEDIR/movie_vp8.gnu .

# Get sources and compile system model binaries using tag
(
    git clone /auto/hantro-projects/gforce/GIT/h1_encoder
    cd h1_encoder
    if [ `git tag | grep -c $systag` == "0" ]
    then
            echo "System model tag $systag is not correct!"
            exit
    fi
    git checkout -b branch_$systag $systag
    cd software/linux_reference/test/h264
    make system_multifile
    cp h264_testenc_multifile $TMPDIR
    cd ../vp8
    make system_multifile
    cp vp8_testenc_multifile $TMPDIR 
)

checkfile() {
    if [ ! -e $1 ]; then
        echo "Can't find $1"
        exit
    fi
}

# Get input files from lab network
scpcmd="scp "
for i in 0 1;
do
    if [ ! -e ${movieinput[$i]##*/} ]; then
        echo "Copy ${movieinput[$i]}..."
        scpcmd+="hlabws5:${movieinput[$i]} "
    fi
done
scpcmd+="."

echo $scpcmd
$scpcmd

# Check files
for i in 0 1;
do
    checkfile ${movieinput[$i]##*/}
    checkfile vx170dec_pclinux_multifileoutput
    checkfile movie_encode.sh
    checkfile h264_testenc_multifile
    checkfile vp8_testenc_multifile
done

# Run the movie cases
for i in 0 1 2 3;
do
    let "j=i/2" # Input file index

    if [ ! -e movie_${i}_sys.log ]; then
        echo "Start movie test $i, input ${movieinput[$j]##*/}, encoder log in movie_${i}_sys.log"
        # Start movie test decoder
        ./vx170dec_pclinux_multifileoutput -P -L -Oout.yuv ${movieinput[$j]##*/} > movie_${i}_dec.log &

        # Start movie test encoder
        sleep 100
        ./movie_encode.sh $i sys

        # Wait for decoder to finish
        wait
    fi
done

./movie_plot.sh

