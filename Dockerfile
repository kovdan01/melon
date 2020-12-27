FROM kovdan01/melon-android-env:latest

ADD . /home/builduser/repo

RUN cd repo && \
    cmake -S . -B build -D CMAKE_BUILD_TYPE=Release -D CMAKE_PREFIX_PATH=/opt/android-libs/aarch64/ -G Ninja -D CMAKE_ANDROID_NDK=/opt/android-ndk/ -D CMAKE_SYSTEM_NAME=Android -D CMAKE_SYSTEM_VERSION=23 -D CMAKE_ANDROID_ARCH_ABI=arm64-v8a -D MELON_BUILD_SERVER=OFF -D MELON_BUILD_DESKTOP_CLIENT=OFF && \
    cmake --build build --target all && \
    cmake --install build --prefix install --strip
