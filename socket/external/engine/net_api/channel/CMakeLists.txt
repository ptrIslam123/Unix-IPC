cmake_minimum_required(VERSION 3.16)

project(tcp_channel)

add_library(${PROJECT_NAME} INTERFACE

)

target_link_libraries(${PROJECT_NAME} INTERFACE
    buffer
    address_family
    socket
    receivers
    senders
)

target_include_directories(${PROJECT_NAME} INTERFACE .)
target_include_directories(${PROJECT_NAME} INTERFACE ..)

set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)