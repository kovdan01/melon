cmake_minimum_required(VERSION 3.5)

project(TestWidgets LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)

set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_CURRENT_LIST_DIR}/ui)

set(HEADERS
    include/mainwindow.h
)

set(SOURCES
    src/main.cpp
    src/mainwindow.cpp
)

set(UI_FILES
    ui/mainwindow.ui
)

add_executable(${PROJECT_NAME}
    ${HEADERS}
    ${SOURCES}
    ${UI_FILES}
)

target_link_libraries(TestWidgets PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
