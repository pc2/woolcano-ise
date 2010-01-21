include ../Makefile.common

BITCODE = adpcm.bc rawcaudio.bc rawdaudio.bc timing.bc

timing_.bc : $(BITCODE)
	llvm-link adpcm.bc timing.bc -f -o timing_.bc
rawcaudio_.bc : $(BITCODE)
	llvm-link adpcm.bc rawcaudio.bc -f -o rawcaudio_.bc
rawdaudio_.bc : $(BITCODE)
	llvm-link adpcm.bc rawdaudio.bc -f -o rawdaudio_.bc

.C.BC:

adpcm.bc: adpcm.c
	$(CC) $(CCOPTS) $< -c -o $@
timing.bc: timing.c
	$(CC) $(CCOPTS) $< -c -o $@
rawcaudio.bc: rawcaudio.c
	$(CC) $(CCOPTS) $< -c -o $@
rawdaudio.bc: rawdaudio.c
	$(CC) $(CCOPTS) $< -c -o $@

clean:
	rm -f $(BITCODE) timing_.bc rawcaudioc_.bc rawdaudioc_.bc
