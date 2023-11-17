# Android Guide

## Getting the tools and libraries

You should be able to compile this project using either Linux or Windows.
Since I prefer Linux, I will explain how to compile using it.
I used Ubuntu 22.04 LTS.

First, you need to install the Android SDK and Android NDK.
I suggest you to download them using Android Studio SDK Manager.
When I compiled, I used the following versions:

- Android SDK API level 34

- Android NDK 26.1.10909125

I believe there would no issues compiling using different versions, since I haven't used anything special.

You are also going to need:

- cmake, which you can install using your Linux distribution package manager.

- OpenJDK, as far as I remember, I had to install version 17.

- SDL2 Library source files (I used version 2.28.5).

## Compiling in Android

First, you actually need to compile SDL2. I will leave a link to guide I followed to do that: [a link](https://github.com/AlexanderAgd/SDL2-Android).

After you have the compiled **libSDL2.so**, you may need to change the include and link directories in **CMakeLists.txt**.

You may also need to take a look at **android/build.gradle**.

Then, to compile:

**cd android**    

**./gradlew build**

To install in your device:

**./gradlew installDebug**

And it is done!    

## Running in Android

Just execute the app called **Game** with the SDL logo.

## Pairing a device

Gradlew requires an Android device to be paired to install the apk.
To do that, follow these steps:

For Wifi debugging:

1. Enable Developer Options in your Android device.
1. Enable Wifi debugging.
1. Enter Wifi debugging options.
1. Click on pair your device using a pairing code.
1. Pair your device using `adb pair ip:port`
1. Connect your device using `adb connect ip:port`

It is also possible to use a USB cable instead of Wifi, but I haven't tried that.
