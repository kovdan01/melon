cmake_minimum_required(VERSION 3.16)

project(cosec-examples
    DESCRIPTION "Examples for COSEC students"
    VERSION 0.0.0
    LANGUAGES CXX ASM
)

find_package(ntc-cmake 1.1.0 REQUIRED)
include(ntc-dev-build)

find_package(Boost 1.74 REQUIRED COMPONENTS chrono container context log thread)

add_subdirectory(utils)
add_subdirectory(asio-utils)
