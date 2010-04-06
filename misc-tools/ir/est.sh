#!/bin/bash

if [ $# == 0 ] ; then
  echo "usage: $0 file.bc"
  exit 1
fi

opt -load ../../build/ise/lib/ISEPass/libLLVMISEPass.so -ise -ise-runtime-estimation $1
