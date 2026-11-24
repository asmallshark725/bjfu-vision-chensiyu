cmake_minimum_required(VERSION 3.16)
project(task4_hello_armor LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
find_package(OpenCV 4 REQUIRED COMPONENTS core imgproc imgcodecs videoio highgui calib3d)

add_library(task4_vision
    src/armor_detector.cpp
    src/pnp_solver.cpp
    ../task3/src/light.cpp
    ../task3/src/armor.cpp
)
target_include_directories(task4_vision PUBLIC include ../task3/include ${OpenCV_INCLUDE_DIRS})
target_link_libraries(task4_vision PUBLIC ${OpenCV_LIBS})
target_compile_options(task4_vision PRIVATE -Wall -Wextra -Wpedantic)

add_executable(hello_armor main.cpp)
target_link_libraries(hello_armor PRIVATE task4_vision)
target_compile_options(hello_armor PRIVATE -Wall -Wextra -Wpedantic)

include(CTest)
if(BUILD_TESTING)
    add_executable(task4_checks tests/checks.cpp)
    target_link_libraries(task4_checks PRIVATE task4_vision)
    add_test(NAME task4_geometry_and_pnp
             COMMAND task4_checks ${CMAKE_CURRENT_SOURCE_DIR}/config/camera.yml)
endif()
