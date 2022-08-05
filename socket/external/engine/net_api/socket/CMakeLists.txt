cmake_minimum_required(VERSION 3.16)

project(socket)

add_library(${PROJECT_NAME} STATIC
    socket.cpp
    native_socket_api.cpp
)

target_link_libraries(${PROJECT_NAME} PUBLIC
    buffer
    address_family
)

target_include_directories(${PROJECT_NAME} PUBLIC ./ ../)

set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)
