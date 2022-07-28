cmake_minimum_required(VERSION 3.16)

project(net)

add_subdirectory(servers/simple_server)
add_subdirectory(servers/multiplex_server)
add_subdirectory(clients)