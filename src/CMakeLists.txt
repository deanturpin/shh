cmake_minimum_required(VERSION 3.28.3)

set(CMAKE_CXX_STANDARD 26)

if(APPLE)
    set(CMAKE_CXX_COMPILER /usr/local/bin/clang++)
    set(CMAKE_C_COMPILER /usr/local/bin/clang)
endif()

# Define the project
project(wispa)
add_executable(${PROJECT_NAME} main.cxx)
