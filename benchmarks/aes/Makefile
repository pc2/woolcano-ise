include ../Makefile.common

BITCODE = aes_modes.bc aes_test.bc aescrypt.bc aeskey.bc aestab.bc

aes.bc: $(BITCODE)
	llvm-link $(BITCODE) -f -o aes.bc

.C.CPP.BC:

aes_modes.bc: aes_modes.c
	$(CC) $(CCOPTS) $< -c -o $@
aes_test.bc: aes_test.cpp
	$(CC) $(CCOPTS) $< -c -o $@
aescrypt.bc: aescrypt.c
	$(CC) $(CCOPTS) $< -c -o $@
aeskey.bc: aeskey.c
	$(CC) $(CCOPTS) $< -c -o $@
aestab.bc: aestab.c
	$(CC) $(CCOPTS) $< -c -o $@

clean:
	rm -f $(BITCODE) aes_.bc
