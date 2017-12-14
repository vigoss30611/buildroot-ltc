# First copy a movie file, for example:
# scp proto3@172.28.107.124:/mnt/lacie/decoder_stability/h264/baseline/24_season5_1280x720_5400kbps.h264 .
# scp proto3@172.28.107.124:/mnt/lacie/decoder_stability/h264/baseline/movie_trailers_1920x1080_10000kbps.h264
./hx170dec_pclinux_multifileoutput -P -L $1
