project (whetstone C CXX)

cmake_minimum_required(VERSION 2.6)
cmake_policy(SET CMP0011 NEW) 

message("# Creating Makefile for '${PROJECT_NAME}'")
set (LEVEL ${CMAKE_CURRENT_SOURCE_DIR}/../../)
include (${LEVEL}/cmake/enable_llvm_toolflow.cmake)

# ======================================================
# adding libraries
# ======================================================
add_bc_library(${PROJECT_NAME} whetstone.c) 


include (${LEVEL}/cmake/enable_ise_targets.cmake)
