Buidling the source codes
=========================

In the top of the repo (woolcano-ise/) type the following:
( mkdir build && cd build && cmake .. && make -j2 test_ise)

This will build up the code under seperate dir (build/),
next it will compile the ISE pass,
than it will compile and profile the benchmarking applications,
finally it will run the ISE pass on top of each application.

Use command: make -j2 test_ise_stats to obtain detailed stats about each step.
Use command: make test_ise_bench to benchmark ISE algos

For more informations about running the code have a look at: 
docs/notes/running.txt

LLVM 
=========================

Use "ise_frozen" tag from llvm-mirror git repository.
This corresponds to commit:
$ git rev-parse ise_frozen
0940bfd7b5f49436c17c5dc4a8534f90c5a9d02b

This code is based on LLVM 2.5 and should be used with the LLVM-GCC 2.5
front-end.


Doxygen documentation
=========================

1. Get doxygen for MAC: ftp://ftp.stack.nl/pub/users/dimitri/Doxygen-1.6.2.dmg
2. Install it to the /Applications folder
3. Make doxy available by adding it to the PATH:
    PATH=$PATH:/Applications/Doxygen.app/Contents/Resources/
-   or by linking the files to the directory which is in the PATH:
    ln -s /Applications/Doxygen.app/Contents/Resources/dox* /usr/local/bin
4. Run doxygen
    cd woolcano-ise
    doxygen doxygen.cfg
-  this will generate the doxygen docs under the woolcano-ise/doxygen/ dir.


LLVM Profiler
=========================

In default build process profiler is not compiled.
This command builds profiler library necessary for the profile.pl script.

make -C `llvm-config --obj-root`/runtime


Upgrading to OSX 10.6
=========================

Cmake by default on 10.6 OSX will create 64bit executables (O-Mach 64).
The pass for the 'opt' tool has to be in 32 bit format (O-Mach).
To generate 32 bit executables One needs to setup environmental variables:
 CXXFLAGS="-m32"
 CFLAGS="-m32"
