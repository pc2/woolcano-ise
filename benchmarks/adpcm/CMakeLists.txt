project (adpcm C CXX)

cmake_minimum_required(VERSION 2.6)
cmake_policy(SET CMP0011 NEW) 

message("# Creating Makefile for '${PROJECT_NAME}'")

include (../enable_llvm_toolflow.cmake)

# ======================================================
# adding libraries
# ======================================================
add_bc_libraries(adpcm.c rawcaudio.c rawdaudio.c timing.c)


# ======================================================
# linking (merging) few libraries into single library
# ======================================================
target_link_bc_libraries(adpcm libadpcm libtiming) 
target_link_bc_libraries(rawcaudio_ libadpcm librawcaudio) 
target_link_bc_libraries(rawdaudio_ libadpcm librawdaudio) 


# ======================================================
# run profiler
run_prof(adpcm_prof adpcm)
