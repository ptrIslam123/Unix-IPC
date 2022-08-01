#include <iostream>

#include <memory>
#include <optional>

#include "net_api/socket/socket.h"
#include "net_api/address_family/socket_address_ipv4.h"

#include "buffers/buffer.h"
#include "buffers/static_buffer.h"

int main(int argc, char **argv) {
    net::Socket listener(std::make_unique<net::address::SocketAddressIpv4>(std::nullopt, 8000));
    listener.bind();
    listener.makeListeningQueue(10);

    int clientfd = accept(listener.fd(), NULL, NULL);

    char msg[] = {"hello from server!"};
    io::StaticBuffer<1024> buff;
    buff.write(msg, strlen(msg));

    write(clientfd, buff.data(), buff.size());

    close(clientfd);

    return 0;
}