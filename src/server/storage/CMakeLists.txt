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

# catch2
find_package(Catch2 REQUIRED)

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

# melon storage wrappers
set(LIB_HEADERS
    include/storage.hpp
)

set(LIB_SOURCES
    src/storage.cpp
)

add_library(storage-wrappers STATIC
    ${LIB_HEADERS}
    ${LIB_SOURCES}
)

target_include_directories(storage-wrappers PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/autogen)
target_sources(storage-wrappers PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/autogen/melon/db_scheme.h)

target_compile_features(storage-wrappers PUBLIC cxx_std_20)

target_link_libraries(storage-wrappers PUBLIC melon::core)
target_link_libraries(storage-wrappers PUBLIC PkgConfig::mariadb)
target_link_libraries(storage-wrappers PUBLIC sqlpp11::sqlpp11)
target_link_libraries(storage-wrappers PUBLIC sqlpp11::connector-mysql)

ntc_target(storage-wrappers
    ALIAS_NAME melon::storage_wrappers
)

set(HEADERS)

set(SOURCES
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
target_link_libraries(${PROJECT_NAME} PRIVATE melon::storage_wrappers)

# compile main only once to speed up compilation
set(TESTS_MAIN_NAME tests-main)

add_library(${TESTS_MAIN_NAME} OBJECT
    src/test_main.cpp)

target_compile_features(${TESTS_MAIN_NAME} PRIVATE cxx_std_20)
target_link_libraries(${TESTS_MAIN_NAME} PRIVATE Catch2::Catch2)

set(TEST_STORAGE_NAME melon-storage-tests)

set(TEST_HEADERS)

set(TEST_SOURCES
    src/storage_tests.cpp
)

add_executable(${TEST_STORAGE_NAME}
    ${TEST_HEADERS}
    ${TEST_SOURCES}
)

target_link_libraries(${TEST_STORAGE_NAME} PRIVATE melon::storage_wrappers)
target_link_libraries(${TEST_STORAGE_NAME} PRIVATE ${TESTS_MAIN_NAME})
target_link_libraries(${TEST_STORAGE_NAME} PRIVATE Catch2::Catch2)

ntc_target(${PROJECT_NAME})

include(CTest)
include(Catch)
catch_discover_tests(${TEST_STORAGE_NAME})
