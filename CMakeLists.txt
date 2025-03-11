cmake_minimum_required(VERSION 3.29)

execute_process(
    COMMAND git rev-parse --short HEAD
    OUTPUT_VARIABLE GIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

set(CMAKE_CXX_FLAGS "-std=c++26 -g -O1 -Wall -Wextra -Wpedantic -march=native -Wnrvo")
set(CMAKE_BUILD_TYPE Release)
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

install(TARGETS ${PROJECT_NAME}
	DESTINATION bin
)

# CPack configuration
set(CPACK_GENERATOR "DEB")
set(CPACK_PACKAGE_NAME "shh")
set(CPACK_PACKAGE_VERSION "0.1.0")
set(CPACK_PACKAGE_DESCRIPTION "A brief description of your package")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A summary of your package")
set(CPACK_PACKAGE_MAINTAINER "Your Name <youremail@example.com>")
set(CPACK_PACKAGE_VENDOR "Your Company")
set(CPACK_PACKAGE_CONTACT "Your Name <youremail@example.com>")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Your Name <youremail@example.com>")
set(CPACK_DEBIAN_PACKAGE_SECTION "utils")
set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6 (>= 2.7)")  # Add any runtime dependencies here

include(CPack)

