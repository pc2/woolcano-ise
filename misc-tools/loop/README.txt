It does not make sens what kind of options are we using with the llvm-gcc.
It does not unroll the loop at all!

With the 'opt' we have to use -std-compile-opts which includes -loop-unroll
amongst the others. The loop-unroll pass works when it is connected with other
passes.
