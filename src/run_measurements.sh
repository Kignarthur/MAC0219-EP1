#! /bin/bash

set -o xtrace

MEASUREMENTS=10
ITERATIONS=6 #10
INITIAL_SIZE=16
THREADS=32

NAMES=('mandelbrot_seq' 'mandelbrot_pth' 'mandelbrot_omp')

make
mkdir results

for NAME in ${NAMES[@]}; do
    mkdir results/$NAME

    size=$INITIAL_SIZE

    for ((i=1; i<=$ITERATIONS; i++)); do
        if [[ $NAME == ${NAMES[0]} ]]; then
            for alloc in 0 1; do
                perf stat -r $MEASUREMENTS ./$NAME -2.5 1.5 -2.0 2.0 $size $alloc >> full.log 2>&1
                perf stat -r $MEASUREMENTS ./$NAME -0.8 -0.7 0.05 0.15 $size $alloc >> seahorse.log 2>&1
                perf stat -r $MEASUREMENTS ./$NAME 0.175 0.375 -0.1 0.1 $size $alloc >> elephant.log 2>&1
                perf stat -r $MEASUREMENTS ./$NAME -0.188 -0.012 0.554 0.754 $size $alloc >> triple_spiral.log 2>&1
            done
        else
            for ((threads=1; threads<=$THREADS; threads*=2)); do
                perf stat -r $MEASUREMENTS ./$NAME -2.5 1.5 -2.0 2.0 $size $threads >> full.log 2>&1
                perf stat -r $MEASUREMENTS ./$NAME -0.8 -0.7 0.05 0.15 $size $threads >> seahorse.log 2>&1
                perf stat -r $MEASUREMENTS ./$NAME 0.175 0.375 -0.1 0.1 $size $threads >> elephant.log 2>&1
                perf stat -r $MEASUREMENTS ./$NAME -0.188 -0.012 0.554 0.754 $size $threads >> triple_spiral.log 2>&1
            done
        fi
        size=$(($size * 2))
    done

    mv *.log results/$NAME
    [ -e output.ppm ] && rm output.ppm
done

python3 process_log.py
