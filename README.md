# w32bindkeys

![GitHub](https://img.shields.io/github/license/ritschmaster/w32bindkeys)
[![Join the chat at https://gitter.im/w32bindkeys/community](https://badges.gitter.im/w32bindkeys/community.svg)](https://gitter.im/w32bindkeys/community?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)
![GitHub All Releases](https://img.shields.io/github/downloads/ritschmaster/w32bindkeys/total)

xbindkeys for Windows

## Using w32bindkeys

### Using binary releases

1. Head over to the [Release](https://github.com/ritschmaster/w32bindkeys/releases) page and download the latest binary zip file.
2. Decompress it somwhere
3. Execute `w32bindkeys.exe`
4. An example configuration file was copied to your home directory named `.w32bindkeysrc`. Modify it to your needs!

### Using source code releases

1. Head over to the [Release](https://github.com/ritschmaster/w32bindkeys/releases) page and download the latest source zip file.
2. Decompress it somwhere
3. See [Compiling](#Compiling)
4. Execute `w32bindkeys.exe`
5. An example configuration file was copied to your home directory named `.w32bindkeysrc`. Modify it to your needs!

### Using the <kbd>Win</kbd> key

<kbd>Win</kbd> is reserved in Windows for some special bindings. Like opening the Explorer by hitting <kbd>Win</kbd> + <kbd>E</kbd>. This can be annoying but luckily w32bindkeys can overwrite those bindings. Additionally almost every special binding can be disabled by doing the following:

1. Run `regedit`
2. Go to `Computer\HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Policies\Explorer\`
3. Create the DWORD `NoWinKeys` with the value `1`
4. Restart the computer
5. <kbd>Win</kbd> + <kbd>E</kbd> and friends are now disabled. The only exception is <kbd>Win</kbd> + <kbd>L</kbd>.

It is not possible to overwrite <kbd>Win</kbd> + <kbd>L</kbd>. The key must be disabled, but by doing so locking the PC will be disabled too.

## Compiling

### Dependencies

* [MinGW](http://mingw.org/)
* [Collections-C](https://github.com/srdja/Collections-C)

### On Windows (64 bit) using Msys2

You will need a functioning MinGW environment. This sections describes the compilation using [Msys2](https://www.msys2.org/).

First install the dependencies:

    pacman -S make cmake automake autoconf mingw-w64-x86_64-gcc libtool

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

### On Fedora Linux (64 bit) for Windows (64 bit)

First install MinGW:

    sudo dnf install -y mingw64-gcc mingw64-gcc-c++ cmake automake autoconf autoconf-archive libtool

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

    export PKG_CONFIG_PATH=/usr/x86_64-w64-mingw32/lib/pkgconfig/
    ./configure --host=x86_64-w64-mingw32
    make

If you want to re-use the core functionality you may supply a special option to the `configure` script that will produce a dynamic installable library:

    ./configure --host=x86_64-w64-mingw32 --enable-install-base

## Version scheme

The version scheme of w32bindkeys is as follows: 1.2.3

1. Stages:
 0. Alpha
 1. Beta
 2. For Production use
2. Releases breaking compatibility of its API or the configuration file.
3. Collection of changes not breaking any compatibility.

## Author

Richard Bäck <richard.baeck@mailbox.org>

## License

MIT License
