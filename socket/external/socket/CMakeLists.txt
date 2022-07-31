cmake_minimum_required(VERSION 3.16)

project(socket)

add_subdirectory(address_family)

add_library(${PROJECT_NAME} socket.cpp)

target_link_libraries(${PROJECT_NAME} PUBLIC
    socket_address_family
    socket_api
)

target_include_directories(${PROJECT_NAME} PUBLIC ../)

set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)