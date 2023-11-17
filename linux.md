# Linux Guide

## Compiling in Linux

First, you need to download the following packages (considering you are using Ubuntu):

- libglew-dev

- libsdl2-dev

- libboost-program-options-dev

You also need libraries not included in your distro:

- My-lib (https://github.com/ehmcruz/my-lib). Cmake is configured to search **my-lib** in the same parent folder as this repository. If you put somewhere else, just modify **CMakeLists.txt**.

Then, to compile:

**mkdir build**    
**cd build**

- For SDL Renderer only: **cmake ..**

- To also support Opengl: **cmake -DSUPPORT_OPENGL=ON ..**

**cmake --build .**

And it is done!    
You should get an executable called **pacman** in the build directory, along with a copy of the **shaders** folder.

## Running in Linux

**./pacman**

For help: **./pacman --help**
