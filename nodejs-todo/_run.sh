#! /bin/sh

bench=nodejs-todo
cli=cwz@146.6.53.187
node_bin_path=node

f=0

$node_bin_path server.js 146.6.53.156 50000 &
sleep 10
ssh -p 2002 $cli '~/dev-tools/wrk2/wrk -R4800 -t240 -c240 -d90s http://146.6.53.156:50000/todos'
pkill -9 node
sleep 20
