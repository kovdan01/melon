# Melon

A federated messenger.

[![Build all](https://github.com/kovdan01/melon/workflows/Build%20all/badge.svg)](https://github.com/kovdan01/melon/actions?query=workflow%3A%22Build+all%22+branch%3Adevelop)

## How to build

1. Install cmake 3.16 or newer and a compiler with C++20 support.
   Also, PkgConfig is needed to find some libraries.

2. Install dependency libraries: boost, message pack, cyrus-sasl, qt5.

   **In Ubuntu:**

   ```bash
   sudo apt-get install libboost-all-dev
   sudo apt-get install libmsgpackc2 libmsgpack-dev
   sudo apt-get install libsasl2-dev
   sudo apt-get install qt5-default qttools5-dev
   ```

   **In Arch Linux:**

   ```bash
   sudo pacman -S boost boost-libs
   sudo pacman -S msgpack-c
   sudo pacman -S libsasl
   sudo pacman -S qt5-base qt5-tools
   ```

3. Execute the following commands:

   ```bash
   $ mkdir build && cd build
   $ cmake /path/to/repository/root
   $ cmake --build . --target all
   ```

4. If you want to run tests, execute `ctest` command after a successful build.
