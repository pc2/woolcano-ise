include ../Makefile.common

BITCODE = _fft.bc main.bc

fft.bc : $(BITCODE)
	llvm-link $(BITCODE) -f -o fft.bc

.C.BC:

_fft.bc: fft.c
	$(CC) $(CCOPTS) $< -c -o $@

main.bc: main.c
	$(CC) $(CCOPTS) $< -c -o $@

clean:
	rm -f $(BITCODE) fft_test.bc
