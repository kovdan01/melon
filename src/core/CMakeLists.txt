cmake_minimum_required(VERSION 3.19 FATAL_ERROR)

project(melon-core
        VERSION 0.0.0
        LANGUAGES CXX
)

find_package(ntc-cmake REQUIRED)
include(ntc-dev-build)

# boost asio
find_package(
    Boost 1.74.0 REQUIRED
    COMPONENTS system
)

# message pack
# find_package(msgpack REQUIRED)

# thread support
set(THREADS_PREFER_PTHERAD_FLAG ON)
find_package(Threads REQUIRED)

set(HEADERS
    include/melon_core.hpp
)

set(SOURCES
    src/melon_core.cpp
)

add_library(${PROJECT_NAME}
    ${HEADERS}
    ${SOURCES}
)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)

target_link_libraries(${PROJECT_NAME} PRIVATE Boost::system)
target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)

ntc_target(${PROJECT_NAME}
    ALIAS_NAME melon::core
    HEADER_PREFIX melon/core/
)
