Buidling the source codes
=========================

In the top of the repo (woolcano-ise/) type the following:
( mkdir build && cd build && cmake ../ise && make -j2)

This will build up the code under seperate dir (build/) than sources (ise/).

Doxygen documentation
=====================

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
