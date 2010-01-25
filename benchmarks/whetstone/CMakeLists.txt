project (whetstone C CXX)

cmake_minimum_required(VERSION 2.6)
cmake_policy(SET CMP0011 NEW) 

message("# Creating Makefile for '${PROJECT_NAME}'")

include (../enable_llvm_toolflow.cmake)

# ======================================================
# adding libraries
# ======================================================
add_bc_library(whetstone whetstone.c) 


# ======================================================
# run profiler
run_prof(whetstone_prof whetstone)
