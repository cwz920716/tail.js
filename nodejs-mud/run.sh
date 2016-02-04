#! /bin/sh

bench=nodejs-mud
cli=wcui@worf.ece.utexas.edu
node_bin_path=../instrument-v8/node-v4.2.1/node

f=0

$node_bin_path server.js 146.6.177.25 50000 &
sleep 10
ssh -p 2002 $cli '. .bash_profile; ~/Eve/wrk2/wrk -R400 -t100 -c100 -d90s -s ~/Eve/wrk2/scripts/test.lua http://146.6.177.25:50000'
pkill -9 node
sleep 10
mv /tmp/logs.txt ./logs-$f.txt
cat ./logs-$f.txt | sort -g -k 1 -t"," > ./logs-$f.csv
scp ./logs-$f.csv trinity@146.6.53.156:wcui/R/Plot1/logs-0.csv
