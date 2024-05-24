cmake_minimum_required(VERSION 3.28)

# Retrieve the git hash
execute_process(
    COMMAND git rev-parse --short HEAD
    OUTPUT_VARIABLE GIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

set(CMAKE_CXX_FLAGS "-std=c++23 -O1 -Wall -Wextra -Wpedantic -march=native")
set(CMAKE_BUILD_TYPE Debug)

project(wispa)


add_executable(${PROJECT_NAME}
    main.cxx
    oui.cxx
    packet.cxx
)

# Define a preprocessor macro with the git hash
target_compile_definitions(${PROJECT_NAME} PRIVATE GIT_HASH="${GIT_HASH}")

target_link_libraries(${PROJECT_NAME} pcap)