#!/bin/bash

ALGO=maxmiso
ARCH=virtex
ISE_PASS=${PWD}/../build/ise/lib/ISEPass/libLLVMISEPass.so
OPT_STATS="-stats -time-passes -track-memory"

if [ $# == 0 ] ; then
  echo "usage: $0 dir"
  exit
fi
dir="${1/\//}"
save_pwd=$PWD



function run() {
  src_llvm=`llvm-config --src-root`
  profile_script="${src_llvm}/utils/profile.pl"

  cd $dir
  rm -f ${dir}.prof ${dir}_pass.bc
  make clean && make

  # generate profilinf infos
  echo "# running profiling"
  ${profile_script} -stats -block -o $dir.prof $dir.bc
  if [ -e llvmprof.out ] ; then
    mv llvmprof.out $dir.prof
  fi

  # run necessary optimizations
  echo "# running opt with passes"
  opt $opt_p  ${dir}.bc -f -o ${dir}_pass.bc  
  mv ${dir}_pass.bc ${dir}.bc

  # run ISE pass
  echo "# running opt with ISE pass"
  opt \
    ${OPT_STATS} \
    -load ${ISE_PASS} \
    -ise \
    -ise-runtime-estimation \
    -ise-algorithm=${ALGO} \
    -ise-architecture=${ARCH} \
    -ise-profile-info-file=${dir}.prof \
    -f -o ${dir}_ise.bc \
    ${dir}.bc 

}

for gcc_o in -O3  ; do
  for gcc_p in "-funroll-loops" ; do
    for opt_p in "-std-compile-opts ${OPT_STATS}" ; do
        echo "CC = llvm-gcc" > Makefile.common
        echo "CCOPTS = ${gcc_o} ${gcc_p} -emit-llvm" >> Makefile.common
        echo "" && echo ""
        echo "app = ${dir}"
        echo "gcc = ${gcc_o} ${gcc_p} -emit-llvm"
        echo "opt = ${opt_o} ${opt_p}"
        run 
      done
    done
done
# echo "CCOPTS = -O3 -fforce-mem -fforce-addr -funroll-all-loops -emit-llvm" > Makefile.common
# awk '{if ($1 ~ /ratio/) print $0}' ../${dir}_ise.log > ../${dir}_ise_ratio.log

