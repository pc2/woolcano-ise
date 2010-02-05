# find path to src llvm and place it under LLVM_SRC_DIR
execute_process(
  COMMAND llvm-config --src-root 
    OUTPUT_STRIP_TRAILING_WHITESPACE
    OUTPUT_VARIABLE LLVM_SRC_DIR
)

# find path to obj llvm and place it under LLVM_OBJ_DIR
execute_process(
  COMMAND llvm-config --obj-root 
    OUTPUT_STRIP_TRAILING_WHITESPACE
    OUTPUT_VARIABLE LLVM_OBJ_DIR
)


# check if LLVM_SRC_DIR and LLVM_OBJ_DIR directories exist
if (IS_DIRECTORY ${LLVM_SRC_DIR})
    message(" .. Found LLVM SRC DIR: ${LLVM_SRC_DIR}")
else()
    message(FATAL_ERROR " .. Couldn't find llvm src dir: ${LLVM_SRC_DIR}")
endif()
if (IS_DIRECTORY ${LLVM_OBJ_DIR})
    message(" .. Found LLVM OBJ DIR: ${LLVM_OBJ_DIR}")
else()
    message(FATAL_ERROR "ERROR: Couldn't find llvm obj dir: ${LLVM_OBJ_DIR}")
endif()

# ======================================================
# add llvm support
# add module & include header directories
set(LLVM_SRC_INC_DIR "${LLVM_SRC_DIR}/include")
set(LLVM_OBJ_INC_DIR "${LLVM_OBJ_DIR}/include")
include_directories(${LLVM_SRC_INC_DIR} ${LLVM_OBJ_INC_DIR}) 

# Add path for custom modules
set(CMAKE_MODULE_PATH
  ${CMAKE_MODULE_PATH}
  "${LLVM_SRC_DIR}/cmake"
  "${LLVM_SRC_DIR}/cmake/modules"
)

# setup define MACRO's
include(${LLVM_SRC_DIR}/cmake/modules/AddLLVMDefinitions.cmake)
add_llvm_definitions( -D__STDC_LIMIT_MACROS )
add_llvm_definitions( -D__STDC_CONSTANT_MACROS )

# include rest of llvm
include(${LLVM_SRC_DIR}/cmake/modules/AddLLVM.cmake)
