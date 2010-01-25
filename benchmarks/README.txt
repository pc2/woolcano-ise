Buidling the source codes
=========================

There are two build systems provided: CMake & automake
The CMake build system is preffered.

To get the results do the following:
( mkdir build && cd build && cmake ../ise && make -j2)

This will build up the sources (in build/) and run profiler on them.


Creating CMakeFiles.txt
=========================
Have a look on bottom of the "enable_llvm_toolflow.cmake" file to get the template for the CMakeLists.txt file.
