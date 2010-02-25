# ======================================================
# create verbose status message

# message(STATUS "Enable LLVM toolflow")
# message(STATUS "====================")


set (compiler        "llvm-gcc")
set (linker          "llvm-link")
set (dis             "llvm-dis")
set (opt             "opt")
set (profiler        "" )

set (COMPILER_LINKER_BC_FLAGS    -c --emit-llvm)
set (PROFILER_FLAGS              -block)

if (NOT DEFINED COMPILER_FLAGS)
  set (COMPILER_FLAGS              "")
endif (NOT DEFINED COMPILER_FLAGS)
if (NOT DEFINED OPT_FLAGS)
  set (OPT_FLAGS                   "")
endif (NOT DEFINED OPT_FLAGS)

set (LIB_PREFIX  "lib")
set (BC_SUFFIX   "bc")
set (LL_SUFFIX   "ll")
set (PROF_SUFFIX "out")

message("COMPILER_FLAGS = ${COMPILER_FLAGS}")
message("OPT_FLAGS      = ${OPT_FLAGS}")

# ======================================================
# get full paths to the tools

find_program(LLVM_GCC ${compiler})
if (LLVM_GCC)
#   message(STATUS "Found compiler       under: ${LLVM_GCC}")
else(LLVM_GCC)
#   message(FATAL_ERROR "ERROR: Could not find ${compiler}")
endif(LLVM_GCC)

find_program(LLVM_LINKER ${linker})
if (LLVM_LINKER) 
#   message(STATUS "Found linker         under: ${LLVM_LINKER}")
else(LLVM_LINKER)
#  message(FATAL_ERROR "ERROR: Could not find ${linker}")
endif(LLVM_LINKER)

find_program(LLVM_DIS ${dis})
if (LLVM_DIS) 
#   message(STATUS "Found dissasembler   under: ${LLVM_DIS}")
else(LLVM_DIS)
#  message(FATAL_ERROR "ERROR: Could not find ${dis}")
endif(LLVM_DIS)

find_program(LLVM_OPT ${opt})
if (LLVM_OPT) 
#   message(STATUS "Found opt            under: ${LLVM_OPT}")
else(LLVM_OPT)
#  message(FATAL_ERROR "ERROR: Could not find ${opt}")
endif(LLVM_OPT)



# ======================================================
# setup profiler

# find path to src llvm and place it under LLVM_SRC_DIR
execute_process(
  COMMAND llvm-config --src-root 
    OUTPUT_STRIP_TRAILING_WHITESPACE
    OUTPUT_VARIABLE LLVM_SRC_DIR
)
# check if LLVM_SRC_DIR directory exist
if (IS_DIRECTORY ${LLVM_SRC_DIR})
#    message(STATUS "Found LLVM SRC DIR   under: ${LLVM_SRC_DIR}")
else()
#    message(FATAL_ERROR " .. Couldn't find llvm src dir: ${LLVM_SRC_DIR}")
endif()

set(LLVM_PROFILER   ${LLVM_SRC_DIR}/utils/profile.pl)
if(EXISTS ${LLVM_PROFILER})
#   message(STATUS "Found profiler       under: ${LLVM_PROFILER}")
else()
#  message(FATAL_ERROR "ERROR: Could not find profiler")
endif()

message("\n")

# ======================================================
# generates bc out of the source files
# params: t = target
#         s = source

macro(add_bc_library t s)
  set(t_outfile      ${t}.${BC_SUFFIX})
  set(t_outfile_full ${CMAKE_CURRENT_BINARY_DIR}/${t_outfile})
  set(src            ${CMAKE_CURRENT_SOURCE_DIR}/${s})

  # message(STATUS "add_bc_library:  ${t} <=  ${src}")
  # bc_generator
  add_custom_command(
    OUTPUT ${t_outfile}
    COMMAND ${LLVM_GCC} ${COMPILER_LINKER_BC_FLAGS} ${src} -o ${t_outfile_full}
    DEPENDS ${src}
  )

  # register target and make dependency with bc_generator
  add_custom_target(${t} ALL DEPENDS ${t_outfile})
  # assosiate target with output file
  set_target_properties(${t} PROPERTIES OUTPUT_NAME ${t_outfile})
endmacro(add_bc_library)

macro(add_bc_libraries)
   foreach (src ${ARGN})
      string(REGEX REPLACE ".c$|.cxx$|.cc$|.cpp$" "" t ${src})
      add_bc_library(${LIB_PREFIX}${t} ${src})
   endforeach(src)
endmacro(add_bc_libraries)
# ======================================================
# generates ll out of the bc_targets
# params: t = bc_target

macro(run_dis)
   foreach (t ${ARGN})
    set(t_outfile      ${CMAKE_CURRENT_BINARY_DIR}/${t}.${LL_SUFFIX})
    get_target_property(src ${t} OUTPUT_NAME)
    
    add_custom_command(TARGET ${t} POST_BUILD
       COMMAND  ${LLVM_DIS} ${src} -f -o ${t_outfile}
    )
   endforeach(t)
endmacro(run_dis)

# ======================================================
# links *.bc files together
# params: t       = target
#         ${ARGN} = bc_targets

macro(target_link_bc_libraries t)
   set(t_outfile ${CMAKE_CURRENT_BINARY_DIR}/${t}.${BC_SUFFIX})

   # get source files from bc_targets
   set(bc_srcs "")
   foreach (i ${ARGN})
      get_target_property(s ${i} OUTPUT_NAME)
      set(bc_srcs ${bc_srcs} ${s})
   endforeach(i)

   # linker_generator - it depends on ${bc_srcs} and these will be generated
   add_custom_command(
      OUTPUT ${t_outfile}
      COMMAND ${LLVM_LINKER} ${bc_srcs} -f -o ${t_outfile}
      DEPENDS ${bc_srcs}
   )

   # register target and make dependency with the linker_generator 
   add_custom_target(${t} ALL DEPENDS ${t_outfile})
   # assosiate target with output file
   set_target_properties(${t} PROPERTIES OUTPUT_NAME ${t_outfile})
   set_directory_properties(${CMAKE_CURRENT_BINARY_DIR} ADDITIONAL_MAKE_CLEAN_FILES ${t_outfile})
endmacro(target_link_bc_libraries)


# ======================================================
# load custom pass to the opt 
macro(add_opt_pass)
   foreach (i ${ARGN})
      set (OPT_FLAGS  ${OPT_FLAGS} -load ${i})
   endforeach(i)
endmacro(add_opt_pass)


# ======================================================
# run opt on specified file
# params: t       = target
#         s       = file (source)
#         ${ARGN} = additional params
macro(run_opt_file t src)
   set(t_outfile ${CMAKE_CURRENT_BINARY_DIR}/${t}.${BC_SUFFIX})

   # opt_generator
   add_custom_command(
      OUTPUT  ${t_outfile} 
      COMMAND ${LLVM_OPT} ${OPT_FLAGS} ${src} ${ARGN} -f -o ${t_outfile}
      DEPENDS ${src}
   )

   # register target and make dependency with the opt_generator 
   # this will be executed allwasy (make all)
   add_custom_target(${t} ALL DEPENDS ${t_outfile})
   # assosiate target with output file
   set_target_properties(${t} PROPERTIES OUTPUT_NAME ${t_outfile})
endmacro(run_opt_file)

# ======================================================
# execute opt without building the target 
# params: o      = output file
#         src    = src file
#         dir    = working directory
#        ${ARGN} = other commands
macro(exec_opt o src dir)
  file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${dir})
  message("\n\nRunning opt \n===========")
  message("WORKING_DIRECTORY: ${PROJECT_NAME}/${dir}/")
  message(" COMMAND ${LLVM_OPT} ${OPT_FLAGS} -f -o ${o} ${src} ${ARGN}\n")
  execute_process(
    COMMAND ${LLVM_OPT} ${OPT_FLAGS} -f -o ${o} ${src} ${ARGN}
    WORKING_DIRECTORY ${dir})
endmacro(exec_opt src)


# ======================================================
# run opt on specified target
# params: t       = target
#         s       = bc_target (source)
#         ${ARGN} = additional params
macro(run_opt t s)
   set(t_outfile ${CMAKE_CURRENT_BINARY_DIR}/${t}.${BC_SUFFIX})
   get_target_property(src ${s} OUTPUT_NAME)
   run_opt_file(${t} ${src} ${ARGN})
endmacro(run_opt)



# ======================================================
# run profiling 
# params:  t       = target
#          s       = bc_target (source)
#          ${ARGN} = additional args for the running app = ./app ${ARGN}
# output:  ${t}.out, ${t}.out_decoded_txt
#
# if ${ARGN} = "-stats"
# output: ${t}.1_instr_txt ${t}.2_lli_txt   = stats 

macro(run_prof t s stats)
   set(t_outfile     ${CMAKE_CURRENT_BINARY_DIR}/${t}.${PROF_SUFFIX})
   set(t_outfile_txt ${CMAKE_CURRENT_BINARY_DIR}/${t}.${PROF_SUFFIX}_decoded_txt)

   # include this file to cleanup when profiler with stats was executed
   if ("${stats}" STREQUAL "-stats")
    set(t_prof  
      ${CMAKE_CURRENT_BINARY_DIR}/${t}.1_instr_txt 
      ${CMAKE_CURRENT_BINARY_DIR}/${t}.2_lli_txt)
   else ("${stats}" STREQUAL "-stats")
     set(t_prof "")
   endif ("${stats}" STREQUAL "-stats")

   get_target_property(src ${s} OUTPUT_NAME)

   # profiler_generator
   add_custom_command(
      OUTPUT  ${t_outfile} ${t_outfile_txt} ${t_prof}
      COMMAND ${LLVM_PROFILER} ${PROFILER_FLAGS} ${stats} -- ${src} ${ARGN} > ${t_outfile_txt}
      COMMAND mv llvmprof.out ${t_outfile}
      DEPENDS ${src}
   )

   # register target and make dependency with the profiler_generator 
   add_custom_target(${t} DEPENDS ${t_outfile})
   # assosiate target with output file
   set_target_properties(${t} PROPERTIES OUTPUT_NAME ${t_outfile})


endmacro(run_prof)

# ======================================================
