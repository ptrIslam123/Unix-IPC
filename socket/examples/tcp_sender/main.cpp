#include <iostream>

#include <memory>
#include <optional>

#include "engine/net_api/socket/socket.h"
#include "engine/net_api/address_family/socket_address_ipv4.h"
#include "engine/net_api/sender/tcp_sender.h"
#include "engine/buffers/static_buffer.h"

int main(int argc, char **argv) {
    if (argc != 3) {
        throw std::runtime_error("no passing input arguments");
    }

    const std::string serverIpAddress(argv[1]);
    const short serverPort = std::stoi(argv[2]);
    net::Socket socket(std::make_unique<net::address::SocketAddressIpv4>(serverIpAddress, serverPort),
            net::Socket::Type::Usual);
    net::TcpSender sender(std::move(socket));

    for (auto i = 0; i < 2; ++i) {
        std::string data;
        std::cout << "Enter some input data for receiving tcp receiver" << std::endl;
        std::cin >> data;
        io::StaticBuffer<1024> buffer;
        buffer.write(data.data(), data.size());

        sender.send(buffer);
    }

    return 0;
}