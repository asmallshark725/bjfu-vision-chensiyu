cmake_minimum_required(VERSION 3.16)
project(add_demo LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
add_executable(armor_demo main.cpp src/light.cpp src/armor.cpp src/detector.cpp)
target_include_directories(armor_demo PRIVATE include)