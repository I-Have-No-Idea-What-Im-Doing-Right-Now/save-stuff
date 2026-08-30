# save-stuff
A tool for quickly making backups

## Building
The build process uses CMake, so CMake must be installed to build the project
### On macOS
Build using CMake. The build process requires `xxd` for embedding files
```
mkdir build
cd build
cmake build ..
make
```
If you need to build for Windows on macOS, `mingw-w64` is required which can be installed using homebrew (`brew install mingw-w64`)

To build for windows:
```
mkdir build-windows
cd build-windows
cmake -DCMAKE_TOOLCHAIN_FILE=../windows-toolchain.cmake ..
make
```
### On Windows
You need `xxd` for the build process. The easiest way to get `xxd` on Windows is to install Vim (`winget install vim.vim`)

To build:
```
mkdir build
cd build
cmake ..
cmake --build . --config Release
```