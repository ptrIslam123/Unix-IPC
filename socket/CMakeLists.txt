cmake_minimum_required(VERSION 3.16)

project(net)

add_subdirectory(servers/simple_server)
add_subdirectory(servers/multiplex_server)
add_subdirectory(servers/multiplex_server_use_poll)

add_subdirectory(external/engine/buffers)
add_subdirectory(external/engine/net_api)

add_subdirectory(examples/multiplex_server)
add_subdirectory(examples/simple_tcp_client)
add_subdirectory(examples/tcp_receiver)
add_subdirectory(examples/tcp_sender)
add_subdirectory(examples/multithread_server)

add_subdirectory(clients)