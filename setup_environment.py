#!/bin/python

import os
import shutil
import sys

current = os.path.abspath(os.path.dirname(os.path.realpath(__file__)))
sys.path.append(os.path.join(current, "3rd-party", "cpp-deps-automation"))

from builder import Builder, execute_command


def delete_dir_if_exists(directory):
    if os.path.isdir(directory):
        shutil.rmtree(directory)


def dependency_build_and_prefix(label):
    os.mkdir(label)
    dependency_path = os.path.abspath(label)
    dependency_build_path = os.path.join(dependency_path, "build")
    dependency_prefix_path = os.path.join(dependency_path, "prefix")
    return (dependency_build_path, dependency_prefix_path)


class MelonBuilder(Builder):
    def __init__(self, repo_path):
        self.repo_path = repo_path
        Builder.__init__(self)


    def build_ntc_cmake(self):
        build_path, prefix_path = dependency_build_and_prefix("ntc-cmake")
        source_dir = os.path.join(self.repo_path, "cmake", "ntc")
        self.prefixes["ntc-cmake"] = self.build_cmake(source_dir=source_dir,
                                                      build_dir=build_path,
                                                      prefix_dir=prefix_path)


    def build_lebedev_utils(self):
        build_path, prefix_path = dependency_build_and_prefix("lebedev-utils")
        source_dir = os.path.join(self.repo_path, "3rd-party", "lebedev-utils")
        ntc_cmake_prefix = self.get_prefix("ntc-cmake")
        boost_prefix = self.get_prefix("boost")
        cmake_params = '-D CMAKE_PREFIX_PATH="{};{}"'.format(ntc_cmake_prefix, boost_prefix)
        self.prefixes["lebedev-utils"] = self.build_cmake(source_dir=source_dir,
                                                          cmake_params=cmake_params,
                                                          build_dir=build_path,
                                                          prefix_dir=prefix_path)


    def build_msgpack(self):
        build_path, prefix_path = dependency_build_and_prefix("msgpack-c")
        source_dir = os.path.join(self.repo_path, "3rd-party", "msgpack-c")
        cmake_params = '-D MSGPACK_BUILD_TESTS=OFF -D CMAKE_PREFIX_PATH="{}"'.format(self.get_prefix("boost"))
        self.prefixes["msgpack-c"] = self.build_cmake(source_dir=source_dir,
                                                      cmake_params=cmake_params,
                                                      build_dir=build_path,
                                                      prefix_dir=prefix_path)


def build_dependencies(builder):
    deps_path = os.path.join(current, ".deps")
    delete_dir_if_exists(deps_path)
    os.mkdir(deps_path)
    os.chdir(deps_path)

    builder.download_cmake()
    builder.download_ninja()
    builder.build_yaml_cpp()
    builder.build_cyrus_sasl()
    builder.build_date()
    builder.build_sqlpp11()
    builder.build_sqlpp11_mysql()
    builder.build_catch2()
    builder.build_boost()
    builder.build_qt5base()
    builder.build_qt5tools()
    builder.build_ntc_cmake()
    builder.build_lebedev_utils()
    builder.build_msgpack()

    os.chdir(current)


def main():
    builder = MelonBuilder(current)
    build_dependencies(builder)

    cmake_prefix_path = ";".join(list(builder.prefixes.values()))
    cmake_prefix_path += ";/home/kovdan01/lib/sqlpp11-connector-mysql-0.29/prefix"

    build_path = os.path.join(current, "build")
    delete_dir_if_exists(build_path)
    build_debug_path = os.path.join(build_path, "debug-asan-ubsan")
    build_release_path = os.path.join(build_path, "release")
    os.mkdir(build_path)
    os.mkdir(build_debug_path)
    os.mkdir(build_release_path)

    prefix_path = os.path.join(current, "prefix")
    delete_dir_if_exists(prefix_path)
    prefix_debug_path = os.path.join(prefix_path, "debug-asan-ubsan")
    prefix_release_path = os.path.join(prefix_path, "release")
    os.mkdir(prefix_path)
    os.mkdir(prefix_debug_path)
    os.mkdir(prefix_release_path)

    execute_command('{} '.format(builder.cmake_binary) +
                    '-S {} '.format(current) +
                    '-B {} '.format(build_debug_path) +
                    '-D CMAKE_BUILD_TYPE=Debug ' +
                    '-G Ninja ' + 
                    '-D CMAKE_MAKE_PROGRAM={} '.format(builder.ninja_binary) +
                    '-D CMAKE_CXX_FLAGS="-g -fsanitize=address,undefined -fno-sanitize-recover=all" ' +
                    '-D CMAKE_PREFIX_PATH="{}" '.format(cmake_prefix_path) +
                    '-D CMAKE_INSTALL_PREFIX="{}" '.format(prefix_debug_path))

    execute_command('{} '.format(builder.cmake_binary) +
                    '-S {} '.format(current) +
                    '-B {} '.format(build_release_path) +
                    '-D CMAKE_BUILD_TYPE=Release ' +
                    '-G Ninja ' + 
                    '-D CMAKE_MAKE_PROGRAM={} '.format(builder.ninja_binary) +
                    '-D CMAKE_PREFIX_PATH="{}" '.format(cmake_prefix_path) +
                    '-D CMAKE_INSTALL_PREFIX="{}" '.format(prefix_release_path))

    with open('build_debug.sh', 'w') as f:
        f.write('#!/bin/bash\n\n' +
                'repo=`dirname "$0"`\n' +
                '{} --build $repo/build/debug-asan-ubsan --target all\n'.format(builder.cmake_binary) +
                '{} --build $repo/build/debug-asan-ubsan --target install\n'.format(builder.cmake_binary))
    execute_command("chmod +x build_debug.sh")

    with open('build_release.sh', 'w') as f:
        f.write('#!/bin/bash\n\n' +
                'repo=`dirname "$0"`\n' +
                '{} --build $repo/build/release --target all\n'.format(builder.cmake_binary) +
                '{} --build $repo/build/release --target install\n'.format(builder.cmake_binary))
    execute_command("chmod +x build_release.sh")

    ctest_binary = os.path.join(os.path.dirname(builder.cmake_binary), "ctest")

    with open('test_debug.sh', 'w') as f:
        f.write('#!/bin/bash\n\n' +
                'repo=`dirname "$0"`\n' +
                '{} -VV --test-dir $repo/build/debug-asan-ubsan\n'.format(ctest_binary))
    execute_command("chmod +x test_debug.sh")

    with open('test_release.sh', 'w') as f:
        f.write('#!/bin/bash\n\n' +
                'repo=`dirname "$0"`\n' +
                '{} -VV --test-dir $repo/build/release\n'.format(ctest_binary))
    execute_command("chmod +x test_release.sh")


if __name__ == "__main__":
    main()
