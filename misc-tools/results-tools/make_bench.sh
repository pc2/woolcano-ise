#!/bin/bash

ARCH=virtex
# ISE_PASS=${PWD}/../b/lib/ISEPass/libLLVMISEPass.so
ISE_PASS=${PWD}/XCODE_libLLVMISEPass.so
# OPT_STATS="-stats -time-passes -track-memory"
OPT_STATS=""

if [ $# != 2 ] ; then
  echo "usage: $0 algorithm dir"
  exit
fi

ALGO=$1
dir="${2/\//}"
save_pwd=$PWD


# remove first arg which is app name
shift 2


function run() {
  cd $dir
  rm -f ${dir}.prof ${dir}_pass.bc
  make clean && make

  # run necessary optimizations
  echo "# running opt with passes (optimizing IR with $opt_p)"
  opt $opt_p  ${dir}.bc -f -o ${dir}_pass.bc  
  mv ${dir}_pass.bc ${dir}.bc

  tempfoo=`basename $0`
  TMPFILE=`mktemp /tmp/${tempfoo}.XXXXXX` || exit 1

   opt \
    -load ${ISE_PASS} \
    -ise \
    -ise-algorithm=${ALGO} \
    -ise-algorithm-stop=true \
    -ise-architecture=${ARCH} \
    -disable-output \
    $* \
    ${dir}.bc  2>&1 | grep "processing" |  awk '$13 ~ /[0-9]/ {print $13}' > $TMPFILE


  # read results into array
  counter=0;
  cand=();
  while  read line; do
    cand[${counter}]=${line}
    counter=`expr $counter + 1`
  done < $TMPFILE
  rm $TMPFILE


  # run ISE pass
 echo "# running opt with ISE pass: $*"
  counter=0;
  opt \
    ${OPT_STATS} \
    -load ${ISE_PASS} \
    -ise \
    -ise-benchmark \
    -ise-benchmark-ticks=100 \
    -ise-algorithm=${ALGO} \
    -ise-architecture=${ARCH} \
    -disable-output \
    $* \
    ${dir}.bc  | grep ^[0-9] > $TMPFILE
   
   echo -e "Nodes\t, msecs\t\t, it\t, candidate_nodes"
    while read line; do
      echo -e "$line \t ${cand[${counter}]}"
      counter=`expr $counter + 1`
    done < $TMPFILE | sort -n | awk \
    '$1 ~ /[0-9]+/ && $2 ~/[\.0-9]+/ && $3 ~ /[0-9]+/ && $4 ~ /[0-9]+/ \
    { $2=sprintf("%.5f",$2)} { print $1 "\t, " $2 "\t, " $3 "\t, " $4 }'
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

