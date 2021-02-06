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
pkg_check_modules(mariadb REQUIRED IMPORTED_TARGET libmariadb)

# sqlpp11
find_package(Sqlpp11 REQUIRED)

set(SQLPP11_MYSQL_CONNECTOR_PREFIX
    "/usr"
    CACHE
    FILEPATH
    "Path to the sqlpp11 mysql connector prefix"
)
set(SQLPP11_MYSQL_CONNECTOR_INCLUDE
    "${SQLPP11_MYSQL_CONNECTOR_PREFIX}/include"
    CACHE
    FILEPATH
    "Path to the directory containing sqlpp11/mysql/mysql.h"
)
set(SQLPP11_MYSQL_CONNECTOR_LIB
    "${SQLPP11_MYSQL_CONNECTOR_PREFIX}/lib"
    CACHE
    FILEPATH
    "Path to the directory containing libsqlpp-mysql.a"
)

set(SQLPP11_)

# thread support
set(THREADS_PREFER_PTHERAD_FLAG ON)
find_package(Threads REQUIRED)

set(HEADERS
    include/melon/storage.hpp
)

set(SOURCES
    src/melon_storage.cpp
    src/main.cpp
)

#message(STATUS "CMAKE_CURRENT_BINARY_DIR = ${CMAKE_CURRENT_BINARY_DIR}")
#message(STATUS "CMAKE_SOURCE_DIR = ${CMAKE_SOURCE_DIR}")

#find_package(PythonInterp REQUIRED)

#add_custom_command(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/melondb.h"
#    COMMAND "${PYTHON_EXECUTABLE}" /home/user/Downloads/sqlpp11/scripts/ddl2cpp" --argument
#    DEPENDS some_target)


##add_custom_target(generate_header ALL
##    DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/include/Generated.hpp")

#install(FILES ${CMAKE_CURRENT_BINARY_DIR}/include/Generated.hpp DESTINATION include)

include_directories("melondb.h")

add_executable(${PROJECT_NAME}
    ${HEADERS}
    ${SOURCES}
)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)

target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE ${SQLPP11_MYSQL_CONNECTOR_INCLUDE})
target_link_directories(${PROJECT_NAME} PRIVATE ${SQLPP11_MYSQL_CONNECTOR_LIB})

target_link_libraries(${PROJECT_NAME} PRIVATE melon::core)
target_link_libraries(${PROJECT_NAME} PRIVATE Boost::program_options)
target_link_libraries(${PROJECT_NAME} PRIVATE Boost::system)
target_link_libraries(${PROJECT_NAME} PRIVATE PkgConfig::mariadb)
target_link_libraries(${PROJECT_NAME} PRIVATE sqlpp11::sqlpp11)
target_link_libraries(${PROJECT_NAME} PRIVATE sqlpp-mysql)
target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)

ntc_target(${PROJECT_NAME}
    ALIAS_NAME melon::server
    HEADER_PREFIX melon/server/storage
)
