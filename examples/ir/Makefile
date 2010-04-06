CC             =  llvm-gcc 
CCOPTS_roll    =  -O3 -emit-llvm
CCOPTS_unroll  =  -O3 -funroll-all-loops -emit-llvm

OPT            =   opt
# OPTOPTS_unroll =   -f -std-compile-opts -loop-unroll -unroll-allow-partial 
OPTOPTS_unroll =   -std-compile-opts 

DISS           =  llvm-dis
DISS_OPT       =  

SOURCE = main.cc

all: main.bc main.ll
	./vis.sh main.bc

main.ll: main.bc
	$(DISS) $< $(DISS_OPT) -f -o $@

main.bc: $(SOURCE)
	$(CC) $(CCOPTS_roll) $< -c -o $@

.PHONY: clean
clean:
	rm -f main.{ll,bc}
