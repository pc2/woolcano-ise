CC = llvm-gcc
CFLAGS = --emit-llvm -O3

#SOURCE = input
SOURCE = main

all: test_opt

.PHONY: test_opt
test_opt: $(SOURCE).bc
	@echo ""
	@echo "-----------------------------------"
	opt -load ../../Debug/lib/bb-info.so -bb_info -bb-info-detailed -disable-output $< 
	
$(SOURCE).bc: $(SOURCE).cc
	$(CC) $(CFLAGS) -c $? -o $(SOURCE).bc

.PHONY: clean
clean:	
	rm -f $(SOURCE).{ll,bc}
