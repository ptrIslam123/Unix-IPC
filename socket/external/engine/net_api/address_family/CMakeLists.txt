cmake_minimum_required(VERSION 3.16)

project(address_family)

add_library(${PROJECT_NAME} STATIC
    socket_address_ipv4.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC .)

set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)
