cmake_minimum_required(VERSION 3.28)

set(CMAKE_CXX_FLAGS "-std=c++23 -O1 -Wall -Wextra -Wpedantic -march=native")
set(CMAKE_BUILD_TYPE Debug)

if(APPLE)
    # Use the latest homebrew version
    set(CMAKE_CXX_COMPILER /opt/homebrew/bin/g++-13)
    set(CMAKE_C_COMPILER /opt/homebrew/bin/gcc-13)
elseif(LINUX)
    # Use the compiled version
    set(CMAKE_CXX_COMPILER /usr/local/bin/g++)
    set(CMAKE_C_COMPILER /usr/local/bin/gcc)
else()
    # godspeed
    message(WARNING "Platform unknown")
endif()

project(wispa)
add_executable(${PROJECT_NAME}
    main.cxx
    oui.cxx
    packet.cxx
)

target_link_libraries(${PROJECT_NAME} pcap)
