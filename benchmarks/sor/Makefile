include ../Makefile.common

BITCODE = _sor.bc main.bc

sor.bc : $(BITCODE)
	llvm-link $(BITCODE) -f -o sor.bc

_sor.bc: sor.c
	$(CC) $(CCOPTS) $< -c -o $@

main.bc: main.c
	$(CC) $(CCOPTS) $< -c -o $@

clean:
	rm -rf sor.bc $(BITCODE) sor_test.bc tmp* *_txt *prof *ll *bc *log dots
