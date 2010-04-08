#!/bin/bash

# --- prepare frontend configuration
gcc_o="-O3"
gcc_p="-funroll-loops"

echo "CC = llvm-gcc" > Makefile.common
echo "CCOPTS = ${gcc_o} ${gcc_p} -emit-llvm" >> Makefile.common

# --- run various optimizations = prepare binary
opt_p="-std-compile-opts -stats -time-passes -track-memory"
opt_p="-std-compile-opts" 

for app in adpcm blowfish aes cjpeg fft sha sor whetstone md5; do
  cd $app
  rm -f ${app}.prof ${app}_pass.bc _${app}.log 2>&1 > /dev/null

  echo -e "\n# compilation: $app"
  make clean
  make

  echo "# running optimizations on top of: $app"
  opt $opt_p  ${app}.bc -f -o ${app}_pass.bc  
  mv ${app}.bc ${app}_no_opt.bc
  mv ${app}_pass.bc ${app}.bc
  cd ..
done
