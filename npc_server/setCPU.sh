#! /bin/sh

for pid in `pgrep node`
do
    taskset -pc 1 $pid
done
