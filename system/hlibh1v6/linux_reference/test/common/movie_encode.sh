
usage() {
    echo "Usage: movie_encode.sh [casenumber] [environment]"
    echo "Casenumber:   0       720p 5Mbps H.264"
    echo "              1       720p 5Mbps VP8"
    echo "              2       1080p 8Mbps H.264"
    echo "              3       1080p 8Mbps VP8"
    echo "Environment:  hw      HW on FPGA board for testing"
    echo "              sys     System model on PC for reference"
}

if [ $# -lt 2 ];
then
    usage
    exit
fi

casenum=$1
env=$2
moviefile=movie_${casenum}_$env
log="$moviefile.log"
case "$casenum" in
    0)
    encbin="./h264_testenc_multifile"
    param="-w1280 -h720 -B5000000 -I500 -j24 -K1 --psnr -i out.yuv -o $moviefile.h264"
    ;;

    1)
    encbin="./vp8_testenc_multifile"
    param="-w1280 -h720 -B5000000 -I500 -j24 -b100000 --psnr -i out.yuv -o $moviefile.ivf"
    ;;

    2)
    encbin="./h264_testenc_multifile"
    param="-w1920 -h1088 -B8000000 -I500 -j24 -K1 --psnr -i out.yuv -o $moviefile.h264"
    ;;

    3)
    encbin="./vp8_testenc_multifile"
    param="-w1920 -h1088 -B8000000 -I500 -j24 -b200000 --psnr -i out.yuv -o $moviefile.ivf"
    ;;

    *)
    usage
    exit
    ;;

esac

TEST_DATA_FILES="none"
export TEST_DATA_FILES="none"
touch none

echo "Running $moviefile"
$encbin $param > $log

echo "Create md5sum"
if [ -e $moviefile.h264 ]; then
    md5sum $moviefile.h264 > $moviefile.h264.md5sum
fi
if [ -e $moviefile.ivf ]; then
    md5sum $moviefile.ivf > $moviefile.ivf.md5sum
fi

rm -f EOF

