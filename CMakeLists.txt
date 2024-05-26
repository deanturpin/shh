cmake_minimum_required(VERSION 3.28)

# Retrieve the git hash
execute_process(
    COMMAND git rev-parse --short HEAD
    OUTPUT_VARIABLE GIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Warn if the compiler does not elide the copy from a local variable to the
# return value of a function in a context where it is allowed by
# [class.copy.elision]. This elision is commonly known as the Named Return
# Value Optimization."

set(CMAKE_CXX_FLAGS "-std=c++23 -O1 -Wall -Wextra -Wpedantic -march=native -Wnrvo")
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