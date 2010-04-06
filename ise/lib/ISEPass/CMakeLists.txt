set(target LLVMISEPass)
add_llvm_library(${target} MODULE ISEPass.cpp)
target_link_libraries(${target} LLVMISECommon)

if (APPLE)
  set(CMAKE_MODULE_LINKER_FLAGS "-undefined dynamic_lookup")
endif (APPLE)

# During configuration process we are creating variable in the file which points out to the LLVM ISE pass
# this is later picked up by other tools to obtain information if LLVM ISE is present and if yes then where

# solution: 1
#execute_process(
#  COMMAND echo message\("Including: llvm_ise_pass_file.cmake"\)\nset\(LLVM_ISE_PASS_FILE ${CMAKE_CURRENT_BINARY_DIR}/lib${target}.so\) 
#  OUTPUT_FILE ${CMAKE_BINARY_DIR}/llvm_ise_pass_file.cmake
#)


# solution: 2
#execute_process(COMMAND uname -a 
#    OUTPUT_VARIABLE _output OUTPUT_STRIP_TRAILING_WHITESPACE)
#define COMPILE_TIME_VALUE \"${_output}\"")

# solution: 3 (the easiest)
set(dst_file ${CMAKE_BINARY_DIR}/llvm_ise_pass_file.cmake)
file(WRITE   ${dst_file} "message(STATUS \"including ${dst_file}\")\n")
file(APPEND  ${dst_file} "set(LLVM_ISE_PASS_FILE ${CMAKE_CURRENT_BINARY_DIR}/lib${target}.so)\n")
