cmake_minimum_required(VERSION 3.16)

project(simple_client)

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} PUBLIC
    socket
    address_family
    acceptor
)

target_link_libraries(${PROJECT_NAME} INTERFACE utils)

target_include_directories(${PROJECT_NAME} PUBLIC ../../external)

set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)