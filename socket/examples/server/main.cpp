#include <iostream>

#include <memory>
#include <optional>

#include "engine/net_api/socket/socket.h"
#include "engine/net_api/address_family/socket_address_ipv4.h"
#include "engine/net_api/acceptor/tcp_multiplex_acceptor.h"
#include "engine/buffers/static_buffer.h"
#include "engine/utils/result/result.h"

typedef util::Result<int, std::string> Result;

Result Foo(int a, int b) {
    if (b == 0) {
        return std::string("undefined");
    }

    return a / b;
}

void TcpMultiplexServerExample() {
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

}

int main(int argc, char **argv) {

    int a = 20;
    int b = 0;
    auto result = Foo(a, b);
    if (result.isOk()) {
        std::cout << "result of " << a << "/" << b << " = " << result.value() << std::endl;
    } else {
        std::cout << "result of " << a << "/" << b << " = " << result.error() << std::endl;
    }

    return 0;
}