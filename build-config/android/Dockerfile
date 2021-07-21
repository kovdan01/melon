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
    export CMAKE_PREFIX_PATH="$REPO_PATH/cmake/ntc/prefix" &&                 \
    $REPO_PATH/build-config/scripts/android-configure-base.sh                 \
        -S ..                                                                 \
        -D CMAKE_INSTALL_PREFIX=`pwd`/../prefix                               \
        -D CMAKE_BUILD_TYPE=Release &&                                        \
    cmake --build . --target all &&                                           \
    cmake --build . --target install &&                                       \
    cd $REPO_PATH &&                                                          \
    cd 3rd-party/msgpack-c &&                                                 \
    mkdir build &&                                                            \
    mkdir prefix &&                                                           \
    cd build &&                                                               \
    export CMAKE_PREFIX_PATH="" &&                                            \
    $REPO_PATH/build-config/scripts/android-configure-base.sh                 \
        -S ..                                                                 \
        -D CMAKE_INSTALL_PREFIX=`pwd`/../prefix                               \
        -D CMAKE_BUILD_TYPE=Release &&                                        \
    cmake --build . --target all &&                                           \
    cmake --build . --target install &&                                       \
    cd $REPO_PATH &&                                                          \
    export CMAKE_PREFIX_PATH="$REPO_PATH/3rd-party/lebedev-utils/prefix;$REPO_PATH/3rd-party/msgpack-c/prefix" && \
    $REPO_PATH/build-config/scripts/android-configure-base.sh                 \
        -S .                                                                  \
        -B build                                                              \
        -D CMAKE_BUILD_TYPE=Release                                           \
        -D MELON_BUILD_SERVER=OFF                                             \
        -D MELON_BUILD_DESKTOP_CLIENT=OFF &&                                  \
    cmake --build build --target all &&                                       \
    cmake --install build --prefix install --strip
