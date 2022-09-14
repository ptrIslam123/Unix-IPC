cmake_minimum_required(VERSION 3.16)

project(acceptor)

add_library(${PROJECT_NAME} STATIC
    multiplex_acceptor/tcp_multiplex_acceptor.cpp
    multithread_acceptor/tcp_multithread_acceptor.cpp
)

find_library(PTHREAD_LIBRARY pthread)

target_link_libraries(${PROJECT_NAME} PUBLIC
    socket
    buffer
    address_family
)

target_link_libraries(${PROJECT_NAME} INTERFACE static_buffer)

target_link_libraries(${PROJECT_NAME} PUBLIC ${PTHREAD_LIBRARY})

target_include_directories(${PROJECT_NAME} PUBLIC .)
target_include_directories(${PROJECT_NAME} PUBLIC ..)

set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)
