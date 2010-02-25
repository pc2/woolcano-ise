# This file enables following targets:
#   (app)_prof
#   (app)_prof_stats
#   (app)_ise
#   (app)_ise_stats

# ======================================================
# create (app)_prof & (app)_prof_stats

if(NOT RUN_PROF_ARGS)
  set(RUN_PROF_ARGS "")
endif(NOT RUN_PROF_ARGS)

run_prof(${PROJECT_NAME}_prof       ${PROJECT_NAME} ""     ${RUN_PROF_ARGS})
run_prof(${PROJECT_NAME}_prof_stats ${PROJECT_NAME} -stats ${RUN_PROF_ARGS})


# ======================================================
# create ${app}_ise

set (src_file   ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.bc)
set (dst_file   ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}_ise.bc)
set (opt_file   ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}_prof.out)
set (WRK_DIR    "${PROJECT_NAME}_ise")
set (OPT_PASS_ARGS "-ise-runtime-estimation")

set (cmake_file ${CMAKE_CURRENT_BINARY_DIR}/cmake/${PROJECT_NAME}_ise.cmake)
CONFIGURE_FILE( ${LEVEL}/cmake/create_ise_pass_test.cmake.in  ${cmake_file} @ONLY)

add_custom_target(${PROJECT_NAME}_ise 
  COMMAND ${CMAKE_COMMAND} -P ${cmake_file}
  DEPENDS LLVMISEPass ${opt_file} ${t_file})


# ======================================================
# create ${app}_ise_stats

set (src_file   ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.bc)
set (dst_file   ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}_ise_stats.bc)
set (opt_file   ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}_prof_stats.out)
set (stats_file ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}_ise_stats.bc.txt)
set (WRK_DIR    "${PROJECT_NAME}_ise_stats")
set (OPT_PASS_ARGS -info-output-file=${stats_file} -time-passes -track-memory)

set (cmake_file ${CMAKE_CURRENT_BINARY_DIR}/cmake/${PROJECT_NAME}_ise_stats.cmake)
CONFIGURE_FILE( ${LEVEL}/cmake/create_ise_pass_test.cmake.in  ${cmake_file} @ONLY)

add_custom_target(${PROJECT_NAME}_ise_stats
  COMMAND ${CMAKE_COMMAND} -P ${cmake_file}
  DEPENDS LLVMISEPass ${opt_file} ${t_file})

# ======================================================
# create ${app}_ise_bench

set (src_file   ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.bc)
set (dst_file   ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}_ise_bench.bc)
set (opt_file   ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}_prof.out)
set (WRK_DIR    "${PROJECT_NAME}_ise_bench")
set (OPT_PASS_ARGS -ise-benchmark)

set (cmake_file ${CMAKE_CURRENT_BINARY_DIR}/cmake/${PROJECT_NAME}_ise_bench.cmake)
CONFIGURE_FILE( ${LEVEL}/cmake/create_ise_pass_test.cmake.in  ${cmake_file} @ONLY)

add_custom_target(${PROJECT_NAME}_ise_bench
  COMMAND ${CMAKE_COMMAND} -P ${cmake_file}
  DEPENDS LLVMISEPass ${opt_file} ${t_file})



# ======================================================
if(TARGET test_ise)
  add_dependencies(test_ise ${PROJECT_NAME}_ise)
endif(TARGET test_ise)

if(TARGET test_ise_stats)
  add_dependencies(test_ise_stats ${PROJECT_NAME}_ise_stats)
endif(TARGET test_ise_stats)

if(TARGET test_ise_bench)
  add_dependencies(test_ise_bench ${PROJECT_NAME}_ise_bench)
endif(TARGET test_ise_bench)

# ======================================================
set(clean_files 
  ${PROJECT_NAME}_ise.bc ${PROJECT_NAME}_ise_stats.bc 
  ${PROJECT_NAME}_ise ${PROJECT_NAME}_ise_stats)
set_directory_properties(PROPERTIES 
  ADDITIONAL_MAKE_CLEAN_FILES "${clean_files}")
