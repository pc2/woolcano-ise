include ../Makefile.common

BITCODE = bf_cbc.bc bf_cfb64.bc bf_ecb.bc bf_enc.bc bf_ofb64.bc bf_skey.bc blowfish_test.bc # bf.bc

blowfish.bc : $(BITCODE)
	llvm-link $(BITCODE) -f -o blowfish.bc

.C.BC:

blowfish_test.bc: blowfish_test.c
	$(CC) $(CCOPTS) $< -c -o $@
#bf.bc: bf.c
#	$(CC) $(CCOPTS) $< -c -o $@
bf_cbc.bc: bf_cbc.c
	$(CC) $(CCOPTS) $< -c -o $@
bf_cfb64.bc: bf_cfb64.c
	$(CC) $(CCOPTS) $< -c -o $@
bf_ecb.bc: bf_ecb.c
	$(CC) $(CCOPTS) $< -c -o $@
bf_enc.bc: bf_enc.c
	$(CC) $(CCOPTS) $< -c -o $@
bf_ofb64.bc: bf_ofb64.c
	$(CC) $(CCOPTS) $< -c -o $@
bf_skey.bc: bf_skey.c
	$(CC) $(CCOPTS) $< -c -o $@

clean:
	rm -f $(BITCODE) blowfish.bc
