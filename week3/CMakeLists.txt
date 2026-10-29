cmake_minimum_required(VERSION 3.16)
project(add_demo LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

add_executable(day1 main.cpp src/add.cpp)
target_include_directories(day1 PRIVATE include)
