include ../Makefile.common

BITCODE = sor.bc main.bc

sor_test.bc : $(BITCODE)
	llvm-link $(BITCODE) -f -o sor_test.bc

.C.BC:

sor.bc: sor.c
	$(CC) $(CCOPTS) $< -c -o $@

main.bc: main.c
	$(CC) $(CCOPTS) $< -c -o $@

clean:
	rm -f $(BITCODE) sor_test.bc
