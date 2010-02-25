project (adpcm C CXX)

cmake_minimum_required(VERSION 2.6)
cmake_policy(SET CMP0011 NEW) 

# message("# Creating Makefile for '${PROJECT_NAME}'")
set (LEVEL ${CMAKE_CURRENT_SOURCE_DIR}/../../)
include (${LEVEL}/cmake/enable_llvm_toolflow.cmake)

# ======================================================
# adding libraries
# ======================================================
add_bc_libraries(adpcm.c rawcaudio.c rawdaudio.c timing.c)


# ======================================================
# linking (merging) few libraries into single library
# ======================================================
target_link_bc_libraries(${PROJECT_NAME} libadpcm libtiming) 
# target_link_bc_libraries(rawcaudio_ libadpcm librawcaudio) 
# target_link_bc_libraries(rawdaudio_ libadpcm librawdaudio) 

include (${LEVEL}/cmake/enable_ise_targets.cmake)
