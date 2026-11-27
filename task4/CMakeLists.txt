cmake_minimum_required(VERSION 3.16)
project(task4 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(OpenCV 4 REQUIRED)

add_executable(video_read
    video_read.cpp
    ../task3/src/light.cpp
    ../task3/src/armor.cpp
)

target_include_directories(video_read PRIVATE
    ${OpenCV_INCLUDE_DIRS}
    ../task3/include
)
target_link_libraries(video_read PRIVATE ${OpenCV_LIBS})
