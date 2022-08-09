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

int main(int argc, char **argv) {
    if (argc != 2) {
        throw std::runtime_error("no passing input arguments");
    }
    const auto serverPort = std::stoi(argv[1]);
    auto serverAddress = std::make_unique<net::address::SocketAddressIpv4>(std::nullopt, serverPort);
    net::tcp::TcpListener tcpSocketListener(std::move(serverAddress), serverPort);

    net::TcpReceiver<1024> receiver(std::move(tcpSocketListener), 5);
    const net::TcpReceiver<1024>::Messages &messages = receiver.receive(5);

    for (auto i = 0; i < messages.size(); ++i) {
        const auto address = messages[i].first.getAddress().value()->getAddressStr();
        const auto data = std::move(messages[i].second);
        std::cout << "receive from: " << address << " data = [" << data.data() << "]" << std::endl;
    }

//    const auto serverPort = std::stoi(argv[1]);
//    auto serverAddress = std::make_unique<net::address::SocketAddressIpv4>(std::nullopt, serverPort);
//    net::tcp::TcpListener tcpSocketListener(std::move(serverAddress), serverPort);
//    net::acceptor::TcpMultiplexAcceptor acceptor(std::move(tcpSocketListener), handler);
//    acceptor.pollingLoop();

    return 0;
}