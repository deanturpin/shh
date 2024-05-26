cmake_minimum_required(VERSION 3.28)

# Retrieve the git hash
execute_process(
    COMMAND git rev-parse --short HEAD
    OUTPUT_VARIABLE GIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

set(CMAKE_CXX_FLAGS "-std=c++23 -O1 -Wall -Wextra -Wpedantic -march=native")
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_COMPILER g++-14)
set(CMAKE_C_COMPILER gcc-14)

project(shh)

add_executable(${PROJECT_NAME}
    src/main.cxx
    src/oui.cxx
    src/packet.cxx
)

# Define a preprocessor macro with the git hash
target_compile_definitions(${PROJECT_NAME} PRIVATE GIT_HASH="${GIT_HASH}")

target_link_libraries(${PROJECT_NAME} pcap)