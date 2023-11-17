# Pacman SDL Opengl

Just a small game I am writing to learn the basics of Opengl and new features of the latest C++ standards.  
I plan also to use it to learn (the basics of) Vulkan.

For now, it works on the following platforms:
- Linux (SDL and Opengl)
- Windows (SDL only for now)
- Android (SDL only for now)

It is possible to compile in macOS and iOS as well, but since I don't have any Apple hardware, I could not try that.

## Overall dependencies

To compile, you need a C++23 capable compiler and the following tools and libraries:

- cmake
- SDL
- Opengl
- My-lib (https://github.com/ehmcruz/my-lib). Cmake is configured to search **my-lib** in the same parent folder as this repository. If you put somewhere else, just modify **CMakeLists.txt**.
- Boost (for Linux and Windows only). Sub-libraries: program options.

## Setting the compiler

To select the compiler to use, just pass it to **cmake**. For instance, for **clang**:

**cmake -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_C_COMPILER=clang ..**

Warning!    
The code was mostly tested in Linux with gcc (g++).    
I also tested with MSVC in Windows.    
MingW in Windows is presenting weird issues with cmake.

---

## Compiling guide

- Linux: [a relative link](linux.md)

- Windows: [a relative link](windows.md)

- Android: [a relative link](android.md)

---

# Known bugs

- When setting a high zoom level, the camera seems not to be focusing correctly when PACMAN is near the borders.
