#!/bin/python

import os
import shutil
import sys

current = os.path.abspath(os.path.dirname(os.path.realpath(__file__)))
sys.path.append(os.path.join(current, "3rd-party", "cpp-deps-automation"))

from builder import Builder, save_builder, execute_command


def recreate_directory(directory):
    if os.path.isdir(directory):
        shutil.rmtree(directory)
    os.mkdir(directory)


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
    recreate_directory(deps_path)
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

    save_builder(builder)
    os.chdir(current)


def construct_debug_release_path(path):
    debug_path = os.path.join(path, "debug-asan-ubsan")
    release_path = os.path.join(path, "release")
    return (debug_path, release_path)


def build_melon(builder):
    cmake_prefix_path = ";".join(list(builder.prefixes.values()))

    recreate_directory("build")
    build_debug_path, build_release_path = construct_debug_release_path(os.path.join(current, "build"))
    recreate_directory("prefix")
    prefix_debug_path, prefix_release_path = construct_debug_release_path(os.path.join(current, "prefix"))

    builder.build_cmake(source_dir=current,
                        build_type="Debug",
                        build_dir=build_debug_path,
                        prefix_dir=prefix_debug_path,
                        cmake_params='-D CMAKE_CXX_FLAGS="-g -fsanitize=address,undefined -fno-sanitize-recover=all" ' +
                                     '-D CMAKE_PREFIX_PATH="{}" '.format(cmake_prefix_path))

    builder.build_cmake(source_dir=current,
                        build_type="Release",
                        build_dir=build_release_path,
                        prefix_dir=prefix_release_path,
                        cmake_params='-D CMAKE_PREFIX_PATH="{}" '.format(cmake_prefix_path))


def create_shell_scripts(builder):
    build_debug_path, build_release_path = construct_debug_release_path(os.path.join(current, "build"))

    def create_build_script(build_path, build_type):
        with open("build_{}.sh".format(build_type), "w") as f:
            f.write('#!/bin/bash\n\n' +
                    '{} --build {} --target all\n'.format(builder.cmake_binary, build_path) +
                    '{} --build {} --target install\n'.format(builder.cmake_binary, build_path))
        execute_command("chmod +x build_{}.sh".format(build_type))

    ctest_binary = os.path.join(os.path.dirname(builder.cmake_binary), "ctest")

    def create_test_script(build_path, build_type):
        with open("test_{}.sh".format(build_type), "w") as f:
            f.write('#!/bin/bash\n\n' +
                    '{} -VV --test-dir {}\n'.format(ctest_binary, build_path))
        execute_command("chmod +x test_{}.sh".format(build_type))

    def create_both_scripts(build_path, build_type):
        create_build_script(build_path, build_type)
        create_test_script(build_path, build_type)

    create_both_scripts(build_debug_path, "debug")
    create_both_scripts(build_release_path, "release")


def main():
    builder = MelonBuilder(current)
    build_dependencies(builder)
    build_melon(builder)
    create_shell_scripts(builder)


if __name__ == "__main__":
    main()
