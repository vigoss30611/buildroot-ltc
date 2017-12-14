#!/bin/bash

env="sys"
if [ $# -gt 1 ]; then
    env=$1
fi

echo "Creating movie pdf files from $env logs..."

export LD_LIBRARY_PATH=${HOME}/lib

for i in 0 1 2 3;
do
    if [ -e movie_${i}_$env.log ];
    then
        grep "|" movie_${i}_$env.log > movie
        if [ $i -eq 0 ] || [ $i -eq 2 ]; then
            gnuplot movie_h264.gnu
        else
            gnuplot movie_vp8.gnu
        fi
        mv movie.pdf movie_${i}.pdf
    fi
done

