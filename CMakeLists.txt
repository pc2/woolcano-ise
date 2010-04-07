project (woolcano-ise C CXX)

cmake_minimum_required(VERSION 2.6)

message("# Creating Makefile for '${PROJECT_NAME}'")

IF(UNIX)
  IF(APPLE)
    set(CMAKE_MODULE_LINKER_FLAGS "-undefined dynamic_lookup")
  ELSE(APPLE)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
  ENDIF(APPLE)
ENDIF(UNIX)

add_subdirectory(ise)
add_subdirectory(benchmarks)
