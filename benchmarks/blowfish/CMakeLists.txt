project (blowfish C CXX)

cmake_minimum_required(VERSION 2.6)
cmake_policy(SET CMP0011 NEW) 

message("# Creating Makefile for '${PROJECT_NAME}'")
set (LEVEL ${CMAKE_CURRENT_SOURCE_DIR}/../../)
include (${LEVEL}/cmake/enable_llvm_toolflow.cmake)


# ======================================================
# adding libraries
# ======================================================
add_bc_library(libbf_cbc     bf_cbc.c)
add_bc_library(libbf_cfb64   bf_cfb64.c)
add_bc_library(libbf_ecb     bf_ecb.c)
add_bc_library(libbf_enc     bf_enc.c)
add_bc_library(libbf_ofb64   bf_ofb64.c)
add_bc_library(libbf_skey    bf_skey.c)
add_bc_library(blowfish_test blowfish_test.c)

# ======================================================
# linking (merging) few libraries into single library
# ======================================================
target_link_bc_libraries(${PROJECT_NAME} blowfish_test libbf_cbc libbf_cfb64 libbf_ecb libbf_enc libbf_ofb64 libbf_skey) 

include (${LEVEL}/cmake/enable_ise_targets.cmake)
