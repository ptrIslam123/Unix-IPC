cmake_minimum_required(VERSION 3.16)

project(socket_address_family)

add_library(${PROJECT_NAME}
    socket_address_ipv4.cpp
)

target_link_libraries(${PROJECT_NAME} PUBLIC
    socket_api
)

target_include_directories(${PROJECT_NAME} PUBLIC ../socket_api)

set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)