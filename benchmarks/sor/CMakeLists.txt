project (sor C CXX)

cmake_minimum_required(VERSION 2.6)
cmake_policy(SET CMP0011 NEW) 

message("# Creating Makefile for '${PROJECT_NAME}'")
set (LEVEL ${CMAKE_CURRENT_SOURCE_DIR}/../../)
include (${LEVEL}/cmake/enable_llvm_toolflow.cmake)



# ======================================================
# adding libraries
# ======================================================

# like this:
# create libmain.bc from main.c   & libsor.bc from sor.c
# add_bc_library(libmain main.c) 
# add_bc_library(libsor sor.c)

# or like this
# foreach input param create bc file: generate lib${param}.bc
# add_bc_libraries(main.c sor.c)
add_bc_library(main_sor main.c)
add_bc_library(libsor sor.c)


# ======================================================
# linking (merging) few libraries into single library
# ======================================================
# link both libraries to one
target_link_bc_libraries(${PROJECT_NAME} main_sor libsor) 

include (${LEVEL}/cmake/enable_ise_targets.cmake)
