# IzInjector

[![Checks](https://img.shields.io/github/check-runs/Iswenzz/IzInjector/master?logo=github)](https://github.com/Iswenzz/IzInjector/actions)
[![CodeFactor](https://img.shields.io/codefactor/grade/github/Iswenzz/IzInjector?label=codefactor&logo=codefactor)](https://www.codefactor.io/repository/github/iswenzz/izinjector)
[![CodeCov](https://img.shields.io/codecov/c/github/Iswenzz/IzInjector?label=codecov&logo=codecov)](https://codecov.io/gh/Iswenzz/IzInjector)
[![License](https://img.shields.io/github/license/Iswenzz/IzInjector?color=blue&logo=gitbook&logoColor=white)](https://github.com/Iswenzz/IzInjector/blob/master/LICENSE)

This application is intended to allow users to inject a Dynamic-Link Library (DLL) file into another process in memory. Note that this is meant to be used for educational purposes only, and I do not condone its use for cheating of any kind.

## Command Line Interface

```
Usage: IzInjector --mode INJECT --name notepad.exe [DLLs Paths]

        --help                    Display the program help message
        --version                 Display the program version
        -m, --mode=               DLL Injection mode [INJECT or EJECT]
        -n, --name=               The target process name [notepad.exe]
        -p, --pid=                The target process ID
        <files>                   DLL Files
```

## Building

This project builds with [CMake](https://cmake.org/) and [vcpkg](https://vcpkg.io/).

### x64

```sh
mkdir build64 && cd build64
cmake .. --preset x64-windows
cmake --build .
```

### x86

```sh
mkdir build32 && cd build32
cmake .. --preset x86-windows
cmake --build .
```

### [Download](https://github.com/Iswenzz/IzInjector/releases)

## Package

This package is available for use via [vcpkg-registry](https://github.com/Iswenzz/vcpkg-registry).

## Contributors:

**_Note:_** If you would like to contribute to this repository, feel free to send a pull request, and I will review your code.
Also feel free to post about any problems that may arise in the issues section of the repository.
