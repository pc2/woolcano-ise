project (md5 C CXX)

cmake_minimum_required(VERSION 2.6)
cmake_policy(SET CMP0011 NEW) 

message("# Creating Makefile for '${PROJECT_NAME}'")

include (../enable_llvm_toolflow.cmake)

# ======================================================
# adding libraries
# ======================================================
add_bc_libraries(md5_test.c md5.c)

# ======================================================
# linking (merging) few libraries into single library
# ======================================================
target_link_bc_libraries(md5 libmd5_test libmd5) 


# ======================================================
# run profiler
run_prof(md5_prof md5)
