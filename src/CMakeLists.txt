cmake_minimum_required(VERSION 3.28.3)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_COMPILER /usr/local/bin/clang++)
set(CMAKE_C_COMPILER /usr/local/bin/clang)

project(wispa)
add_executable(${PROJECT_NAME} main.cxx)
