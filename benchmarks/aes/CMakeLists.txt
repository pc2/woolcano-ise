project (aes C CXX)

cmake_minimum_required(VERSION 2.6)
cmake_policy(SET CMP0011 NEW) 

message("# Creating Makefile for '${PROJECT_NAME}'")

include (../enable_llvm_toolflow.cmake)

# ======================================================
# adding libraries
# ======================================================
add_bc_library(libaes_modes aes_modes.c)
add_bc_library(libaes_test  aes_test.cpp)
add_bc_library(libaescrypt  aescrypt.c) 
add_bc_library(libaeskey    aeskey.c) 
add_bc_library(libaestab    aestab.c)


# ======================================================
# linking (merging) few libraries into single library
# ======================================================
# link both libraries to one
target_link_bc_libraries(aes libaes_modes libaes_test libaescrypt libaeskey libaestab)

# ======================================================
# run profiler
run_prof(aes_prof aes)
