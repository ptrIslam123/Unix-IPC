#include "socket_address_ipv4.h"
#include <cstring>
#include <arpa/inet.h>

namespace net {

namespace address {

SocketAddressIpv4::SocketAddressIpv4(const struct sockaddr_in &socketAddress, const short port):
SocketAddress(),
ipAddress_(std::nullopt),
port_(port),
socketAddress_(socketAddress) {
}

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
    return (SocketAddress::Address*)(&socketAddress_);
}

socklen_t SocketAddressIpv4::getAddressLen() const {
    return sizeof(socketAddress_);
}

void SocketAddressIpv4::fellAddress() {
    socketAddress_.sin_family = getFamily();
    socketAddress_.sin_port = htons(port_);

    if (ipAddress_.has_value()) {
        int res = inet_pton(AF_INET, ipAddress_.value().c_str(), &socketAddress_);
    } else {
        socketAddress_.sin_addr.s_addr = htonl(INADDR_ANY);
    }
}

const std::string_view SocketAddressIpv4::getAddressStr() {
    static std::array<char, INET_ADDRSTRLEN> addressBuff = {0};
    static bool isEvalSocketAddress = false;
    if (!isEvalSocketAddress) {
        inet_ntop(getFamily(), &socketAddress_, addressBuff.data(), getAddressLen());
        isEvalSocketAddress = true;
    }

    return std::string_view(addressBuff.data());
}

std::unique_ptr<SocketAddress> SocketAddressIpv4::copy() const {
    return std::unique_ptr<SocketAddress>(new SocketAddressIpv4(socketAddress_, port_));
}

} // namespace address

} // namespace net_api