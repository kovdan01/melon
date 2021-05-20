cmake_minimum_required(VERSION 3.19 FATAL_ERROR)

project(melon-core
        VERSION 0.0.0
        LANGUAGES CXX
)

find_package(ntc-cmake REQUIRED)
include(ntc-dev-build)

# boost asio
find_package(
    Boost 1.76 REQUIRED
    COMPONENTS
    chrono
    context
    log
    system
    thread
)

# own libraries
find_package(ce-utils REQUIRED)
find_package(asio-utils REQUIRED)

# message pack
find_package(msgpack REQUIRED)

# thread support
set(THREADS_PREFER_PTHERAD_FLAG ON)
find_package(Threads REQUIRED)

# yaml-cpp
find_package(yaml-cpp REQUIRED)

# cyrus SASL
find_package(PkgConfig REQUIRED)
pkg_check_modules(cyrus-sasl REQUIRED IMPORTED_TARGET libsasl2)

set(HEADERS
    include/melon/core.hpp
    include/melon/core/commands.hpp
    include/melon/core/entities.hpp
    include/melon/core/entities.ipp
    include/melon/core/exception.hpp
    include/melon/core/jni_tests.hpp
    include/melon/core/log_configuration.hpp
    include/melon/core/sasl_client_wrapper.hpp
    include/melon/core/sasl_client_wrapper.ipp
    include/melon/core/serialization.hpp
    include/melon/core/session.hpp
    include/melon/core/token.hpp
    include/melon/core/token.ipp
    include/melon/core/yaml_config.hpp
)

set(SOURCES
    src/exception.cpp
    src/jni_tests.cpp
    src/log_configuration.cpp
    src/melon_core.cpp
    src/serialization.cpp
    src/yaml_config.cpp
)

add_library(${PROJECT_NAME}
    ${HEADERS}
    ${SOURCES}
)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)

target_link_libraries(${PROJECT_NAME} PRIVATE Boost::system)
target_link_libraries(${PROJECT_NAME} PUBLIC Boost::headers)
target_link_libraries(${PROJECT_NAME} PUBLIC Boost::log)
target_link_libraries(${PROJECT_NAME} PUBLIC ce::asio-utils)
target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)
target_link_libraries(${PROJECT_NAME} PUBLIC yaml-cpp)
target_link_libraries(${PROJECT_NAME} PUBLIC msgpackc-cxx)
target_link_libraries(${PROJECT_NAME} PUBLIC PkgConfig::cyrus-sasl)

ntc_target(${PROJECT_NAME}
    ALIAS_NAME melon::core
    HEADER_PREFIX melon/core/
)
