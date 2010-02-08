project (cjpeg C CXX)

cmake_minimum_required(VERSION 2.6)
cmake_policy(SET CMP0011 NEW) 

message("# Creating Makefile for '${PROJECT_NAME}'")
set (LEVEL ${CMAKE_CURRENT_SOURCE_DIR}/../../)
include (${LEVEL}/cmake/enable_llvm_toolflow.cmake)


# ======================================================
# adding libraries
# ======================================================
add_bc_libraries( 
  cdjpeg.c 
  cjpeg.c 
  jcapimin.c 
  jcapistd.c 
  jccoefct.c 
  jccolor.c 
  jcdctmgr.c 
  jchuff.c 
  jcinit.c 
  jcmainct.c 
  jcmarker.c 
  jcmaster.c 
  jcomapi.c 
  jcparam.c 
  jcphuff.c 
  jcprepct.c 
  jcsample.c 
  jctrans.c 
  jdatadst.c 
  jerror.c 
  jfdctflt.c 
  jfdctfst.c 
  jfdctint.c 
  jmemansi.c 
  jmemmgr.c 
  jutils.c 
  rdbmp.c 
  rdgif.c 
  rdppm.c 
  rdrle.c 
  rdswitch.c 
  rdtarga.c)


# ======================================================
# linking (merging) few libraries into single library
# ======================================================
# link both libraries to one
target_link_bc_libraries( ${PROJECT_NAME}
  libcdjpeg 
  libcjpeg 
  libjcapimin
  libjcapistd 
  libjccoefct 
  libjccolor 
  libjcdctmgr 
  libjchuff 
  libjcinit 
  libjcmainct 
  libjcmarker 
  libjcmaster 
  libjcomapi 
  libjcparam 
  libjcphuff 
  libjcprepct 
  libjcsample 
  libjctrans 
  libjdatadst 
  libjerror 
  libjfdctflt 
  libjfdctfst 
  libjfdctint 
  libjmemansi 
  libjmemmgr 
  libjutils 
  librdbmp 
  librdgif 
  librdppm 
  librdrle 
  librdswitch 
  librdtarga)


# ======================================================
# run profiler
run_prof(${PROJECT_NAME}_prof ${PROJECT_NAME} -dct float -progressive -opt -outfile ${CMAKE_CURRENT_SOURCE_DIR}/output1/output_small_encode_.jpeg ${CMAKE_CURRENT_SOURCE_DIR}/input1/input_small.ppm )


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
   DEPENDS ${PROJECT_NAME}.bc ${PROJECT_NAME}_prof.out LLVMISEPass )

add_dependencies(test_ise ${PROJECT_NAME}_ise)
