include ../Makefile.common

BITCODE = sha_test.bc _sha.bc
#-DUNROLL_LOOPS 

sha.bc : $(BITCODE)
	llvm-link $(BITCODE) -f -o sha.bc

.C.BC:

_sha.bc: sha.c
	$(CC) $(CCOPTS) $< -c -o $@

sha_test.bc: sha_test.c
	$(CC) $(CCOPTS) $< -c -o $@

clean:
	rm -f $(BITCODE) sha_test_.bc
