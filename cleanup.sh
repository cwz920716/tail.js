#! /bin/sh

cd node-v4.2.1
make clean

cd ..
for file in `find . -type f -name '*~'`
do
  echo $file
  rm $file
done
