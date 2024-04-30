cmake_minimum_required(VERSION 3.28.3)

set(CMAKE_CXX_FLAGS "-std=c++23 -O1 -Wall -Wextra -Wpedantic -march=native")

if(APPLE)
    # clang for macOS
    set(CMAKE_CXX_COMPILER /usr/local/bin/clang++)
    set(CMAKE_C_COMPILER /usr/local/bin/clang)
elseif(LINUX)
    # gnu for Linux
    set(CMAKE_CXX_COMPILER /usr/local/bin/g++)
    set(CMAKE_C_COMPILER /usr/local/bin/gcc)
else()
    # godspeed
    message(WARNING "Platform unknown")
endif()

project(wispa)
add_executable(${PROJECT_NAME} main.cxx)
