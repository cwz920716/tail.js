#! /bin/sh

cd node-v4.2.1
make clean
cd ..

cd instrument-v8/node-v4.2.1
make clean
cd ../..

cd redis-server
rm -f *.txt *.cdf *.pdf *.plt *.dot *.csv
cd ..

cd nodejs-mud
rm -f *.txt *.cdf *.pdf *.plt *.dot *.csv
cd ..

cd nodejs-todo
rm -f *.txt *.cdf *.pdf *.plt *.dot *.csv
cd ..

cd lighter
rm -f *.txt *.cdf *.pdf *.plt *.dot *.csv
cd ..

cd lets-chat
rm -f *.txt *.cdf *.pdf *.plt *.dot *.csv
cd ..

cd word-finder
rm -f *.txt *.cdf *.pdf *.plt *.dot *.csv
cd ..

cd etherpad-lite
rm -f *.txt *.cdf *.pdf *.plt *.dot *.csv
cd ..

cd tools
rm -f *.txt *.cdf *.pdf *.plt *.dot *.csv
cd ..

for file in `find . -type f -name '*~'`
do
  echo $file
  rm $file
done
