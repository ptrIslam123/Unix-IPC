cmake_minimum_required(VERSION 3.16)

project(scope_guard)

add_library(${PROJECT_NAME} INTERFACE scope_guard.h)

set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)