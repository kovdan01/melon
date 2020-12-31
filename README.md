# Melon

A federated messenger.

[![Build Linux & macOS](https://github.com/kovdan01/melon/workflows/Build%20Linux%20&%20macOS/badge.svg)](https://github.com/kovdan01/melon/actions?query=workflow%3A%22Build+Linux+%26+MacOS%22+branch%3Adevelop)
[![Build Windows](https://github.com/kovdan01/melon/workflows/Build%20Windows/badge.svg)](https://github.com/kovdan01/melon/actions?query=workflow%3A%22Build+Windows%22+branch%3Adevelop)
[![Build Android](https://github.com/kovdan01/melon/workflows/Build%20Android/badge.svg)](https://github.com/kovdan01/melon/actions?query=workflow%3A%22Build+Android%22+branch%3Adevelop)

## How to build

### Common info

Core libraries, server services and desktop client are written in C++ and are built via CMake. Building server services and desktop client can be disabled with passing the following CMake options during configuration: `-DMELON_BUILD_SERVER=OFF -DMELON_BUILD_DESKTOP_CLIENT=OFF`.

Android application is written in Java and Kotlin and uses core libraries via JNI.

Here we provide brief build instructions, however, if it is not enough for you, full build scripts are available as GitHub Actions configuration files in [`.github/workflows`](.github/workflows).

### Unix

1. Install cmake 3.19 or newer and a compiler with C++20 support.
   Also, PkgConfig is needed to find some libraries.

2. Install dependency libraries: boost 1.74.0 or newer, cyrus-sasl 2.1.27 or newer, yaml-cpp 0.6.3 or newer, qt 5.15.1 or newer.

   **In Arch Linux:**

   ```bash
   sudo pacman -S boost boost-libs
   sudo pacman -S libsasl
   sudo pacman -S yaml-cpp
   sudo pacman -S qt5-base qt5-tools
   ```

   Note that boost 1.74.0 (as well as some other libraries) is not available in some linux distributions yet, that's why you might want to build such dependencies from sources. See corresponding GitHub Actions configuration files containing build scripts and do not forget to add custom prefixes to `CMAKE_PREFIX_PATH` later.

3. Execute the following commands (delete `-G Ninja` if ninja generator is not available):

   ```bash
   mkdir build && cd build
   cmake \
     -G Ninja \
     -D CMAKE_BUILD_TYPE=Release \
     /path/to/repository/root
   cmake --build . --target all
   ```

4. If you want to run tests, execute `ctest` command after a successful build.

### Windows

Building under Windows is done under MSYS2 MinGW64. Build instructions are identical with the Unix ones.

### Android

#### Core libraries

To build core libraries for aarch64 (or any other supported Android architecture) you need to install Android NDK and build all the dependencies (see building under Unix) with a corresponding cross-compiler. If you don't want to deal with this mess, a Docker image [`kovdan01/melon-android-env`](https://hub.docker.com/repository/docker/kovdan01/melon-android-env/) with prebuilt dependencies for aarch64 is provided. The corresponding Dockerfile can be found in the repository in [`build-config/android-env/Dockerfile`](build-config/android-env/Dockerfile). Further build commands can be found in [`build-config/android/Dockerfile`](build-config/android/Dockerfile)

#### Application

Assuming current directory is `src/client-andorid`, run:

```bash
gradle wrapper
./gradlew :app:packageDebugUniversalApk
```

An APK file will appear: `app/build/outputs/universal_apk/debug/app-debug-universal.apk`.
