#!/bin/bash
set -e
GXX="${GXX:-g++}"
LIBSTDCXX_INCLUDES=$(
    echo |  ${GXX} -E -x c++ -v - \
         |& sed -n '/> search starts/,/End of search/p' \
         |  sed '1d;$d' \
         |  while read ; do
                DIR="$(realpath $(echo "$REPLY" | sed -e 's/ *//'))"
                if [ -f "${DIR}/cxxabi.h" -o -f "${DIR}/bits/c++config.h" ] ; then
                    echo "${DIR}"
                fi
            done
)
LIBSTDCXX_INCLUDES="${LIBSTDCXX_INCLUDES/$'\n'/;}"
DELIMS="${LIBSTDCXX_INCLUDES//[^;]}"
if [ ${#DELIMS} != 1 ] ; then
    echo "Couldn't determine two directories with needed includes!"
    exit 1
fi
cmake -G Ninja \
      -DCMAKE_C_COMPILER="$(${GXX} -print-prog-name=gcc)" \
      -DCMAKE_CXX_COMPILER="${GXX}" \
      -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_CXX_STANDARD=17 \
      -DLLVM_ENABLE_PROJECTS="clang;clang-tools-extra" \
      -DLLVM_ENABLE_RUNTIMES="libcxx" \
      -DLLVM_BUILD_LLVM_DYLIB=ON \
      -DLLVM_LINK_LLVM_DYLIB=ON \
      -DLLVM_TARGETS_TO_BUILD=X86 \
      -DLLVM_ENABLE_BINDINGS=OFF \
      -DLLVM_INSTALL_TOOLCHAIN_ONLY=ON \
      -DENABLE_EXPERIMENTAL_NEW_PASS_MANAGER=ON \
      -DCLANG_LINK_CLANG_DYLIB=ON \
      -DCLANG_DEFAULT_CXX_STDLIB=libc++ \
      -DCLANG_DEFAULT_OBJCOPY=llvm-objcopy \
      -DCLANG_ENABLE_ARCMT=OFF \
      -DLIBCXX_ABI_UNSTABLE=ON \
      -DLIBCXX_CXX_ABI=libstdc++ \
      -DLIBCXX_CXX_ABI_INCLUDE_PATHS="${LIBSTDCXX_INCLUDES}" \
      -DLIBCXX_ENABLE_STATIC=OFF \
      -DLIBCXX_ENABLE_FILESYSTEM=ON \
      -DLIBCXX_INCLUDE_BENCHMARKS=OFF \
      -DLIBCXX_INCLUDE_DOCS=OFF \
      -DLIBCXX_INCLUDE_TESTS=OFF \
      "$@"
