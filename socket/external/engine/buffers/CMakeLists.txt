cmake_minimum_required(VERSION 3.16)

project(buffer)

add_library(${PROJECT_NAME} STATIC
    io_operations_api.cpp
    buffer.cpp
    dynamic_buffer.cpp
)

add_library(static_buffer INTERFACE static_buffer.h)

target_include_directories(${PROJECT_NAME} PUBLIC .)
target_include_directories(static_buffer INTERFACE .)

set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

set_target_properties(static_buffer PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)