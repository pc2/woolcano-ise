#!/bin/bash

for app in adpcm aes cjpeg blowfish fft md5 sha sor whetstone cjpeg; do
  echo -n "$app, "
  opt -load ./bb-info.dylib -bb_info -disable-output $app/$app.bc | awk '{printf $2 ", " $4 ", " $6 "\n"}'
done;
