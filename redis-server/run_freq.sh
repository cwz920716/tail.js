#! /bin/sh

pkill node

freqs="3200000 2200000 1200000 0"
for f in $freqs
do
  export NODE_CPU0_FREQ=$f
  ../node-v4.2.1/node server.js 146.6.53.156 server &
  sleep 10
  ssh -p 2002 cwz@10.145.121.128 '~/Eve/wrk2/wrk -R1000 -t100 -c400 -d70s http://146.6.53.156:50000 > /dev/null'
  mv /tmp/logs.txt ./logs-$f.txt
  mv /tmp/loops.txt ./loops-$f.txt
  pkill node
  sleep 80
  cd ../tools
  python plot_cdf.py ../redis-server/logs-$f.txt 0 'time(ns)' > /dev/null
  python plot_cdf.py ../redis-server/loops-$f.txt 0 'eveents per round' > /dev/null
  cd ../redis-server
done
