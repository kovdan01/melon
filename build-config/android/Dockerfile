# Build context is assumed to be in the repository root

FROM kovdan01/melon-android-env:latest

ADD . /home/builduser/repo

RUN cd repo &&                                                                \
    REPO_PATH=`pwd` &&                                                        \
    cd cmake/ntc &&                                                           \
    mkdir build &&                                                            \
    mkdir prefix &&                                                           \
    cd build &&                                                               \
    cmake -DCMAKE_INSTALL_PREFIX=`pwd`/../prefix .. &&                        \
    cmake --build . --target all &&                                           \
    cmake --build . --target install &&                                       \
    cd $REPO_PATH &&                                                          \
    cd 3rd-party/lebedev-utils &&                                             \
    mkdir build &&                                                            \
    mkdir prefix &&                                                           \
    cd build &&                                                               \
    cmake                                                                     \
        -G Ninja                                                              \
        -D CMAKE_INSTALL_PREFIX=`pwd`/../prefix                               \
        -D CMAKE_BUILD_TYPE=Release                                           \
        -D CMAKE_PREFIX_PATH="$REPO_PATH/cmake/ntc/prefix;/opt/android-libs/aarch64/" \
        $CMAKE_LINKER_FLAGS .. &&                                             \
    cmake --build . --target all &&                                           \
    cmake --build . --target install &&                                       \
    cd $REPO_PATH &&                                                          \
    cmake                                                                     \
        -S .                                                                  \
        -B build                                                              \
        -G Ninja                                                              \
        -D MELON_BUILD_SERVER=OFF                                             \
        -D MELON_BUILD_DESKTOP_CLIENT=OFF                                     \
        -D CMAKE_BUILD_TYPE=Release                                           \
        -D CMAKE_PREFIX_PATH=/opt/android-libs/aarch64/                       \
        -D CMAKE_ANDROID_NDK=/opt/android-ndk/                                \
        -D CMAKE_SYSTEM_NAME=Android                                          \
        -D CMAKE_SYSTEM_VERSION=23                                            \
        -D CMAKE_ANDROID_ARCH_ABI=arm64-v8a &&                                \
    cmake --build build --target all &&                                       \
    cmake --install build --prefix install --strip
