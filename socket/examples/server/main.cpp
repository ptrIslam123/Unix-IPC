#include <iostream>

#include <memory>
#include <optional>

#include "net_api/socket/socket.h"
#include "net_api/address_family/socket_address_ipv4.h"

int main(int argc, char **argv) {
    net::Socket listener(std::make_unique<net::address::SocketAddressIpv4>(std::nullopt, 8000));
    listener.bind();
    listener.makeListeningQueue(10);
    return 0;
}