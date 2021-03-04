cmake_minimum_required(VERSION 3.19 FATAL_ERROR)

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
    Boost 1.75.0 REQUIRED
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
    include/chat_widget.hpp
    include/main_window.hpp
    include/ram_storage.hpp
    include/chat_list_widget.hpp
    include/message_list_model.hpp
    include/chat_list_model.hpp
)

set(SOURCES
    src/chat_widget.cpp
    src/main.cpp
    src/main_window.cpp
    src/chat_list_widget.cpp
    src/message_list_model.cpp
    src/chat_list_model.cpp
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
