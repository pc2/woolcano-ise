#!/bin/bash 

TITLE=""
EPSFILE=""

if [ $# !=  0 ]; then
	PAT=$*
	TITLE="$1"
	EPSFILE=`echo "$PAT" | sed 's/[\/\*]//g' `
else
	echo "usage: $0 \"file{s}\" to_eps"
	echo -e " file		: single file with data to plot"
	echo -e " files		: pattern mask like *.txt or maxmiso/*.txt"
	echo -e " WARNING	remember to put file in \"\" since this will be used as title"
  echo ""
  echo "Example: $0 \"maxmiso/*.txt\""
	exit 1
fi


DATAFILE=all.dat
echo $PAT
grep -h '^[0-9]' $PAT  | sed 's/,//g' > $DATAFILE

cat<<EOF | gnuplot -geometry 1000x500 -persist - && rm $DATAFILE

# set terminal postscript enhanced color size 1000,500 
# set output '$EPSFILE.eps'

# set size 2,1
# set size ratio square 1,2
set origin 0,0

set title "$TITLE"
set xlabel "DAG size"


set key box
set key top left

set grid y x
# set grid y2 x

set y2tics
set ytics nomirror

set multiplot

	set size 0.5, 1
	set origin 0,  0
	set xrange [0:200]
	set y2label ""
	set ylabel "Time [msec]"
	plot 	"$DATAFILE" using 1:2 title "time" 		with points ps 2, \
		"$DATAFILE" using 1:4 title "candidates_found"	with points ps 2 axes x1y2


	set y2label "Candidates found"
	set ylabel ""
	set size 0.5, 1
	set origin 0.5, 0
	set xrange [200:1800]
	plot 	"$DATAFILE" using 1:2 title "time" 		with points ps 2, \
		"$DATAFILE" using 1:4 title "candidates_found"	with points ps 2 axes x1y2

unset multiplot

quit
