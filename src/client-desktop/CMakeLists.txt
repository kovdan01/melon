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
    Boost 1.75 REQUIRED
    COMPONENTS system
)

# thread support
set(THREADS_PREFER_PTHERAD_FLAG ON)
find_package(Threads REQUIRED)

# qt widgets, translations
find_package(
    Qt5 REQUIRED
    COMPONENTS Widgets LinguistTools Sql
)

set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_CURRENT_LIST_DIR}/ui)

set(HEADERS
    include/chat_item_delegate.hpp
    include/chat_list_model.hpp
    include/chat_list_view.hpp
    include/chat_widget.hpp
    include/config.hpp
    include/db_storage.hpp
    include/db_storage.ipp
    include/main_window.hpp
    include/message_item_delegate.hpp
    include/message_list_model.hpp
    include/ram_storage.hpp
    include/ram_storage.ipp
    include/helpers.hpp
)

set(SOURCES
    src/chat_item_delegate.cpp
    src/chat_list_model.cpp
    src/chat_list_view.cpp
    src/chat_widget.cpp
    src/db_storage.cpp
    src/main.cpp
    src/main_window.cpp
    src/message_item_delegate.cpp
    src/message_list_model.cpp
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

if(MINGW)
    # for localtime_r
    target_compile_definitions(${PROJECT_NAME} PRIVATE _POSIX_C_SOURCE)
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE melon::core)
target_link_libraries(${PROJECT_NAME} PRIVATE Boost::system)
target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Widgets Qt5::Sql)

ntc_target(${PROJECT_NAME}
    TRANSLATIONS en_US
)
