
reset
set out "movie.pdf"
set terminal pdf fsize 3
set grid

# 
unset key
unset title
unset xlabel
unset ylabel
unset ytics
unset yrange

set ylabel "qp"
set ytics 1
set key bottom
set xlabel "frame"
set xtics auto

set title "H.264 movie qp/frame"
plot \
"movie" using 3:5 w l axes x1y1 title "qp/frame"

unset key
unset title
unset xlabel
unset ylabel
unset ytics
unset yrange

# 
set ylabel "PSNR"
set key bottom
set xlabel "frame"
set ytics auto

set title "H.264 movie PSNR/frame"
plot \
"movie" using 3:15 w l axes x1y1 title "PSNR/frame"

# 
set ylabel "bits"
set key bottom
set xlabel "frame"

set title "H.264 movie bitrate"
plot \
"movie" using 3:9 w l axes x1y1 title "stream bitrate", \
"movie" using 3:10 w l axes x1y1 title "moving average bitrate (1sec)"

# 
set ylabel "bytes"
set key bottom
set xlabel "frame"
set ytics auto

set title "H.264 movie bytes/frame"
plot \
"movie" using 3:13 w l axes x1y1 title "bytes/frame"

## 
unset key
unset title
unset xlabel
unset ylabel
unset ytics
unset yrange


