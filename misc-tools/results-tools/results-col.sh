#!/bin/bash

# get the algorith names from results/ident-algo-app.txt
ls results/*ident-*.txt | cut -d- -f2 | sort | uniq | while read algo; do

  OUTFILE="results/$algo-excel.txt"
  echo $OUTFILE

  # browse thru results/ident-algo..
  echo -e "$algo\t, size \t, time \t\t, it \t, cand" > $OUTFILE
  ls results/*ident-$algo* | cut -d- -f3 | cut -d. -f1 | while read app; do
    echo -e "\n\n $app" >> $OUTFILE
    grep [0-9] results/ident-$algo-$app.txt | grep "," | while read line; do
      echo -e "\t, $line  " >> $OUTFILE
    done
  done
done
