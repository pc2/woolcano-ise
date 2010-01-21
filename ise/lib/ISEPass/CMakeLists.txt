add_llvm_library(LLVMISEPass MODULE
  ISEPass.cpp
  )
# target_link_libraries(LLVMISEPass LLVMCore LLVMSystem LLVMSupport LLVMAnalysis LLVMISECommon)
target_link_libraries(LLVMISEPass LLVMISECommon)
set(CMAKE_MODULE_LINKER_FLAGS "-undefined dynamic_lookup")