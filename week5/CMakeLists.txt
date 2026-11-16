cmake_minimum_required(VERSION 3.16)
project(week5 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
find_package(OpenCV 4 REQUIRED)

add_executable(day1 day1.cpp)
target_include_directories(day1 PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(day1 PRIVATE ${OpenCV_LIBS})

add_executable(day2 day2.cpp)
target_include_directories(day2 PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(day2 PRIVATE ${OpenCV_LIBS})

add_executable(day3 day3.cpp)
target_include_directories(day3 PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(day3 PRIVATE ${OpenCV_LIBS})

add_executable(day4 day4.cpp)
target_include_directories(day4 PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(day4 PRIVATE ${OpenCV_LIBS})

add_executable(day5 day5.cpp)
target_include_directories(day5 PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(day5 PRIVATE ${OpenCV_LIBS})

add_executable(day6 day6.cpp ../task3/src/light.cpp)
target_include_directories(day6 PRIVATE
    ${OpenCV_INCLUDE_DIRS}
    ../task3/include
)
target_link_libraries(day6 PRIVATE ${OpenCV_LIBS})
