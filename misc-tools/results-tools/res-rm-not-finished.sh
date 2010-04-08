#!/bin/bash

if [ $# == 0 ]; then
  echo usage: $0 res_dir
  exit
fi

find $1 -type f -name \*.txt | while read file; do
  if [ ! -s  $file ] ; then
    echo -n "empty file: "
    mv -v $file $file.bak
  else
   grep , $file  | awk '{ if ($1 ~ /[0-9]/  && $2 == "," ) print }' | egrep -q ","
   if [ ! $? == 0 ] ; then
      echo -e "no results: "
      mv -v $file $file.bak
   fi;
  fi;
done;
