cmake_minimum_required(VERSION 3.16)

project(acceptor)

add_library(${PROJECT_NAME} STATIC
   tcp_multiplex_acceptor.cpp
)

target_link_directories(${PROJECT_NAME} PUBLIC
    socket
    buffer
    address_family
)

target_include_directories(${PROJECT_NAME} PUBLIC ../)

set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)
