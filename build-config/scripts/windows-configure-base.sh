#!/bin/bash

$CMAKE_TOOL_BINARY_PATH\\cmake \
    -G Ninja \
    -D CMAKE_RANLIB=/$MSYS_VARIANT/bin/ranlib.exe \
    -D CMAKE_VERBOSE_MAKEFILE=ON \
    -D CMAKE_MAKE_PROGRAM=/$MSYS_VARIANT/bin/ninja.exe \
    -D CMAKE_C_COMPILER=/$MSYS_VARIANT/bin/gcc.exe \
    -D CMAKE_CXX_COMPILER=/$MSYS_VARIANT/bin/g++.exe \
    -D CMAKE_AR=/$MSYS_VARIANT/bin/ar.exe \
    -D CMAKE_LINKER=/$MSYS_VARIANT/bin/ld.exe \
    -D CMAKE_DLLTOOL=/$MSYS_VARIANT/bin/dlltool.exe \
    -D CMAKE_NM=/$MSYS_VARIANT/bin/nm.exe \
    "$@"
