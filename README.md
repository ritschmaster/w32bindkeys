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

## FAQ

### There is already auto hotkey. Why should I even bother to give w32bindkeys a try?

1. Auto hotkey is complicated and slow. Especially if you just want to open a terminal on <kbd>Win</kbd> + <kbd>Enter</kbd>.
2. If you are a Linux user you might already use xbindkeys. w32bindkeys helps you to bridge that gap to a frequent use of Windows.

### I want to remap the <kbd>Win</kbd> key

w32bindkeys supplies you with that functionality! But be aware that it is imossible to remap <kbd>Win</kbd> + <kbd>L</kbd>. The key must be disabled, but by doing so locking the PC will be disabled too.

You might be interested in this [solution](https://stackoverflow.com/questions/301053/re-assign-override-hotkey-win-l-to-lock-windows#answer-27975295) to that problem. It works like a charm.

### I need key bindings in my application too. Can I re-use parts of your project?

Not only can you re-use it, but you can actually rely on it directly via the optional library that is produced during the build process!

### Any cool hacks you know of?

<kbd>Win</kbd> is reserved in Windows for some special bindings. Like opening the Explorer by hitting <kbd>Win</kbd> + <kbd>E</kbd>. This can be annoying even if you do not use w32bindkeys. Almost every special binding can be disabled by doing the following:

1. Run `regedit`
2. Go to `Computer\HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Policies\Explorer\`
3. Create the DWORD `NoWinKeys` with the value `1`
4. Restart the computer
5. <kbd>Win</kbd> + <kbd>E</kbd> and friends are now disabled. The only exception is <kbd>Win</kbd> + <kbd>L</kbd>.


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

    ./configure --enable-install-library

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

    ./configure --host=x86_64-w64-mingw32 --enable-install-library


## Version scheme

The version scheme of w32bindkeys is as follows: x.y.z

* x is the stage:
  * < 0 is an unstable release
  * > 0 is a stable release
* y incidates changes to the API or the configuration file functionalities.
* z indicates additions without breaking any compatibility.

## Author

Richard Bäck <richard.baeck@mailbox.org>

## License

MIT License
