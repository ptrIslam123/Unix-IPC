cmake_minimum_required(VERSION 3.16)

project(net)

add_subdirectory(servers/simple_server)
add_subdirectory(servers/multiplex_server)
add_subdirectory(servers/multiplex_server_use_poll)
add_subdirectory(external/buffers)
add_subdirectory(external/net_api)
add_subdirectory(examples/server)
add_subdirectory(clients)