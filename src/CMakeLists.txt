cmake_minimum_required(VERSION 3.28.3)

set(CMAKE_CXX_FLAGS "-std=c++23 -Ofast -Wall -Wextra -Wpedantic -march=native")

if(APPLE)
    set(CMAKE_CXX_COMPILER /usr/local/bin/clang++)
    set(CMAKE_C_COMPILER /usr/local/bin/clang)
endif()

project(wispa)
add_executable(${PROJECT_NAME} main.cxx)
