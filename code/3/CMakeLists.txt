cmake_minimum_required(VERSION 3.14)
project(asio_echo LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Boost REQUIRED COMPONENTS system)

add_executable(asio_echo_server asio_echo_server.cpp)
target_link_libraries(asio_echo_server PRIVATE Boost::system pthread)

add_executable(asio_echo_client asio_echo_client.cpp)
target_link_libraries(asio_echo_client PRIVATE Boost::system pthread)
