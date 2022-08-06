cmake_minimum_required(VERSION 3.16)

project(buffer)

add_library(${PROJECT_NAME} STATIC
    io_operations_api.cpp
    buffer.cpp
)

target_link_directories(${PROJECT_NAME} PUBLIC .)

set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)