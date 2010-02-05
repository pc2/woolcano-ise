# This is example of CMakeLists.txt which shows the syntax for llvm toolflow

project (sor C CXX)

cmake_minimum_required(VERSION 2.6)
cmake_policy(SET CMP0011 NEW) 

message("# Creating Makefile for '${PROJECT_NAME}'")

include (enable_llvm_toolflow.cmake)

# ======================================================
# adding libraries
# ======================================================

# like this:
# create libmain.bc from main.c   & libsor.bc from sor.c
# add_bc_library(libmain main.c) 
# add_bc_library(libsor sor.c)

# or equivalent like this (will generate targets lib${param}.bc}
add_bc_libraries(main.c sor.c)


# ======================================================
# linking (merging) few libraries into single library
# ======================================================
# link both libraries to one
target_link_bc_libraries(sor libmain libsor) 


# ======================================================
# optimization pass
# ======================================================
# load custom passes to the optimizer
add_opt_pass( )

# run the optimizer with the -dce pass: generate sor_opt.bc from sor.bc  
run_opt(sor_opt sor -dce)


# ======================================================
# dissasemble the output: generate sor_opt.ll sor.ll main.ll
run_dis(sor_opt libsor libmain)


# ======================================================
# run profiler
run_prof(sor_prof sor_opt sor_opt_args )
