project (woolcano-ise C CXX)

cmake_minimum_required(VERSION 2.6)

message("# Creating Makefile for '${PROJECT_NAME}'")

add_subdirectory(ise)
add_subdirectory(benchmarks)
