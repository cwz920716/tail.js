#! /bin/sh

cd node-v4.2.1
make clean
cd ..

cd instrument-v8/node-v4.2.1
make clean
cd ../..

cd redis-server
sudo rm -f *.txt *.cdf *.pdf *.plt *.dot
cd ..

cd nodejs-mud
sudo rm -f *.txt *.cdf *.pdf *.plt *.dot
cd ..

cd nodejs-todo
sudo rm -f *.txt *.cdf *.pdf *.plt *.dot
cd ..

cd lighter
sudo rm -f *.txt *.cdf *.pdf *.plt *.dot
cd ..

cd lets-chat
sudo rm -f *.txt *.cdf *.pdf *.plt *.dot
cd ..

cd word-finder
sudo rm -f *.txt *.cdf *.pdf *.plt *.dot
cd ..

cd tools
sudo rm -f *.txt *.cdf *.pdf *.plt *.dot
cd ..

for file in `find . -type f -name '*~'`
do
  echo $file
  rm $file
done
