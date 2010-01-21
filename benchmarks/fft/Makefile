include ../Makefile.common

BITCODE = fft.bc main.bc

fft_test.bc : $(BITCODE)
	llvm-link $(BITCODE) -f -o fft_test.bc

.C.BC:

fft.bc: fft.c
	$(CC) $(CCOPTS) $< -c -o $@

main.bc: main.c
	$(CC) $(CCOPTS) $< -c -o $@

clean:
	rm -f $(BITCODE) fft_test.bc
