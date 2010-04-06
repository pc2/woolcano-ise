#!/bin/bash

if [ $# == 0 ] ; then
  echo "usage: $0 file.gv"
  exit 1
fi

if [ ! -d dotBB ] ; then
  mkdir dotBB && cd dotBB && ../../dotBB/configure && make
fi


rm -f *.gv
opt -load dotBB/Debug/lib/dotBB.dylib -dot-from-bb -disable-output $1
open *.gv
