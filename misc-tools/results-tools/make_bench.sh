#!/bin/bash

ARCH=virtex
# ISE_PASS=${PWD}/../b/lib/ISEPass/libLLVMISEPass.so
ISE_PASS=${PWD}/XCODE_libLLVMISEPass.so

if [ $# -lt 3 ] ; then
  echo "usage: $0 algorithm dir suffix"
  exit
fi

ALGO=$1
dir="${2/\//}"
suff="$3"
save_pwd=$PWD

RESULT_DIR="${ALGO}-${suff}/"
# remove first arg which is app name
shift 3
PARAMS=$*

# ---------------- run()  ----------------- #
function run() {
  cd $dir
  mkdir -p "$RESULT_DIR" && rm -rf ${RESULT_DIR}/*

  # --- run opt and get candidates
  RESCANDFILE="${RESULT_DIR}/ident-cand.txt"

  echo "# calculating candidates"
   opt \
    -load ${ISE_PASS} \
    -ise \
    -ise-algorithm=${ALGO} \
    -ise-architecture=${ARCH} \
    -ise-algorithm-stop \
    -disable-output \
    -stats -time-passes -track-memory \
    $PARAMS \
    ${dir}.bc  2>&1 > $RESCANDFILE
  mv cand*.dat ${RESULT_DIR}

 # run ISE pass and get benchmarking results
 echo "# calculating benchmarking results"
 RESBENCHFILE="${RESULT_DIR}/ident-bench.txt"
  counter=0;
  opt \
    ${OPT_STATS} \
    -load ${ISE_PASS} \
    -ise \
    -ise-algorithm-stop \
    -ise-benchmark \
    -ise-benchmark-ticks=100 \
    -ise-algorithm=${ALGO} \
    -ise-architecture=${ARCH} \
    -disable-output \
    $PARAMS \
    ${dir}.bc > $RESBENCHFILE  

  # ---------------- present results ----------------- #
  # read candidate results into array
  tempfoo=`basename $0`
  TMPFILE=`mktemp /tmp/${tempfoo}.XXXXXX` || exit 1
  grep "processing" $RESCANDFILE |  awk '$13 ~ /[0-9]/ {print $13}' > $TMPFILE
  # read results into array
  counter=0;
  cand=();
  while  read line; do
    cand[${counter}]=${line}
#    echo -e "cand[${counter}]=${line}"
    counter=`expr $counter + 1`
  done <  $TMPFILE
  rm $TMPFILE

  # combine benchmarking results with candidate results
  counter=0;
   grep ^[0-9] $RESBENCHFILE > $TMPFILE
   echo -e "DAG_size\t, msecs\t\t, it\t, candidate_found"
    while read line; do
      echo -e "$line \t ${cand[${counter}]}"
      counter=`expr $counter + 1`
    done < $TMPFILE | sort -n | awk \
    '$1 ~ /[0-9]+/ && $2 ~/[\.0-9]+/ && $3 ~ /[0-9]+/ && $4 ~ /[0-9]+/ \
    { $2=sprintf("%.5f",$2)} { print $1 "\t\t, " $2 "\t, " $3 "\t, " $4 }'
    rm $TMPFILE
}


# ---------------- main() ----------------- #
        echo "" && echo ""
        echo "app  = ${dir}"
        echo "algo = ${ALGO}"
        echo "arch = ${ARCH}"
        echo "opt  = ${PARAMS}"
        echo "gcc = ${gcc_o} ${gcc_p} -emit-llvm"
        run 
