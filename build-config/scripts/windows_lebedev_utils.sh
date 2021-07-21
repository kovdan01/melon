#!/bin/bash

self_dir=`dirname "$0"`

# ntc-cmake
cd $REPO_PATH/cmake/ntc
mkdir prefix

$self_dir/windows_configure_base.sh \
    -S . \
    -B build \
    -D CMAKE_INSTALL_PREFIX="$REPO_PATH/cmake/ntc/prefix"

$CMAKE_TOOL_BINARY_PATH\\cmake --build build --target install

# utils and asio-utils from cosec-examples
cd $REPO_PATH/3rd-party/lebedev-utils

$self_dir/windows_configure_base.sh \
    -S . \
    -B build \
    -D CMAKE_INSTALL_PREFIX="$ROOT_PATH/$LEBEDEV_UTILS_INSTALL_PATH" \
    -D CMAKE_PREFIX_PATH="$REPO_PATH/cmake/ntc/prefix;$ROOT_PATH/$BOOST_INSTALL_PATH" \
    -D CMAKE_BUILD_TYPE=Release

$CMAKE_TOOL_BINARY_PATH\\cmake --build build --target install
cd $REPO_PATH
