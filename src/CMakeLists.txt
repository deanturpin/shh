cmake_minimum_required(VERSION 3.28)

set(CMAKE_CXX_FLAGS "-std=c++23 -O1 -Wall -Wextra -Wpedantic -march=native")
set(CMAKE_BUILD_TYPE Debug)

project(wispa)
add_executable(${PROJECT_NAME}
    main.cxx
    oui.cxx
    packet.cxx
)

target_link_libraries(${PROJECT_NAME} pcap)
