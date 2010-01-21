include ../Makefile.common

BITCODE = cdjpeg.bc cjpeg.bc jcapimin.bc jcapistd.bc jccoefct.bc jccolor.bc jcdctmgr.bc jchuff.bc jcinit.bc jcmainct.bc jcmarker.bc jcmaster.bc jcomapi.bc jcparam.bc jcphuff.bc jcprepct.bc jcsample.bc jctrans.bc jdatadst.bc jerror.bc jfdctflt.bc jfdctfst.bc jfdctint.bc jmemansi.bc jmemmgr.bc jutils.bc rdbmp.bc rdgif.bc rdppm.bc rdrle.bc rdswitch.bc rdtarga.bc

cjpeg_.bc : $(BITCODE)
	llvm-link $(BITCODE) -o cjpeg_.bc

.C.BC:

cdjpeg.bc: cdjpeg.c
	$(CC) $(CCOPTS) $< -c -o $@
cjpeg.bc: cjpeg.c
	$(CC) $(CCOPTS) $< -c -o $@
jcapimin.bc: jcapimin.c
	$(CC) $(CCOPTS) $< -c -o $@
jcapistd.bc: jcapistd.c
	$(CC) $(CCOPTS) $< -c -o $@
jccoefct.bc: jccoefct.c
	$(CC) $(CCOPTS) $< -c -o $@
jccolor.bc: jccolor.c
	$(CC) $(CCOPTS) $< -c -o $@
jcdctmgr.bc: jcdctmgr.c
	$(CC) $(CCOPTS) $< -c -o $@
jchuff.bc: jchuff.c
	$(CC) $(CCOPTS) $< -c -o $@
jcinit.bc: jcinit.c
	$(CC) $(CCOPTS) $< -c -o $@
jcmainct.bc: jcmainct.c
	$(CC) $(CCOPTS) $< -c -o $@
jcmarker.bc: jcmarker.c
	$(CC) $(CCOPTS) $< -c -o $@
jcmaster.bc: jcmaster.c
	$(CC) $(CCOPTS) $< -c -o $@
jcomapi.bc: jcomapi.c
	$(CC) $(CCOPTS) $< -c -o $@
jcparam.bc: jcparam.c
	$(CC) $(CCOPTS) $< -c -o $@
jcphuff.bc: jcphuff.c
	$(CC) $(CCOPTS) $< -c -o $@
jcprepct.bc: jcprepct.c
	$(CC) $(CCOPTS) $< -c -o $@
jcsample.bc: jcsample.c
	$(CC) $(CCOPTS) $< -c -o $@
jctrans.bc: jctrans.c
	$(CC) $(CCOPTS) $< -c -o $@
jdatadst.bc: jdatadst.c
	$(CC) $(CCOPTS) $< -c -o $@
jerror.bc: jerror.c
	$(CC) $(CCOPTS) $< -c -o $@
jfdctflt.bc: jfdctflt.c
	$(CC) $(CCOPTS) $< -c -o $@
jfdctfst.bc: jfdctfst.c
	$(CC) $(CCOPTS) $< -c -o $@
jfdctint.bc: jfdctint.c
	$(CC) $(CCOPTS) $< -c -o $@
jmemansi.bc: jmemansi.c
	$(CC) $(CCOPTS) $< -c -o $@
jmemmgr.bc: jmemmgr.c
	$(CC) $(CCOPTS) $< -c -o $@
jutils.bc: jutils.c
	$(CC) $(CCOPTS) $< -c -o $@
rdbmp.bc: rdbmp.c
	$(CC) $(CCOPTS) $< -c -o $@
rdgif.bc: rdgif.c
	$(CC) $(CCOPTS) $< -c -o $@
rdppm.bc: rdppm.c
	$(CC) $(CCOPTS) $< -c -o $@
rdrle.bc: rdrle.c
	$(CC) $(CCOPTS) $< -c -o $@
rdswitch.bc: rdswitch.c
	$(CC) $(CCOPTS) $< -c -o $@
rdtarga.bc: rdtarga.c
	$(CC) $(CCOPTS) $< -c -o $@

clean:
	rm -f $(BITCODE) cjpeg_.bc
