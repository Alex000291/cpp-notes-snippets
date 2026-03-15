cmake_minimum_required(VERSION 3.14)
project(tcp_echo LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(tcp_echo_server tcp_echo_server.cpp)
target_link_libraries(tcp_echo_server PRIVATE pthread)

add_executable(tcp_echo_client tcp_echo_client.cpp)