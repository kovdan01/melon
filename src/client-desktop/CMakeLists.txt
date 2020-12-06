cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

project(melon-client-desktop
        VERSION 0.0.0
        LANGUAGES CXX
)

find_package(ntc-cmake REQUIRED)
include(ntc-dev-build)

# own libraries
find_package(melon-core REQUIRED)

# boost asio
find_package(
    Boost 1.74.0 REQUIRED
    COMPONENTS system
)

# thread support
set(THREADS_PREFER_PTHERAD_FLAG ON)
find_package(Threads REQUIRED)

# qt widgets, translations
find_package(
    Qt5 REQUIRED
    COMPONENTS Widgets LinguistTools
)

set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_CURRENT_LIST_DIR}/ui)

set(HEADERS
    include/main_window.hpp
    include/chat_widget.hpp
)

set(SOURCES
    src/main.cpp
    src/main_window.cpp
    src/chat_widget.cpp
)

set(UI_FILES
    ui/main_window.ui
    ui/chat_widget.ui
)

add_executable(${PROJECT_NAME}
    ${HEADERS}
    ${SOURCES}
    ${UI_FILES}
)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)

target_link_libraries(${PROJECT_NAME} PRIVATE melon::core)
target_link_libraries(${PROJECT_NAME} PRIVATE Boost::system)
target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Widgets)

ntc_target(${PROJECT_NAME}
    TRANSLATIONS en_US
)
