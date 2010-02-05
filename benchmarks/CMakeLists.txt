project (benchmarks C CXX)

cmake_minimum_required(VERSION 2.6)
cmake_policy(SET CMP0011 NEW) 
cmake_policy(SET CMP0002 OLD) 

message("# Creating Makefile for '${PROJECT_NAME}'")

add_custom_target(test_ise)

file(GLOB inFiles *)
foreach(entry ${inFiles})
  if(IS_DIRECTORY ${entry} AND EXISTS ${entry}/CMakeLists.txt)
    add_subdirectory(${entry})
    LIST(APPEND apps ${entry}/${entry}_ise)
  endif()
endforeach(entry)
