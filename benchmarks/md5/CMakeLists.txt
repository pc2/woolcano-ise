project (md5 C CXX)

cmake_minimum_required(VERSION 2.6)
cmake_policy(SET CMP0011 NEW) 

message("# Creating Makefile for '${PROJECT_NAME}'")
set (LEVEL ${CMAKE_CURRENT_SOURCE_DIR}/../../)
include (${LEVEL}/cmake/enable_llvm_toolflow.cmake)


# ======================================================
# adding libraries
# ======================================================
add_bc_libraries(md5_test.c md5.c)

# ======================================================
# linking (merging) few libraries into single library
# ======================================================
target_link_bc_libraries(${PROJECT_NAME} libmd5_test libmd5) 

include (${LEVEL}/cmake/enable_ise_targets.cmake)
