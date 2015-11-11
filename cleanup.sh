#! /bin/sh

cd node-v4.2.1
make clean
cd ..

cd redis-server
sudo rm -f *.txt
cd ..

cd tools
sudo rm -f *.txt *.cdf *.pdf *.plt
cd ..

for file in `find . -type f -name '*~'`
do
  echo $file
  rm $file
done
