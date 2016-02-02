#! /bin/sh

bench=lets-chat
cli=cwz@146.6.53.154
node_bin_path=../instrument-v8/node-v4.2.1/node

f=0

$node_bin_path app.js 146.6.53.156 50000 &
sleep 20
ssh -p 2002 $cli '~/dev-tools/wrk2/wrk -R100 -t25 -c25 -d90s -s ~/dev-tools/wrk2/scripts/chat.lua http://146.6.53.156:50000/'
pkill -9 node
sleep 20
mv /tmp/logs.txt ./logs-$f.txt
mv /tmp/edges.dot ./edges-$f.dot
# dot -Tpdf ./edges-$f.dot -o chat.pdf
# mv /tmp/loops.txt ./loops-$f.txt
cat ./logs-$f.txt | sort -g -k 1 -t"," > ./logs-$f.csv
cd ../tools
python plot_cdf.py ../$bench/logs-$f.txt 0 'time(ns)' '' > /dev/null
python plot_cdf.py ../$bench/logs-$f.txt 1 'time(ns)' 'non-IO' > /dev/null
python plot_cdf.py ../$bench/logs-$f.txt 2 'time(ns)' 'IO' > /dev/null
python plot_cdf.py ../$bench/logs-$f.txt 3 'time(ns)' 'ratio' > /dev/null
python plot_cdf.py ../$bench/logs-$f.txt 4 'time(ns)' 'fraction' > /dev/null
python plot_cdf.py ../$bench/logs-$f.txt 5 'time(ns)' 'compute' > /dev/null
cd ../$bench
mongo letschat --eval "db.dropDatabase();"
npm run migrate
echo 3 | sudo tee /proc/sys/vm/drop_caches
