# Windows Guide

## Compiling in Windows

Compiling in Windows requires some effort.    
With MSYS2 would be easy, but some weird bugs were ocurring between MingW and Cmake when I tried to compile with MSYS2.    
Therefore I decided to use MSVC as default in Windows.

First, you need to install the compiler. I suggest downloading and installing Visual Studio Community Edition.

Afterwards, you need to download the libraries.

I warn that SDL2 and Boost may require some changes to cmake scripts depending on your system.

I myself had to make some hacks in this cmake repository's script because the standard cmake code was giving linking errors in boost.

### My-lib

Go to https://github.com/ehmcruz/my-lib and clone the repository.

The cmake script is configured to find My-lib in the same parent folder as this reposiory, such that the folder structure looks like this:

- /path/pacman-sdl-opengl
- /path/my-lib

### SDL2

Go to https://www.libsdl.org/ and download the pre-compiled development libraries for Windows.

I installed it in a folder called **c:\\my-msvc-libs**.

I tested with SDL2 2.28.5, but should work with other versions.

### Boost

Go to https://www.boost.org/ and download the pre-compiled development libraries for Windows.

I installed it in a folder called **c:\\my-msvc-libs**.

I tested with Boost 1.83.0, but should work with other versions.

### Compiling

To compile, first open a developer terminal in Windows.    
You cand do this by clicking in the Windows menu, and typing **x64**.    
It will appear something like **x64 Native Tools Command Prompt for VS**.    
Open it.

Then, go to this project repository within the terminal.

**mkdir build**    
**cd build**

- For SDL Renderer only: **cmake ..**

- To also support Opengl: I still need to add support in the cmake for Opengl in windows.

**cmake --build .**

And it is done!    
You should get an executable called **pacman.exe** in the **Debug** folder inside the build directory, along with a copy of the **shaders** folder and necessary dlls.

## Running in Windows

Just execute **pacman.exe**

For help: **pacman.exe --help**
