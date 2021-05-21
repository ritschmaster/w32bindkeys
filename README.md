# w32bindkeys

![GitHub](https://img.shields.io/github/license/ritschmaster/w32bindkeys)
[![Telegram chat](https://img.shields.io/badge/chat-%40w32bindkeys-%2335ADE1?logo=telegram)](https://t.me/w32bindkeys)
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

### Are there known limitations?

Yes, there is: w32bindkeys uses the [WIN32 API Low Level Keyboard Hook](https://docs.microsoft.com/en-us/previous-versions/windows/desktop/legacy/ms644985(v=vs.85)). The nature of this hook is that the registered function (i.e. processing w32bindkeys key bindings) will be terminated if its execution speed is too slow. This happens without any notification and therefore the developer cannot check if his hook will be removed. Normally this won't happen, but if many key bindings are registered then this might occur. To avoid this issue w32bindkeys applies the following strategies:

1. The key bindings are segmented on currently 30 different hooks. As every hook will receive its own time slot this helps to avoid the removal of the hook.
2. Each hook tracks by itself if it has been successfully left after entrance. This allows w32bindkeys to notice if a hook has been removed and will reset the tracked keys.
3. After a certain amount of time the tracked keys will be reset. This is especially important if the CPU is under heavy load.

Points 2. and 3. will result in losing the track of already pressed keys. Example: Imagine you have bound <kbd>Win</kbd> + <kbd>return</kbd> to opening a command prompt. Then you press <kbd>Win</kbd> for 15 seconds and then finally press <kbd>return</kbd>. The command prompt won't show up as the tracked keys have already been reset in the mean time. But pressing both keys at once of course will open the command prompt.

**If you have any better solution to this issue, please contact me or open a pull request.**

### I want to remap the <kbd>Win</kbd> key

w32bindkeys supplies you with that functionality! But be aware that it is imossible to remap <kbd>Win</kbd> + <kbd>L</kbd>. The key must be disabled, but by doing so locking the PC will be disabled too.

You might be interested in this [solution](https://stackoverflow.com/questions/301053/re-assign-override-hotkey-win-l-to-lock-windows#answer-27975295) to that problem. It works like a charm.

### I need key bindings in my application too. Can I re-use parts of your project?

Not only can you re-use it, but you can actually rely on it directly via the optional library that is produced during the build process!

You might want to have a look at (copy parts of) `src/main.c` to enable w32bindkeys for your application as fast as possible.

### Any cool hacks you know of?

<kbd>Win</kbd> is reserved in Windows for some special bindings. Like opening the Explorer by hitting <kbd>Win</kbd> + <kbd>E</kbd>. This can be annoying even if you do not use w32bindkeys. Almost every special binding can be disabled by doing the following:

1. Run `regedit`
2. Go to `Computer\HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Policies\Explorer\`
3. Create the DWORD `NoWinKeys` with the value `1`
4. Restart the computer
5. <kbd>Win</kbd> + <kbd>E</kbd> and friends are now disabled. The only exception is <kbd>Win</kbd> + <kbd>L</kbd> (see fix above).


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
  * \> 0 is a stable release
* y incidates changes to the API or the configuration file functionalities.
* z indicates additions without breaking any compatibility.

## Author

Richard Bäck <richard.baeck@mailbox.org>

## License

MIT License
