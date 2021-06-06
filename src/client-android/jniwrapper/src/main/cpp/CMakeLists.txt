cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

add_library(hello-jni SHARED
    hello-jni.cpp)

target_link_libraries(
    hello-jni
    PRIVATE
    android
)
