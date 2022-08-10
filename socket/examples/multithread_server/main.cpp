#include <iostream>

#include <memory>
#include <optional>

#include "engine/net_api/socket/socket.h"
#include "engine/net_api/tcp_listener/tcp_listener.h"
#include "engine/net_api/address_family/socket_address_ipv4.h"
#include "engine/net_api/acceptor/multithread_acceptor/tcp_multithread_acceptor.h"
#include "engine/buffers/static_buffer.h"

auto handler = [](net::Socket &&socket){
    while (true) {
        io::StaticBuffer<1024> buffer;
        socket.read(buffer, buffer.capacity());

        const std::string data(buffer.data());
        std::cout << "receive: " << data << "from ip: "
                  << socket.addressStr() << std::endl;

        socket.write(buffer, buffer.capacity());
        if (data == std::string("stop\r\n")) {
            break;
        }
    }
    return true;
};

int main(int argc, char **argv) {
    if (argc != 2) {
        std::runtime_error("not passed input argument");
    }

    const short port = std::atoi(argv[1]);
    net::Socket socket(std::make_unique<net::address::SocketAddressIpv4>(std::nullopt, port),
            net::Socket::Type::Listener);
    net::tcp::TcpListener tcpListener(std::move(socket), 10);
    std::cout << "ip: " << tcpListener.getSocket().getAddress().value()->getAddressStr() << std::endl;
    net::acceptor::TcpMultiThreadAcceptor acceptor(std::move(tcpListener), handler);
    acceptor.pollingLoop();

    return 0;
}