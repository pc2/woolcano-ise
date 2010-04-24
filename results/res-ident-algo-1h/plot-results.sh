#!/bin/bash

DATADIR="plot-data"
mkdir -p $DATADIR && rm -rf $DATADIR/*

# let's plot algorithms first
	grep -h '^[0-9]' maxmiso/*.txt  | sed 's/,//g' > $DATADIR/maxmiso.dat
	./plot-algo.sh "maxmiso.dat"

# and rest of algos
for i in 2 4 8; do
	for o in 1 2 ; do
		dir="i${i}-o${o}"
		for algo in singlecut union; do
		datfile="$DATADIR/$algo-i${i}-o${o}.dat"
		grep -h '^[0-9]' $dir/ident-$algo*.txt  | sed 's/,//g' > $datfile
		./plot-algo.sh "$datfile"
		done;
	done;
done;


# and now applications
for app in adpcm aes blowfish cjpeg fft sha sor whetstone md5; do
	for i in 2 4 8; do
		for o in 1 2 ; do
			
		done
	done
done
