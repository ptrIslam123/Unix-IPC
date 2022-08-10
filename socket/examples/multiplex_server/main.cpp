#include <iostream>

#include <memory>
#include <optional>

#include "engine/net_api/socket/socket.h"
#include "engine/net_api/tcp_listener/tcp_listener.h"
#include "engine/net_api/address_family/socket_address_ipv4.h"
#include "engine/net_api/acceptor/multiplex_acceptor/tcp_multiplex_acceptor.h"
#include "engine/net_api/receiver/tcp_receiver.h"

auto handler = [](net::Socket &&socket) {
    io::StaticBuffer<1024> buffer;
    socket.read(buffer, buffer.capacity());

    std::cout << "receive: " << buffer.data() << "from ip: "
              << socket.addressStr() << std::endl;

    socket.write(buffer, buffer.capacity());
    return true;
};

auto MakeTcpMultiplexAcceptor(net::Socket::Address &&address, short port) {
    net::tcp::TcpListener tcpSocketListener(std::move(address), port);
    std::cout << "server ip: " << tcpSocketListener.getSocket().getAddress().value()->getAddressStr()
              << std::endl;
    net::acceptor::TcpMultiplexAcceptor acceptor(std::move(tcpSocketListener), handler);

    return std::move(acceptor);
}

int main(int argc, char **argv) {
    if (argc != 2) {
        throw std::runtime_error("no passing input arguments");
    }

    const auto serverPort = std::stoi(argv[1]);
    auto serverAddress = std::make_unique<net::address::SocketAddressIpv4>(std::nullopt, serverPort);
//    net::tcp::TcpListener tcpSocketListener(std::move(serverAddress), serverPort);
//    std::cout << "server ip: " << tcpSocketListener.getSocket().getAddress().value()->getAddressStr()
//            << std::endl;
//    net::acceptor::TcpMultiplexAcceptor acceptor(std::move(tcpSocketListener), handler);
//    acceptor.pollingLoop();

    auto acceptor = MakeTcpMultiplexAcceptor(std::move(serverAddress), serverPort);
    acceptor.pollingLoop();

    return 0;
}