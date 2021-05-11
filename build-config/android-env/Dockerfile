# Available as kovdan01/melon-android-env on Docker Hub

FROM archlinux:latest

RUN useradd builduser -m &&                                                                                           \
    passwd -d builduser &&                                                                                            \
    printf 'builduser ALL=(ALL) ALL\n' | tee -a /etc/sudoers &&                                                       \
    printf 'root ALL=(ALL) ALL\n'      | tee -a /etc/sudoers

RUN pacman -Sy --noconfirm &&                                                                                         \
    pacman -S archlinux-keyring --noconfirm &&                                                                        \
    pacman -S pacman-mirrorlist --noconfirm &&                                                                        \
    pacman -Su --noconfirm &&                                                                                         \
    pacman -S pacman-contrib wget sudo patch binutils make gcc pkg-config fakeroot cmake ninja fontconfig freetype2   \
              java-environment lib32-gcc-libs lib32-glibc libx11 libxext libxrender libxtst zlib --noconfirm &&       \
    paccache -r -k 0

WORKDIR "/home/builduser"

ADD . /home/builduser/archlinux-android-pkgs

RUN chown -R builduser archlinux-android-pkgs &&                                                                      \ 
    cd archlinux-android-pkgs/android-sdk &&                                                                          \
    sudo -u builduser bash -c 'makepkg -sri --noconfirm' &&                                                           \
    cd ../android-ndk &&                                                                                              \
    sudo -u builduser bash -c 'makepkg -sri --noconfirm' &&                                                           \
    cd ../android-pkg-config &&                                                                                       \
    sudo -u builduser bash -c 'makepkg -sri --noconfirm' &&                                                           \
    cd ../android-environment &&                                                                                      \
    sudo -u builduser bash -c 'makepkg -sri --noconfirm' &&                                                           \
    cd ../android-configure &&                                                                                        \
    sudo -u builduser bash -c 'makepkg -sri --noconfirm' &&                                                           \
    cd ../android-aarch64-libiconv &&                                                                                 \
    sudo -u builduser bash -c 'makepkg -sri --noconfirm' &&                                                           \
    cd ../android-aarch64-boost &&                                                                                    \
    sudo -u builduser bash -c 'makepkg -sri --noconfirm' &&                                                           \
    cd .. &&                                                                                                          \
    sudo -u builduser bash -c                                                                                         \
      'wget https://github.com/cyrusimap/cyrus-sasl/releases/download/cyrus-sasl-2.1.27/cyrus-sasl-2.1.27.tar.gz' &&  \
    sudo -u builduser bash -c 'tar -xf cyrus-sasl-2.1.27.tar.gz' &&                                                   \
    cd cyrus-sasl-2.1.27 &&                                                                                           \
    sudo -u builduser bash -c                                                                                         \
      'chmod +x ./configure &&                                                                                        \
      ./configure                                                                                                     \
        --prefix=/opt/android-libs/aarch64/                                                                           \
        --host=aarch64-linux-androideabi &&                                                                           \
      make &&                                                                                                         \
      sudo make install' &&                                                                                           \
    cd .. &&                                                                                                          \
    sudo -u builduser bash -c 'wget https://github.com/jbeder/yaml-cpp/archive/yaml-cpp-0.6.3.tar.gz' &&              \
    sudo -u builduser bash -c 'tar -xf yaml-cpp-0.6.3.tar.gz' &&                                                      \
    cd yaml-cpp-yaml-cpp-0.6.3 &&                                                                                     \
    sudo -u builduser bash -c                                                                                         \
      'cmake                                                                                                          \
        -G Ninja                                                                                                      \
        -D CMAKE_BUILD_TYPE=Release                                                                                   \
        -D CMAKE_C_COMPILER=gcc                                                                                       \
        -D CMAKE_CXX_COMPILER=g++                                                                                     \
        -D CMAKE_INSTALL_PREFIX=/opt/android-libs/aarch64/                                                            \
        -D CMAKE_ANDROID_NDK=/opt/android-ndk/                                                                        \
        -D CMAKE_SYSTEM_NAME=Android                                                                                  \
        -D CMAKE_SYSTEM_VERSION=23                                                                                    \
        -D CMAKE_ANDROID_ARCH_ABI=arm64-v8a                                                                           \
        -D YAML_BUILD_SHARED_LIBS=OFF                                                                                 \
        -D YAML_CPP_BUILD_TESTS=OFF . &&                                                                              \
      cmake --build . --target all &&                                                                                 \
      sudo cmake --build . --target install' &&                                                                       \
    cd ../.. &&                                                                                                       \
    rm -rf archlinux-android-pkgs                                                                                     \
           cyrus-sasl-2.1.27 cyrus-sasl-2.1.27.tar.gz                                                                 \
           yaml-cpp-yaml-cpp-0.6.3 yaml-cpp-0.6.3.tar.gz
