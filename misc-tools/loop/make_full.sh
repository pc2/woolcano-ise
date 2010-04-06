#!/bin/bash

if [ $# == 0 ] ; then
  echo "usage: $0 dir"
  exit
fi
dir="${1/\//}"

function run() {
  ./profile_all.pl $dir 

  rm -f ${dir}/${dir}_pass.bc
  opt $opt_p -stats ${dir}/${dir}.bc -f -o ${dir}/${dir}_pass.bc  
  mv ${dir}/${dir}_pass.bc ${dir}/${dir}.bc

  opt \
    $opt_o \
    -load ../build/ise/lib/ISEPass/libLLVMISEPass.so \
    -ise \
    -ise-runtime-estimation \
    -ise-algorithm=maxmiso \
    -ise-architecture=virtex \
    -ise-profile-info-file=${dir}/${dir}.prof \
    -f -o ${dir}/${dir}_ise.bc \
    ${dir}/${dir}.bc 

}

for gcc_o in -O3 -O2 -O1 ; do
  for gcc_p in "" "-funroll-loops" ; do
    for opt_o in "" -O3 -O2 -O1 ; do
      for opt_p in "" "-loop-unroll" "-mem2reg"; do
        echo "CC = llvm-gcc" > Makefile.common
        echo "CCOPTS = ${gcc_o} ${gcc_p} -emit-llvm" >> Makefile.common
        echo "" && echo ""
        echo "gcc = ${gcc_o} ${gcc_p} -emit-llvm"
        echo "opt = ${opt_o} ${opt_p}"
        run 
      done
    done
  done
done 

# echo "CCOPTS = -O3 -fforce-mem -fforce-addr -funroll-all-loops -emit-llvm" > Makefile.common
# awk '{if ($1 ~ /ratio/) print $0}' ../${dir}_ise.log > ../${dir}_ise_ratio.log

