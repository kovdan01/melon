$CMAKE_TOOL_BINARY_PATH\\cmake \
    -G Ninja \
    -S . \
    -B build \
    -D CMAKE_BUILD_TYPE=$1 \
    -D CMAKE_RANLIB=/$2/bin/ranlib.exe \
    -D CMAKE_VERBOSE_MAKEFILE=ON \
    -D CMAKE_MAKE_PROGRAM=/$2/bin/ninja.exe \
    -D CMAKE_C_COMPILER=/$2/bin/gcc.exe \
    -D CMAKE_CXX_COMPILER=/$2/bin/g++.exe \
    -D CMAKE_AR=/$2/bin/ar.exe \
    -D CMAKE_LINKER=/$2/bin/ld.exe \
    -D CMAKE_DLLTOOL=/$2/bin/dlltool.exe \
    -D CMAKE_NM=/$2/bin/nm.exe \
    -D CMAKE_PREFIX_PATH=\
    "/$2;\
    $ROOT_PATH/$BOOST_INSTALL_PATH;\
    $ROOT_PATH/$SQLPP11_INSTALL_PATH;\
    $ROOT_PATH/$CATCH2_INSTALL_PATH;\
    $REPO_PATH/3rd-party/lebedev-utils/prefix"
