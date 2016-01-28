#! /bin/sh

bench=lighter
cli=cwz@146.6.53.187
node_bin_path=node

f=0

$node_bin_path server.js 146.6.53.156 50000 &
sleep 10
# ssh -p 2002 $cli '~/dev-tools/wrk2/wrk -R1000 -t200 -c200 -d90s -s ~/dev-tools/wrk2/scripts/lighter.lua http://146.6.53.156:50000/'
ssh -p 2002 $cli '~/dev-tools/wrk2/wrk -R400 -t50 -c50 -d90s http://146.6.53.156:50000/logo.png'
pkill -9 node
sleep 10
