# Pacman SDL Opengl

Just a small game I am writing to learn the basics of Opengl and new features of the latest C++ standards.
I plan also to use it to learn (the basics of) Vulkan.

For now, it works on the following platforms:
- Linux
- Windows

I intend to extend it to Android latter.

# Overall dependencies

To compile, you need a C++23 capable compiler and the following libraries:

- Opengl
- My-lib (https://github.com/ehmcruz/my-lib). The Makefile is configured to search **my-lib** in the same parent folder as this repository. If you put somewhere else, just modify the Makefile.

From language features, the requirement is actually C++20.  
But I am using some standard C++23 library features.

---

# Linux Guide

## Compiling in Linux

First, you need to download the following packages (considering you are using Ubuntu):

- libglew-dev
- libsdl2-dev

Then, to compile:

**make CONFIG_TARGET_LINUX=1**

## Running in Linux

**./pacman**

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

Then, to compile:

**make CONFIG_TARGET_WINDOWS=1**

Should be easy to compile in Microsoft compiler as well.

## Running in Windows

Just execute **pacman.exe**