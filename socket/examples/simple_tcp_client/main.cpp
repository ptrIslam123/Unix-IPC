#include <iostream>
#include <memory>
#include <optional>

#include "engine/net_api/socket/socket.h"
#include "engine/net_api/address_family/socket_address_ipv4.h"
#include "engine/buffers/static_buffer.h"


int main(int argc, char **argv) {
    if (argc != 3) {
        throw std::runtime_error("no passing input arguments");
    }

    const std::string serverIpAddress = argv[1];
    const short port = std::stoi(argv[2]);
    net::Socket socket(std::make_unique<net::address::SocketAddressIpv4>(serverIpAddress, port),
            net::Socket::Type::Usual);
    socket.connect();
    io::StaticBuffer<1024> buffer;
    std::string msg;
    std::cin >> msg;

    buffer.write(msg.data(), msg.size());
    socket.write(buffer, buffer.size());
    buffer.clear();

    socket.read(buffer, buffer.capacity());
    std::cout << "receive from server: " << buffer.data() << std::endl;

    return 0;
}