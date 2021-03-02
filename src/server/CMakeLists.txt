cmake_minimum_required(VERSION 3.19 FATAL_ERROR)

project(melon-server
        VERSION 0.0.0
        LANGUAGES CXX
)

find_package(ntc-cmake REQUIRED)
include(ntc-dev-build)

add_subdirectory(auth)
add_subdirectory(messaging)

# TODO: configure MariaDB for macOS
if(NOT APPLE)
    add_subdirectory(storage)
endif()

add_subdirectory(client-server)
add_subdirectory(server-server)
