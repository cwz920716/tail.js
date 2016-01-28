#! /bin/sh

bench=etherpad-lite
cli=cwz@146.6.53.187
node_bin_path=node

f=0

$node_bin_path node_modules/ep_etherpad-lite/node/server.js 146.6.53.156 50000 &
sleep 20
ssh -p 2002 $cli '~/dev-tools/wrk2/wrk -R200 -t10 -c10 -d90s -s ~/dev-tools/wrk2/scripts/etherpad.lua http://146.6.53.156:50000/'
pkill -9 node
