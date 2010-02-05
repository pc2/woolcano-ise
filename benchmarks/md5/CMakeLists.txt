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


# ======================================================
# run profiler
run_prof(${PROJECT_NAME}_prof ${PROJECT_NAME})


# ======================================================
# execute external cmake file for this target
# pass necessary variables as macros
set (t_file   ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.bc)
set (opt_file ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}_prof.out)
set (o_file   ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ise.bc)
add_custom_target(${PROJECT_NAME}_ise 
  ${CMAKE_COMMAND} 
    -DLEVEL=${LEVEL} 
    -DBIN_DIR="${CMAKE_BINARY_DIR}" 
    -DSRC_FILE="${t_file}" 
    -DPROF_FILE="${opt_file}" 
    -DO_FILE="${o_file}"
    -P ${LEVEL}/cmake/create_ise_pass_tests.cmake
   DEPENDS ${PROJECT_NAME}.bc ${PROJECT_NAME}_prof.out )

add_dependencies(test_ise ${PROJECT_NAME}_ise)
