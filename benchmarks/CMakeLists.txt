project (bench-all C CXX)

cmake_minimum_required(VERSION 2.6)
cmake_policy(SET CMP0011 NEW) 
cmake_policy(SET CMP0002 OLD) 

message("# Creating Makefile for '${PROJECT_NAME}'")

ADD_SUBDIRECTORY(adpcm)
ADD_SUBDIRECTORY(aes)
ADD_SUBDIRECTORY(blowfish)
ADD_SUBDIRECTORY(cjpeg)
ADD_SUBDIRECTORY(fft)
ADD_SUBDIRECTORY(md5)
ADD_SUBDIRECTORY(sha)
ADD_SUBDIRECTORY(sor)
ADD_SUBDIRECTORY(whetstone)
