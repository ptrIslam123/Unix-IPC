cmake_minimum_required(VERSION 3.16)

project(client)

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} PUBLIC
    buffer
    socket
    address_family
    acceptor
)