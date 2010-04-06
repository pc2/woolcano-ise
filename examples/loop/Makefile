# run like this:
# make clean all && make diff_gcc
# make clean all ** make diff_opt


CC             =  llvm-gcc 
CCOPTS_roll    =  -O3 -emit-llvm
CCOPTS_unroll  =  -O3 -funroll-all-loops -emit-llvm

OPT            =   opt
# OPTOPTS_unroll =   -f -std-compile-opts -loop-unroll -unroll-allow-partial 
OPTOPTS_unroll =   -std-compile-opts 

DISS           =  llvm-dis

SOURCE = main.cc

all: rolled_gcc.bc unrolled_gcc.bc unrolled_opt.bc 

.PHONY: diff_gcc
diff_gcc: rolled_gcc.ll unrolled_gcc.ll
	diff -y -W200 $^

.PHONY: diff_opt
diff_opt: rolled_gcc.ll unrolled_opt.ll
	diff -y -W200 $^

rolled_gcc.ll: rolled_gcc.bc
	$(DISS) $<

unrolled_gcc.ll: unrolled_gcc.bc
	$(DISS) $<

unrolled_opt.ll: unrolled_opt.bc
	$(DISS) $<

unrolled_opt.bc: rolled_gcc.bc
	$(OPT) $(OPTOPTS_unroll) $< -o $@

rolled_gcc.bc: $(SOURCE)
	$(CC) $(CCOPTS_roll) $< -c -o $@


unrolled_gcc.bc: $(SOURCE)
	$(CC) $(CCOPTS_unroll) $< -c -o $@

.PHONY: clean
clean:
	rm -f rolled_{gcc,opt}.{bc,ll} unrolled_{gcc,opt}.{bc,ll}
