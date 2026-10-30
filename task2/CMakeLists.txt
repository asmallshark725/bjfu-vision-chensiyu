cmake_minimum_required(VERSION 3.16)
project(add_demo LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

add_executable(score_app main.cpp src/scores.cpp)
target_include_directories(score_app PRIVATE include)