cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

project(melon-server
        VERSION 0.0.0
        LANGUAGES CXX
)

find_package(ntc-cmake REQUIRED)
include(ntc-dev-build)

add_subdirectory(auth)
add_subdirectory(messaging)
add_subdirectory(storage)
add_subdirectory(client-server)
add_subdirectory(server-server)
