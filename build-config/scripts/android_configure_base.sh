#!/bin/bash

cmake                                                                       \
    -G Ninja                                                                \
    -D CMAKE_PREFIX_PATH="/opt/android-libs/aarch64/;$CMAKE_PREFIX_PATH"    \
    -D CMAKE_FIND_ROOT_PATH="/opt/android-libs/aarch64;$CMAKE_PREFIX_PATH"  \
    -D CMAKE_ANDROID_NDK=/opt/android-ndk/                                  \
    -D CMAKE_SYSTEM_NAME=Android                                            \
    -D CMAKE_SYSTEM_VERSION=23                                              \
    -D CMAKE_ANDROID_ARCH_ABI=arm64-v8a                                     \     
    "$@"
