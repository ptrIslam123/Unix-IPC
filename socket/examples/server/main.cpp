#include <iostream>

#include <memory>
#include <optional>

#include "net_api/socket/socket.h"
#include "net_api/address_family/socket_address_ipv4.h"
#include "net_api/acceptor/tcp_multiplex_acceptor.h"
#include "buffers/static_buffer.h"

int main(int argc, char **argv) {
    net::Socket listener(std::make_unique<net::address::SocketAddressIpv4>(std::nullopt, 8000));
    listener.bind();
    listener.makeListeningQueue(10);

    auto handler = [](net::Socket &&socket) {
        io::StaticBuffer<1024> buffer;
        socket.read(buffer, buffer.capacity());

        std::cout << "receive: " << buffer.data() << "from ip: "
                << socket.addressStr() << std::endl;

        socket.write(buffer, buffer.capacity());
        return true;
    };

    net::acceptor::TcpMultiplexAcceptor acceptor(std::move(listener), handler);
    acceptor.pollingLoop();


    return 0;
}