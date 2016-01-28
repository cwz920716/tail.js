#! /bin/sh

bench=nodejs-todo
cli=cwz@146.6.53.187
node_bin_path=../instrument-v8/node-v4.2.1/node

f=0

$node_bin_path server.js 146.6.53.156 50000 &
sleep 10
ssh -p 2002 $cli '~/dev-tools/wrk2/wrk -R1500 -t150 -c150 -d90s http://146.6.53.156:50000/todos'
pkill -9 node
sleep 20
mv /tmp/logs.txt ./logs-$f.txt
mv /tmp/edges.dot ./edges-$f.dot
# dot -Tpdf ./edges-$f.dot -o todo.pdf
mv /tmp/loops.txt ./loops-$f.txt
cd ../tools
python plot_cdf.py ../$bench/logs-$f.txt 0 'time(ns)' '' > /dev/null
python plot_cdf.py ../$bench/logs-$f.txt 1 'time(ns)' 'non-IO' > /dev/null
python plot_cdf.py ../$bench/logs-$f.txt 2 'time(ns)' 'IO' > /dev/null
python plot_cdf.py ../$bench/logs-$f.txt 3 'time(ns)' 'ratio' > /dev/null
python plot_cdf.py ../$bench/logs-$f.txt 4 'time(ns)' 'fraction' > /dev/null
python plot_cdf.py ../$bench/logs-$f.txt 5 'time(ns)' 'compute' > /dev/null
# python plot_cdf.py ../$bench/loops-$f.txt 0 'events per round' 'none' > /dev/null
cd ../$bench
