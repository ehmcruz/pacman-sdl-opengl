# Pacman SDL Opengl

Just a small game I am writing to learn the basics of Opengl and new features of the latest C++ standards.  
I plan also to use it to learn (the basics of) Vulkan.

For now, it works on the following platforms:
- Linux
- Windows

I intend to extend it to Android latter.

# Overall dependencies

To compile, you need a C++23 capable compiler and the following tools and libraries:

- cmake
- SDL
- Opengl
- My-lib (https://github.com/ehmcruz/my-lib). Cmake is configured to search **my-lib** in the same parent folder as this repository. If you put somewhere else, just modify the Makefile.

## Setting the compiler

To select the compiler to use, just pass it to **cmake**. For instance, for **clang**:

**cmake -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_C_COMPILER=clang ..**

Warning!    
The code was mostly tested with gcc (g++) and mingw.

---

# Linux Guide

## Compiling in Linux

First, you need to download the following packages (considering you are using Ubuntu):

- libglew-dev
- libsdl2-dev
- libboost-program-options-dev

Then, to compile:

**mkdir build**    
**cd build**

- For SDL Renderer only: **cmake ..**

- To also support Opengl: **cmake -DSUPPORT_OPENGL=ON ..**

**make**

And it is done!    
You should get an executable called **pacman** in the build directory, along with a copy of the **shaders** folder.

## Running in Linux

**./pacman**

For help: **./pacman --help**

---

# Windows Guide

## Compiling in Windows

I used the MSYS2 toolchain with Mingw64 and UCRT64 to compile in Windows.  
Honestly, that is a wonderful toolchain for people like myself that is more used to program in Linux.

You will need the following packages in MSYS2:

- mingw-w64-ucrt-x86_64-gcc
- make
- mingw-w64-ucrt-x86_64-SDL2
- mingw-w64-ucrt-x86_64-pkg-config
- mingw-w64-ucrt-x86_64-glfw (I need to confirm if this one is actually required)
- mingw-w64-ucrt-x86_64-glew
- mingw-w64-ucrt-x86_64-boost

Then, to compile:

- For SDL Renderer only: **make CONFIG_TARGET_WINDOWS=1**
- To also support Opengl: **make CONFIG_TARGET_WINDOWS=1 PACMAN_SUPPORT_OPENGL=1**

Should be easy to compile in Microsoft compiler as well.

## Running in Windows

Just execute **pacman.exe**

For help: **pacman.exe --help**

---

# Known bugs

- When setting a high zoom level, the camera seems not to be focusing correctly when PACMAN is near the borders.
