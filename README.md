xbindkeys for Windows

# Dependencies

* [MinGW](http://mingw.org/)
* [Collections-C](https://github.com/srdja/Collections-C)

# Compiling

You will need a functioning MinGW environment. I suggest using [Msys2](https://www.msys2.org/).

First compile and install Collections-C:

    wget https://github.com/srdja/Collections-C/archive/master.zip
    unzip master.zip
    cd Collections-C-master/
    sed -i s/add_subdirectory\(test\)//g CMakeLists.txt # Testing is not needed
    mkdir build
    cd build
    cmake -DCMAKE_INSTALL_PREFIX=/usr ..

Then compile w32bindkeys:

    ./configure
    make

If you want to re-use the core functionality you may supply a special option to the `configure` script that will produce a dynamic installable library:

    ./configure --enable-install-base

Afterwards you may install w32bindkeys - depending on your MinGW environment - by performing:

    make install
