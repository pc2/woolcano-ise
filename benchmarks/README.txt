Buidling the source codes
=========================
There are two build systems provided for building the benchmark apps: CMake & automake
The CMake build system is preffered.
To compile the benchmarking apps type the following:
( mkdir build && cd build && cmake .. && make -j2)
This will build up the sources (in build/).
In addition it will use profiler to profile them.
The main application has always the same name as directory_name under which it is located.
The profiling results are stored into the <directory_name>_prof.bc

To get profiling results under the automake use the ./profile_all.pl script.

Testing with the ISE pass
=========================
Once the applications are built one can use them as case studies for the ISE pass.
In order to do that use command: make test_ise 
This will do the following:
1. find the ISE pass
2. load it to the optimizer (opt)
3. run the optimizer with ISE for each app

Each application can be tested seperatetly.
In such case type: make <directory_name>_ise
i.e to test the sor application type: make sor_ise



Porting other apps.
===================
All apps are compiled with the LLVM compiler.
This is achieved with two build systems: CMake & automake.
The first one is preffered.
In order to add other benchmarking apps you should use CMake.
There is enable_llvm_tooflow.cmake file which helps with that process.
Have a look on ../cmake/examples/enable_llvm_toolflow_example.cmake to build up example app with it.
