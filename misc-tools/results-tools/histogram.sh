#!/bin/bash

grep cand_ *log | awk '{print $6" , " $3 " " $(NF-1) " " $NF}' | sort -n > tmp.hist
last=`tail -1 tmp.hist | awk '{print $1}'`

COUNTER=0
while [  $COUNTER -le $last ]; do
  cnt=`grep "$COUNTER ," tmp.hist | wc -l`
  nodes=`grep "$COUNTER ,"  tmp.hist | awk '{sum+=$3} END {print sum}'`
  sel=`grep "$COUNTER ,"  tmp.hist | awk '$4 !~  "not"  {sum+=1} END {print sum}'`
  proc=$(echo "scale=2; $sel / $cnt * 100 ;" | bc)
  printf "There are %-5d candidates [%6d nodes ; %4d selected = %.1f%% ]  which have the input size of %-3d parameters.\n" $cnt $nodes $sel $proc $COUNTER
  let COUNTER=COUNTER+1 
done

rm -f tmp.hist
