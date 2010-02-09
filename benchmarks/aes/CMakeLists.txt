project (aes C CXX)

cmake_minimum_required(VERSION 2.6)
cmake_policy(SET CMP0011 NEW) 

message("# Creating Makefile for '${PROJECT_NAME}'")
set (LEVEL ${CMAKE_CURRENT_SOURCE_DIR}/../../)
include (${LEVEL}/cmake/enable_llvm_toolflow.cmake)

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
target_link_bc_libraries(${PROJECT_NAME} libaes_modes libaes_test libaescrypt libaeskey libaestab)

include (${LEVEL}/cmake/enable_ise_targets.cmake)
