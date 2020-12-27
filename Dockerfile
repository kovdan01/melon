FROM kovdan01/melon-android-env:latest

ADD . /home/builduser/repo

RUN cd repo && \
    cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=/opt/android-libs/aarch64/ -GNinja -DCMAKE_ANDROID_NDK=/opt/android-ndk/ -DCMAKE_SYSTEM_NAME=Android CMAKE_SYSTEM_VERSION=23 -DCMAKE_ANDROID_ARCH_ABI=arm64-v8a -D MELON_BUILD_SERVER=OFF -D MELON_BUILD_DESKTOP_CLIENT=OFF && \
    cmake --build build --target all && \
    cmake --install build --prefix install --strip
