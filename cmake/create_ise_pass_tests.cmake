
# =========================================================== #
# 0. check if proper params are given
FILE(RELATIVE_PATH THIS_FILE ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_LIST_FILE})
message("\n=====================================================")
message("* Executing ${THIS_FILE}")
message("=====================================================\n")

if (NOT EXISTS ${SRC_FILE})
  message(FATAL_ERROR "file ${SRC_FILE} does not exists")
endif (NOT EXISTS ${SRC_FILE})
if (NOT EXISTS ${PROF_FILE})
  message(FATAL_ERROR "file ${PROF_FILE} does not exists")
endif (NOT EXISTS ${PROF_FILE})


# =========================================================== #
# 1. check if there is llvm pass compiled
set(ise_conf_file llvm_ise_pass_file.cmake)
set(ise_pass_file libLLVMISEPass.so)

# search for configuration file
find_file(ise_conf_full  ${ise_conf_file} HINT ${BIN_DIR})
if(ise_conf_full)
  include(${ise_conf_full})
# if not found than try to find the pass
else(ise_conf_full)
  find_file(LLVM_ISE_PASS_FILE ${ise_pass_file})
endif(ise_conf_full)

if(NOT EXISTS ${LLVM_ISE_PASS_FILE})
    message(FATAL_ERROR "LLVM ISE pass not found")
endif(NOT EXISTS ${LLVM_ISE_PASS_FILE})


# =========================================================== #
# 2. if found load enable_llvm_toolflow
set(f_llvm ${LEVEL}/cmake/enable_llvm_toolflow.cmake)
if (EXISTS ${f_llvm})
  include(${f_llvm})
endif (EXISTS ${f_llvm})


# =========================================================== #
# 3. run ise_pass on app
set(working_dir "test_ise")
add_opt_pass(${LLVM_ISE_PASS_FILE})
exec_opt(${O_FILE} ${SRC_FILE} ${working_dir}
  -ise -ise-output-graphs -ise-profile-info-file ${PROF_FILE})
