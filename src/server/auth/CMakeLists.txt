cmake_minimum_required(VERSION 3.19 FATAL_ERROR)

project(melon-auth
        VERSION 0.0.0
        LANGUAGES CXX
        )

set(TEST_CLIENT_NAME melon-auth-test-client)

find_package(ntc-cmake REQUIRED)
include(ntc-dev-build)

# own libraries
find_package(melon-core REQUIRED)

find_package(ce-utils REQUIRED)
find_package(asio-utils REQUIRED)

# boost program options, asio
find_package(
    Boost 1.75.0 REQUIRED
    COMPONENTS
    chrono
    context
    exception
    log
    program_options
    system
    thread
)

# thread support
set(THREADS_PREFER_PTHERAD_FLAG ON)
find_package(Threads REQUIRED)

# catch2
find_package(Catch2 REQUIRED)

# cyrus SASL
find_package(PkgConfig REQUIRED)
pkg_check_modules(cyrus-sasl REQUIRED IMPORTED_TARGET libsasl2)

# own sasl wrappers
set(LIB_HEADERS
    include/sasl_server_wrapper.hpp
    include/sasl_server_wrapper.ipp
)

set(LIB_SOURCES
)

add_library(sasl-wrappers INTERFACE
    ${LIB_HEADERS}
    ${LIB_SOURCES}
)

target_compile_features(sasl-wrappers INTERFACE cxx_std_20)

target_link_libraries(sasl-wrappers INTERFACE PkgConfig::cyrus-sasl)

ntc_target(sasl-wrappers
    ALIAS_NAME melon::sasl_wrappers
)

# melon-auth executable

set(HEADERS)

set(SOURCES
    src/main.cpp
)

add_executable(${PROJECT_NAME}
    ${HEADERS}
    ${SOURCES}
)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)

target_link_libraries(${PROJECT_NAME} PRIVATE ce::asio-utils)

target_link_libraries(${PROJECT_NAME} PRIVATE melon::core)
target_link_libraries(${PROJECT_NAME} PRIVATE Boost::program_options)
target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)
target_link_libraries(${PROJECT_NAME} PRIVATE melon::sasl_wrappers)

# melon-auth executable

add_executable(${TEST_CLIENT_NAME}
    ${HEADERS}
    src/test_client.cpp
)

if(WIN32)
    target_link_libraries(melon-auth-test-client PRIVATE ws2_32 mswsock)
endif()
target_link_libraries(${TEST_CLIENT_NAME} PRIVATE melon::core)
target_link_libraries(${TEST_CLIENT_NAME} PRIVATE Boost::program_options)
target_link_libraries(${TEST_CLIENT_NAME} PRIVATE Threads::Threads)
target_link_libraries(${TEST_CLIENT_NAME} PRIVATE melon::sasl_wrappers)
target_link_libraries(${TEST_CLIENT_NAME} PRIVATE Catch2::Catch2)

ntc_target(${PROJECT_NAME})

# register tests with CTest

include(CTest)
include(ParseAndAddCatchTests)
ParseAndAddCatchTests(melon-auth-test-client)
