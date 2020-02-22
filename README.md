xbindkeys for Windows

# Dependencies

* [MinGW](http://mingw.org/)
* [Collections-C](https://github.com/srdja/Collections-C)

# Compiling

# On Windows using Msys2

You will need a functioning MinGW environment. This sections describes the compilation using [Msys2](https://www.msys2.org/).

First install the dependencies:

    pacman -S gcc make cmake automake autoconf mingw-w64-x86_64-gcc

Use the __MSYS2 MinGW 64-bit__ Terminal for the next actions and everytime you want to re-compile w32bindkeys.

Then compile and install Collections-C:

    wget https://github.com/srdja/Collections-C/archive/master.zip
    unzip master.zip
    cd Collections-C-master/
    sed -i s/add_subdirectory\(test\)//g CMakeLists.txt # Testing is not needed
    mkdir build
    cd build
    cmake -DCMAKE_INSTALL_PREFIX=/mingw64 ..

Then compile w32bindkeys:

    export PKG_CONFIG_PATH=$PKG_CONFIG_PATH:/mingw64/lib/pkgconfig
    ./autogen.sh
    make

If you want to re-use the core functionality you may supply a special option to the `configure` script that will produce a dynamic installable library:

    ./configure --enable-install-base

Afterwards you may install w32bindkeys by performing:

    make install

## On Fedora Linux (64 bit)

First install MinGW:

    sudo dnf install -y mingw64-gcc mingw64-gcc-c++ mingw64-winpthreads-static cmake automake autoconf autoconf-archive libtool

Then compile and install Collections-C:

    wget https://github.com/srdja/Collections-C/archive/master.zip
    unzip master.zip
    cd Collections-C-master/
    sed -i s/add_subdirectory\(test\)//g CMakeLists.txt # Testing is not needed
    mkdir build
    cd build
    mingw64-cmake ..
    sudo make install
    sudo cp /usr/x86_64-w64-mingw32/sys-root/mingw/lib/pkgconfig/collectionc.pc /usr/lib64/pkgconfig

Then compile w32bindkeys:

    mingw64-configure
    make

If you want to re-use the core functionality you may supply a special option to the `configure` script that will produce a dynamic installable library:

    ./configure --enable-install-base

