#include <iostream>

#include <memory>
#include <optional>

#include <functional>

#include "net_api/socket/socket.h"
#include "net_api/address_family/socket_address_ipv4.h"
#include "net_api/acceptor/tcp_multiplex_acceptor.h"


#include "buffers/buffer.h"
#include "buffers/static_buffer.h"

void foo(net::Socket &&listener) {
    struct pollfd listenerPollFd = {.fd = listener.fd(), .events = POLLRDNORM};
    auto readyCount = poll(&listenerPollFd, 1, -1);

    if (listenerPollFd.revents == POLLRDNORM) {
        std::cout << "New connection event" << std::endl;
    }
}

int main(int argc, char **argv) {
    net::Socket listener(std::make_unique<net::address::SocketAddressIpv4>(std::nullopt, 8000));
    listener.bind();
    listener.makeListeningQueue(10);

    auto handler = [](const io::Buffer &buffer) {
        std::cout << "receive: " << buffer.data() << std::endl;
        return true;
    };

    net::acceptor::TcpMultiplexAcceptor acceptor(std::move(listener), handler);
    acceptor.pollingLoop();

//    foo(std::move(listener));

    return 0;
}