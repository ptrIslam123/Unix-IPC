cmake_minimum_required(VERSION 3.16)

project(socket)

add_library(${PROJECT_NAME}
    socket/socket.cpp
    socket/native_socket_api.cpp
    address_family/socket_address_ipv4.cpp
)

target_link_libraries(${PROJECT_NAME} PUBLIC
    buffer
)

target_include_directories(${PROJECT_NAME} PUBLIC
    address_family
    ../
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)