#! /bin/sh

pkill node
cli=cwz@146.6.28.78
cpufreq-set -c 0 -g ondemand

freqs="-1 3200000 1200000"
for f in $freqs
do
  export NODE_CPU0_FREQ=$f
  if [ $f != "-1" ]; then
    cpufreq-set -c 0 -g userspace
  fi
  ../node-v4.2.1/node server.js 146.6.53.156 50000 &
  sleep 10
  ssh -p 2002 $cli '~/Eve/wrk2/wrk -R400 -t100 -c100 -d90s http://146.6.53.156:50000 > /tmp/wrk_out'
  # ssh -p 2002 $cli 'cat /tmp/wrk_out'
  mv /tmp/logs.txt ./logs-$f.txt
  mv /tmp/loops.txt ./loops-$f.txt
  pkill node
  sleep 40
  cd ../tools
  python plot_cdf.py ../redis-server/logs-$f.txt 0 'time(ns)' '' > /dev/null
  python plot_cdf.py ../redis-server/logs-$f.txt 1 'time(ns)' 'non-IO' > /dev/null
  python plot_cdf.py ../redis-server/logs-$f.txt 2 'time(ns)' 'IO' > /dev/null
  python plot_cdf.py ../redis-server/logs-$f.txt 3 'time(ns)' 'ratio' > /dev/null
  python plot_cdf.py ../redis-server/loops-$f.txt 0 'events per round' 'none' > /dev/null
  cd ../redis-server
done

if false
then

flex_freq="0"
threshes="1 5 10"
for thresh in $threshes
do
  f=$flex_freq
  export NODE_CPU0_FREQ=$f
  export NODE_QUEUE_THRESHOLD=$thresh
  ../node-v4.2.1/node server.js 146.6.53.156 50000 &
  sleep 10
  ssh -p 2002 $cli '~/Eve/wrk2/wrk -R400 -t100 -c400 -d90s http://146.6.53.156:50000 > /tmp/wrk_out'
  # ssh -p 2002 $cli 'cat /tmp/wrk_out'
  mv /tmp/logs.txt ./logs-$f-$thresh.txt
  mv /tmp/loops.txt ./loops-$f-$thresh.txt
  pkill node
  sleep 40
  cd ../tools
  python plot_cdf.py ../redis-server/logs-$f-$thresh.txt 1 'time(ns)' > /dev/null
  python plot_cdf.py ../redis-server/loops-$f-$thresh.txt 0 'events per round' > /dev/null
  cd ../redis-server
done

fi
