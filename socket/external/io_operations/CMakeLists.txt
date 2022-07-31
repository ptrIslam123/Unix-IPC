cmake_minimum_required(VERSION 3.16)

project(io_operations)

add_library(${PROJECT_NAME} io_operations_api.cpp)

set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)