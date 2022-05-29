# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

project(motor_control)
set(CMAKE_CXX_STANDARD 14)

add_executable(motor_control src/main.cpp)
target_include_directories(motor_control PUBLIC include/**)

