include ../Makefile.common

BITCODE = md5_test.bc md5.bc

md5main_.bc : $(BITCODE)
	llvm-link $(BITCODE) -f -o md5_test_.bc

.C.BC:

md5_test.bc: md5_test.c
	$(CC) $(CCOPTS) $< -c -o $@

md5.bc: md5.c
	$(CC) $(CCOPTS) $< -c -o $@

clean:
	rm -f $(BITCODE) md5_test_.bc
