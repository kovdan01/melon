cmake_minimum_required(VERSION 3.19 FATAL_ERROR)

project(melon-auth
        VERSION 0.0.0
        LANGUAGES CXX
        )

find_package(ntc-cmake REQUIRED)
include(ntc-dev-build)

# own libraries
find_package(melon-core REQUIRED)

find_package(ce-utils REQUIRED)
find_package(asio-utils REQUIRED)

# boost program options, asio
find_package(
    Boost 1.76 REQUIRED
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

set(MELON_AUTH_SASL_WRAPPERS melon-auth-sasl-wrappers)

add_library(${MELON_AUTH_SASL_WRAPPERS} INTERFACE
    ${LIB_HEADERS}
    ${LIB_SOURCES}
)

target_compile_features(${MELON_AUTH_SASL_WRAPPERS} INTERFACE cxx_std_20)

target_link_libraries(${MELON_AUTH_SASL_WRAPPERS} INTERFACE PkgConfig::cyrus-sasl)

ntc_target(${MELON_AUTH_SASL_WRAPPERS}
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

ntc_target(${PROJECT_NAME})

# compile test main only once to speed up compilation
set(TESTS_MAIN_NAME melon-auth-tests-main)

add_library(${TESTS_MAIN_NAME} OBJECT
    src/test_main.cpp)

target_compile_features(${TESTS_MAIN_NAME} PRIVATE cxx_std_20)
target_link_libraries(${TESTS_MAIN_NAME} PRIVATE Catch2::Catch2)

# melon-auth-test executable
set(TEST_CLIENT_NAME melon-auth-test-client)

add_executable(${TEST_CLIENT_NAME}
    ${HEADERS}
    src/test_client.cpp
)

if(WIN32)
    target_link_libraries(${TEST_CLIENT_NAME} PRIVATE ws2_32 mswsock)
endif()
target_link_libraries(${TEST_CLIENT_NAME} PRIVATE melon::core)
target_link_libraries(${TEST_CLIENT_NAME} PRIVATE Boost::program_options)
target_link_libraries(${TEST_CLIENT_NAME} PRIVATE Threads::Threads)
target_link_libraries(${TEST_CLIENT_NAME} PRIVATE melon::sasl_wrappers)
target_link_libraries(${TEST_CLIENT_NAME} PRIVATE Catch2::Catch2)
target_link_libraries(${TEST_CLIENT_NAME} PRIVATE ${TESTS_MAIN_NAME})

ntc_target(${TEST_CLIENT_NAME})

# register tests with CTest

include(CTest)
include(Catch)
catch_discover_tests(melon-auth-test-client)
