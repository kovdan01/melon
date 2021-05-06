$CMAKE_TOOL_BINARY_PATH\\cmake \
    -G Ninja \
    -S . \
    -B build \
    -D CMAKE_BUILD_TYPE=$BUILD_TYPE \
    -D CMAKE_RANLIB=/$MSYS_VARIANT/bin/ranlib.exe \
    -D CMAKE_VERBOSE_MAKEFILE=ON \
    -D CMAKE_MAKE_PROGRAM=/$MSYS_VARIANT/bin/ninja.exe \
    -D CMAKE_C_COMPILER=/$MSYS_VARIANT/bin/gcc.exe \
    -D CMAKE_CXX_COMPILER=/$MSYS_VARIANT/bin/g++.exe \
    -D CMAKE_AR=/$MSYS_VARIANT/bin/ar.exe \
    -D CMAKE_LINKER=/$MSYS_VARIANT/bin/ld.exe \
    -D CMAKE_DLLTOOL=/$MSYS_VARIANT/bin/dlltool.exe \
    -D CMAKE_NM=/$MSYS_VARIANT/bin/nm.exe \
    -D CMAKE_PREFIX_PATH=\
    "/$MSYS_VARIANT;\
    $ROOT_PATH/$BOOST_INSTALL_PATH;\
    $ROOT_PATH/$SQLPP11_INSTALL_PATH;\
    $ROOT_PATH/$CATCH2_INSTALL_PATH;\
    $REPO_PATH/3rd-party/lebedev-utils/prefix"
