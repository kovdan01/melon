cmake_minimum_required(VERSION 3.19 FATAL_ERROR)

project(melon
        VERSION 0.0.0
        LANGUAGES CXX
)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake/ntc")

# Include ntc-dev-build to disable find_package(ntc-cmake)
# in subprojects and establish default for BUILD_SHARED_LIBS.
include(ntc-dev-build)

enable_testing()

option(MELON_BUILD_SERVER "Build server services" ON)
option(MELON_BUILD_DESKTOP_CLIENT "Build desktop client (Qt is required)" ON)

add_subdirectory(src/core)
if(${MELON_BUILD_SERVER})
    add_subdirectory(src/server)
elseif()
if(${MELON_BUILD_DESKTOP_CLIENT})
    add_subdirectory(src/client-desktop)
elseif()

include(FeatureSummary)
feature_summary(WHAT ALL)
