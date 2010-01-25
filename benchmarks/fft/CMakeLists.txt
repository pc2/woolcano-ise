project (fft C CXX)

cmake_minimum_required(VERSION 2.6)
cmake_policy(SET CMP0011 NEW) 

message("# Creating Makefile for '${PROJECT_NAME}'")

include (../enable_llvm_toolflow.cmake)

# ======================================================
# adding libraries
# ======================================================
add_bc_libraries(main.c fft.c)


# ======================================================
# linking (merging) few libraries into single library
# ======================================================
target_link_bc_libraries(fft libmain libfft) 


# ======================================================
# run profiler
run_prof(fft_prof fft)
