#!/bin/bash

# -ise-archi-max-ci=1000 -ise-archi-max-input=1000 -ise-archi-max-output=1000 \

# rm -rf results && mkdir results

opt="-ise-archi-max-ci=10 -ise-archi-max-input=10 -ise-archi-max-output=1"
opt="-ise-archi-max-ci=10 -ise-archi-max-input=2 -ise-archi-max-output=1"
opt="-ise-archi-max-ci=10 -ise-archi-max-input=4 -ise-archi-max-output=2"
for algo in maxmiso singlecut union multicut; do
  for app in adpcm blowfish cjpeg fft sha sor whetstone aes ; do
    OUT=results/ident-${algo}-${app}-i4-o2.txt
    if [ -e $OUT ] ; then rm $OUT ; fi
    ./make_bench.sh $algo $app 2>&1 |  tee $OUT
  done;
done;

#  for app in adpcm blowfish cjpeg fft sha sor whetstone aes md5; do
