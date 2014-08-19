#!/bin/bash
make all

rm -f mproc.csv
# initialize mproc csv
touch mproc.csv

echo -n ",1" >> mproc.csv
# insert 1-30 labels
for i in `seq 5 5 30`;
do
	echo -n ",$i" >> mproc.csv
done
echo "" >> mproc.csv

MAX_PRIME=4294967295

CURRENT=500000000
INCREMENT=500000000

while [ $CURRENT -lt $MAX_PRIME ]; do
	echo -n "$CURRENT" >> mproc.csv
	OUT=$( { /usr/bin/time -f "%e" ./prime_mproc -m $CURRENT -c 1 -q; } 2>&1 )
	echo -n ",$OUT" >> mproc.csv
	for i in `seq 5 5 30`; do
		OUT=$( { /usr/bin/time -f "%e" ./prime_mproc -m $CURRENT -c $i -q; } 2>&1 )
		echo -n ",$OUT" >> mproc.csv
	done
	let CURRENT+=$INCREMENT
	echo "" >> mproc.csv
done

echo -n "$MAX_PRIME" >> mproc.csv
OUT=$( { /usr/bin/time -f "%e" ./prime_mproc -m $MAX_PRIME -c 1 -q; } 2>&1 )
echo -n ",$OUT" >> mproc.csv
for i in `seq 5 5 30`; do
	OUT=$( { /usr/bin/time -f "%e" ./prime_mproc -m $MAX_PRIME -c $i -q; } 2>&1 )
	echo -n ",$OUT" >> mproc.csv
done
echo "" >> mproc.csv


# Repeat for pthreads

rm -f mthread.csv
# initialize mproc csv
touch pthread.csv

echo -n ",1" >> pthread.csv
# insert 1-30 labels
for i in `seq 5 5 30`;
do
	echo -n ",$i" >> pthread.csv
done
echo "" >> pthread.csv

CURRENT=500000000

while [ $CURRENT -lt $MAX_PRIME ]; do
	echo -n "$CURRENT" >> pthread.csv
	OUT=$( { /usr/bin/time -f "%e" ./prime_pthread -m $CURRENT -c 1 -q; } 2>&1 )
	echo -n ",$OUT" >> pthread.csv
	for i in `seq 5 5 30`; do
		OUT=$( { /usr/bin/time -f "%e" ./prime_pthread -m $CURRENT -c $i -q; } 2>&1 )
		echo -n ",$OUT" >> pthread.csv
	done
	let CURRENT+=$INCREMENT
	echo "" >> pthread.csv
done

echo -n "$MAX_PRIME" >> pthread.csv
OUT=$( { /usr/bin/time -f "%e" ./prime_pthread -m $MAX_PRIME -c 1 -q; } 2>&1 )
echo -n ",$OUT" >> pthread.csv
for i in `seq 5 5 30`; do
	OUT=$( { /usr/bin/time -f "%e" ./prime_pthread -m $MAX_PRIME -c $i -q; } 2>&1 )
	echo -n ",$OUT" >> pthread.csv
done
echo "" >> pthread.csv
