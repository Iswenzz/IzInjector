# IzInjector
[![Build status](https://ci.appveyor.com/api/projects/status/cert46mc51ly0n37?svg=true)](https://ci.appveyor.com/project/Iswenzz/izinjector)
[![CodeFactor](https://www.codefactor.io/repository/github/iswenzz/izinjector/badge)](https://www.codefactor.io/repository/github/iswenzz/izinjector)
[![codecov](https://codecov.io/gh/Iswenzz/IzInjector/branch/master/graph/badge.svg?token=7lUS3DA3mx)](https://codecov.io/gh/Iswenzz/IzInjector)
[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)

This application is intended to allow users to inject a Dynamic-Link Library (DLL) file into another process in memory. Note that this is meant to be used for educational purposes only, and I do not condone its use for cheating of any kind.

## Command Line Interface
```
Usage: IzInjector --mode INJECT --name notepad.exe [DLLs Paths]

        --help                    Display the program help message
        --version                 Display the program version
        -v, --verbose             Log the command process
        -m, --mode=               DLL Injection mode [INJECT or EJECT]
        -n, --name=               The target process name [notepad.exe]
        -p, --pid=                The target process ID
        <files>                   DLL Files
```

## Building
This project builds with [CMake](https://cmake.org/) and [Conan](https://conan.io/).

### x64
```sh
mkdir build64 && cd build64
conan install ..
cmake ..
cmake --build .
```

### x86
```sh
mkdir build32 && cd build32
conan install .. --build=argtable3 -s arch=x86
cmake .. -A Win32
cmake --build .
```
    
### [Download](https://github.com/Iswenzz/IzInjector/releases)

## Contributors:
***Note:*** If you would like to contribute to this repository, feel free to send a pull request, and I will review your code. Also feel free to post about any problems that may arise in the issues section of the repository.
