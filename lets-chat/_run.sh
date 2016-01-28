#! /bin/sh

bench=lets-chat
cli=cwz@146.6.53.187
node_bin_path=node

f=0

$node_bin_path app.js 146.6.53.156 50000 &
sleep 30
ssh -p 2002 $cli '~/dev-tools/wrk2/wrk -R420 -t105 -c100 -d90s -s ~/dev-tools/wrk2/scripts/chat.lua http://146.6.53.156:50000/'
pkill -9 node
sleep 20
mongo letschat --eval "db.dropDatabase();"
npm run migrate
