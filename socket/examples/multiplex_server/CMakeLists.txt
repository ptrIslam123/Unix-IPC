cmake_minimum_required(VERSION 3.16)

project(server)

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} PUBLIC
    address_family
    acceptor
    tcp_socket
)

target_link_libraries(${PROJECT_NAME} INTERFACE utils)

target_include_directories(${PROJECT_NAME} PUBLIC ../../external)

set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)