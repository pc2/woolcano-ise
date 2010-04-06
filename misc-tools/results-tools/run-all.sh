#!/bin/bash

./make.sh adpcm     2>&1 | tee adpcm.log
./make.sh aes       2>&1 | tee aes.log
./make.sh blowfish  2>&1 | tee blowfish.log
./make.sh fft       2>&1 | tee fft.log
./make.sh md5       2>&1 | tee md5.log
./make.sh sha       2>&1 | tee sha.log
./make.sh sor       2>&1 | tee sor.log
./make.sh whetstone 2>&1 | tee whetstone.log
