add_llvm_library(LLVMBitcodeStatistics MODULE
  BitcodeStatistics.cpp	
  )
# target_link_libraries(LLVMBitcodeStatistics LLVMCore LLVMSystem LLVMSupport LLVMAnalysis LLVMISECommon)
target_link_libraries(LLVMBitcodeStatistics LLVMISECommon)
set(CMAKE_MODULE_LINKER_FLAGS "-undefined dynamic_lookup")

