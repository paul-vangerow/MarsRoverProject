# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

project(motor_control)
set(CMAKE_CXX_STANDARD 14)

MESSAGE(STATUS "1 \n")

FILE(GLOB_RECURSE headers RELATIVE ${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/lib/*.h)

SET(libraries "")

FOREACH(item ${headers})
    get_filename_component(temp ${item} DIRECTORY)
    LIST(APPEND libraries ${temp})
ENDFOREACH()

list(REMOVE_DUPLICATES libraries)

add_executable(motor_control src/main.cpp)

FOREACH(item ${libraries})
    MESSAGE(STATUS "${item} \n")
ENDFOREACH()

target_include_directories(motor_control PUBLIC ${libraries})

