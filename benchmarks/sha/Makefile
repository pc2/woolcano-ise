include ../Makefile.common

BITCODE = sha_test.bc sha.bc
#-DUNROLL_LOOPS 

sha_test_.bc : $(BITCODE)
	llvm-link $(BITCODE) -f -o sha_test_.bc

.C.BC:

sha.bc: sha.c
	$(CC) $(CCOPTS) $< -c -o $@

sha_test.bc: sha_test.c
	$(CC) $(CCOPTS) $< -c -o $@

clean:
	rm -f $(BITCODE) sha_test_.bc
