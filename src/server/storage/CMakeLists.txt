cmake_minimum_required(VERSION 3.19 FATAL_ERROR)

project(melon-storage
        VERSION 0.0.0
        LANGUAGES CXX
)

find_package(ntc-cmake REQUIRED)
include(ntc-dev-build)

# own libraries
find_package(melon-core REQUIRED)

# boost program options, asio
find_package(
    Boost 1.74.0 REQUIRED
    COMPONENTS program_options system
)

# mariadb
find_package(PkgConfig REQUIRED)
pkg_check_modules(mariadb REQUIRED IMPORTED_TARGET mariadb)

# sqlpp11
find_package(Sqlpp11 REQUIRED)

# thread support
set(THREADS_PREFER_PTHERAD_FLAG ON)
find_package(Threads REQUIRED)

set(HEADERS)

set(SOURCES
    src/main.cpp
)

include_directories("melondb.h")

add_executable(${PROJECT_NAME}
    ${HEADERS}
    ${SOURCES}
)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)

target_link_libraries(${PROJECT_NAME} PRIVATE melon::core)
target_link_libraries(${PROJECT_NAME} PRIVATE Boost::program_options)
target_link_libraries(${PROJECT_NAME} PRIVATE Boost::system)
target_link_libraries(${PROJECT_NAME} PRIVATE PkgConfig::mariadb)
target_link_libraries(${PROJECT_NAME} PRIVATE sqlpp11::sqlpp11)
target_link_libraries(${PROJECT_NAME} PRIVATE sqlpp-mysql)
target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)

ntc_target(${PROJECT_NAME})
