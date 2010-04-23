#!/bin/bash

DST_DIR=results

# removing tasks which are not finished
./res-rm-not-finished.sh ${DST_DIR}

# generating commands for maxmiso

dir="results/maxmiso"
algo="maxmiso"
mkdir -p $dir

for app in adpcm aes blowfish cjpeg fft sha sor whetstone md5; do
  OUT="$dir/ident-${algo}-${app}.txt"
  if [ ! -e $OUT ] ; then 
    opt="-ise-alarm-stop=3600 "
    echo "mkdir -p $dir && ./make_bench.sh $algo $app i0-o0 $opt 2>&1 |  tee $OUT"
  fi
done;



# generating commands for the rest
for i in 2 4 8; do
  for o in 1 2 ; do
    dir="results/i${i}-o${o}"
    mkdir -p $dir

    for algo in singlecut union ; do
      for app in adpcm aes blowfish cjpeg fft sha sor whetstone md5; do
        OUT="$dir/ident-${algo}-${app}.txt"
        if [ ! -e $OUT ] ; then 
          opt="-ise-alarm-stop=3600 -ise-archi-max-input=${i} -ise-archi-max-output=${o}"
          echo "mkdir -p $dir && ./make_bench.sh $algo $app i${i}-o${o} $opt 2>&1 |  tee $OUT"
        fi
      done;
    done;
  done;
done;
