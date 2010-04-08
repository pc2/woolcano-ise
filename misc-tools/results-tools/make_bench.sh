#!/bin/bash

ARCH=virtex
# ISE_PASS=${PWD}/../b/lib/ISEPass/libLLVMISEPass.so
ISE_PASS=${PWD}/XCODE_libLLVMISEPass.so

if [ $# -lt 2 ] ; then
  echo "usage: $0 algorithm dir"
  exit
fi

ALGO=$1
dir="${2/\//}"
save_pwd=$PWD


# remove first arg which is app name
shift 2
PARAMS=$*

function run() {
  cd $dir

  # --- run opt and get candidates
  tempfoo=`basename $0`
  TMPFILE=`mktemp /tmp/${tempfoo}.XXXXXX` || exit 1

  echo "# calculating candidates"
   opt \
    -load ${ISE_PASS} \
    -ise \
    -ise-algorithm=${ALGO} \
    -ise-algorithm-stop=true \
    -ise-architecture=${ARCH} \
    -disable-output \
    $PARAMS \
    ${dir}.bc  2>&1 | grep "processing" |  awk '$13 ~ /[0-9]/ {print $13}' > $TMPFILE


  # read results into array
  counter=0;
  cand=();
  while  read line; do
    cand[${counter}]=${line}
    counter=`expr $counter + 1`
  done < $TMPFILE
  rm $TMPFILE


 # run ISE pass and get benchmarking results
 echo "# calculating benchmarking results"
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
    $PARAMS \
    ${dir}.bc  | grep ^[0-9] > $TMPFILE
  

  # combine benchmarking results with candidate results
   echo -e "Nodes\t, msecs\t\t, it\t, candidate_nodes"
    while read line; do
      echo -e "$line \t ${cand[${counter}]}"
      counter=`expr $counter + 1`
    done < $TMPFILE | sort -n | awk \
    '$1 ~ /[0-9]+/ && $2 ~/[\.0-9]+/ && $3 ~ /[0-9]+/ && $4 ~ /[0-9]+/ \
    { $2=sprintf("%.5f",$2)} { print $1 "\t, " $2 "\t, " $3 "\t, " $4 }'
}


# ---------------- main() ----------------- #
        echo "" && echo ""
        echo "app  = ${dir}"
        echo "algo = ${ALGO}"
        echo "arch = ${ARCH}"
        echo "opt  = ${PARAMS}"
        echo "gcc = ${gcc_o} ${gcc_p} -emit-llvm"
        run 
