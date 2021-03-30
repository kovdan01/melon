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
    Boost 1.75 REQUIRED
    COMPONENTS program_options system
)

# mariadb
find_package(PkgConfig REQUIRED)
pkg_check_modules(mariadb REQUIRED IMPORTED_TARGET libmariadb)

# sqlpp11
find_package(Sqlpp11 REQUIRED)
find_package(Sqlpp11ConnectorMysql REQUIRED)

# thread support
set(THREADS_PREFER_PTHERAD_FLAG ON)
find_package(Threads REQUIRED)

# python
find_package(PythonInterp REQUIRED)

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/melon/db_scheme.h)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/autogen/melon/db_scheme.h
    COMMAND ${PYTHON_EXECUTABLE} script/ddl2cpp.py sql/db_scheme_dump.sql ${CMAKE_CURRENT_BINARY_DIR}/autogen/melon/db_scheme melon
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DEPENDS script/ddl2cpp.py
)

set(HEADERS
    include/storage.hpp
)

set(SOURCES
    src/storage.cpp
    src/main.cpp
)

add_executable(${PROJECT_NAME}
    ${HEADERS}
    ${SOURCES}
)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/autogen)
target_sources(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/autogen/melon/db_scheme.h)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)

target_link_libraries(${PROJECT_NAME} PRIVATE melon::core)
target_link_libraries(${PROJECT_NAME} PRIVATE Boost::program_options)
target_link_libraries(${PROJECT_NAME} PRIVATE Boost::system)
target_link_libraries(${PROJECT_NAME} PRIVATE PkgConfig::mariadb)
target_link_libraries(${PROJECT_NAME} PRIVATE sqlpp11::sqlpp11)
target_link_libraries(${PROJECT_NAME} PRIVATE sqlpp11::connector-mysql)
target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)

ntc_target(${PROJECT_NAME})
