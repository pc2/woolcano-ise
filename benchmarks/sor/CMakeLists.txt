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


# ======================================================
# run profiler
run_prof(${PROJECT_NAME}_prof ${PROJECT_NAME})


# ======================================================
# execute external cmake file for this target
# pass necessary variables as macros
set (t_file   ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.bc)
set (opt_file ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}_prof.out)
set (o_file   ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}_ise.bc)

#add_custom_command(
#   OUTPUT sor_ise.bc
#   COMMAND
#     ${CMAKE_COMMAND} 
#       -DLEVEL=${LEVEL} 
#       -DBIN_DIR="${CMAKE_BINARY_DIR}" 
#       -DSRC_FILE="${t_file}" 
#       -DPROF_FILE="${opt_file}" 
#       -DO_FILE="${o_file}"
#       -P ${LEVEL}/cmake/create_ise_pass_tests.cmake
#   DEPENDS ${PROJECT_NAME}.bc ${PROJECT_NAME}_prof.out )
# add_custom_target(${PROJECT_NAME}_ise ALL DEPENDS  sor_ise.bc)


add_custom_target( ${PROJECT_NAME}_ise
   ${CMAKE_COMMAND} 
       -DLEVEL=${LEVEL} 
       -DBIN_DIR="${CMAKE_BINARY_DIR}" 
       -DSRC_FILE="${t_file}" 
       -DPROF_FILE="${opt_file}" 
       -DO_FILE="${o_file}"
       -P ${LEVEL}/cmake/create_ise_pass_tests.cmake
   DEPENDS ${PROJECT_NAME}.bc ${PROJECT_NAME}_prof.out )

add_dependencies(test_ise ${PROJECT_NAME}_ise)
