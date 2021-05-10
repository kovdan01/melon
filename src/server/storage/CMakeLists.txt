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
    Boost 1.76 REQUIRED
    COMPONENTS
    log
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
    COMMAND ${PYTHON_EXECUTABLE} scripts/ddl2cpp.py sql/db_scheme_dump.sql ${CMAKE_CURRENT_BINARY_DIR}/autogen/melon/db_scheme melon
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DEPENDS scripts/ddl2cpp.py
)

# storage wrappers
set(STORAGE_ENTITIES_LIB melon-storage-entities)

add_library(${STORAGE_ENTITIES_LIB}
    include/entities.hpp
    include/config_db.hpp
    src/entities.cpp
    src/config_db.cpp
)

target_include_directories(${STORAGE_ENTITIES_LIB} PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/autogen)
target_sources(${STORAGE_ENTITIES_LIB} PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/autogen/melon/db_scheme.h)

target_compile_features(${STORAGE_ENTITIES_LIB} PRIVATE cxx_std_20)

target_link_libraries(${STORAGE_ENTITIES_LIB} PUBLIC melon::core)
target_link_libraries(${STORAGE_ENTITIES_LIB} PRIVATE PkgConfig::mariadb)
target_link_libraries(${STORAGE_ENTITIES_LIB} PUBLIC sqlpp11::sqlpp11)
target_link_libraries(${STORAGE_ENTITIES_LIB} PUBLIC sqlpp11::connector-mysql)

ntc_target(${STORAGE_ENTITIES_LIB}
    ALIAS_NAME melon::storage_entities
    HEADER_PREFIX melon/server/storage/entities/
)

# storage service itself
add_executable(${PROJECT_NAME}
    src/main.cpp
)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)

# linker errors on GitHub Actions without explicit link with MariaDB and sqlpp11 for some reason
target_link_libraries(${PROJECT_NAME} PRIVATE PkgConfig::mariadb)
target_link_libraries(${PROJECT_NAME} PRIVATE sqlpp11::sqlpp11)
target_link_libraries(${PROJECT_NAME} PRIVATE sqlpp11::connector-mysql)

target_link_libraries(${PROJECT_NAME} PRIVATE melon::storage_entities)
target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)

ntc_target(${PROJECT_NAME})

# compile main only once to speed up compilation
set(TESTS_MAIN melon-storage-tests-main)

add_library(${TESTS_MAIN} OBJECT
    src/tests_main.cpp
)

target_compile_features(${TESTS_MAIN} PRIVATE cxx_std_20)

# linker errors on GitHub Actions without explicit link with MariaDB for some reason
target_link_libraries(${TESTS_MAIN} PRIVATE PkgConfig::mariadb)

target_link_libraries(${TESTS_MAIN} PRIVATE melon::core)
target_link_libraries(${TESTS_MAIN} PRIVATE Catch2::Catch2)

# test cases itself
set(STORAGE_TESTS melon-storage-tests)

add_executable(${STORAGE_TESTS}
    src/tests.cpp
)

target_compile_features(${STORAGE_TESTS} PRIVATE cxx_std_20)

target_link_libraries(${STORAGE_TESTS} PRIVATE melon::storage_entities)
target_link_libraries(${STORAGE_TESTS} PRIVATE Catch2::Catch2)
target_link_libraries(${STORAGE_TESTS} PRIVATE Threads::Threads)
target_link_libraries(${STORAGE_TESTS} PRIVATE Boost::log)
target_link_libraries(${STORAGE_TESTS} PRIVATE ${TESTS_MAIN})

ntc_target(${STORAGE_TESTS})

include(CTest)
include(Catch)
catch_discover_tests(${STORAGE_TESTS})
