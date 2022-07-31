#include "socket_address_ipv4.h"
#include <cstring>

namespace net {

namespace address {

SocketAddressIpv4::SocketAddressIpv4(const IpAddress &ipaddress, const short port):
SocketAddress(),
ipAddress_(ipaddress),
port_(port),
socketAddress_() {
    std::memset(&socketAddress_, 0, sizeof(socketAddress_));
    fellAddress();
}

int SocketAddressIpv4::getFamily() const {
    return AF_INET;
}

int SocketAddressIpv4::getType() const {
    return SOCK_STREAM;
}

SocketAddress::Address *SocketAddressIpv4::getAddress() const {
    return reinterpret_cast<SocketAddress::Address>(&socketAddress_);
}

socklen_t SocketAddressIpv4::getAddressLen() const {
    return sizeof(socketAddress_);
}

void SocketAddressIpv4::fellAddress() {
    socketAddress_.sin_family = getFamily();
    socketAddress_.sin_port = htons(port_);

    if (ipAddress_.has_value()) {
        int res = inet_pton(AF_INET, ipAddress_.value(), &socketAddress_);
    } else {
        socketAddress_.sin_addr = htonl(INADDR_ANY);
    }
}

} // namespace address

} // namespace net