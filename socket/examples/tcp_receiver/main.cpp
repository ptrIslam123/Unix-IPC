#include <iostream>

#include <memory>
#include <optional>

#include "engine/net_api/socket/socket.h"
#include "engine/net_api/tcp_listener/tcp_listener.h"
#include "engine/net_api/address_family/socket_address_ipv4.h"
#include "engine/net_api/acceptor/multiplex_acceptor/tcp_multiplex_acceptor.h"
#include "engine/net_api/receiver/tcp_receiver.h"

int main(int argc, char **argv) {
    if (argc != 2) {
        throw std::runtime_error("no passing input arguments");
    }

    const auto serverPort = std::stoi(argv[1]);
    auto serverAddress = std::make_unique<net::address::SocketAddressIpv4>(std::nullopt, serverPort);
    net::tcp::TcpListener tcpSocketListener(std::move(serverAddress), serverPort);
    net::TcpReceiver<1024> receiver(std::move(tcpSocketListener), 1);

    std::cout << "\t\tStart tcp receiver for listening" << std::endl;
    const net::TcpReceiver<1024>::Messages &messages = receiver.receive(5);

    for (auto i = 0; i < messages.size(); ++i) {
        const auto address = messages[i].first.getAddress().value()->getAddressStr();
        const auto data = std::move(messages[i].second);
        std::cout << "receive from: " << address << " data = [" << data.data() << "]" << std::endl;
    }

    return 0;
}