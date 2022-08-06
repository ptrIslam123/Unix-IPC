#include "socket.h"

#include <netinet/in.h>
#include <cstring>

#include "native_socket_api.h"
#include "../buffers/io_operations_api.h"
#include "address_family/socket_address_ipv4.h"

namespace net {

Socket::Socket(Address &&address):
socket_(0),
address_(std::move(address)) {
    socket_ = native_socket::MakeSocket(address_.value()->getFamily(), address_.value()->getType());
}

Socket::Socket(const int fd):
socket_(fd),
address_(std::nullopt) {
}

Socket::Socket(const int fd, Address &&address):
socket_(fd),
address_(std::move(address)) {
}

Socket::Socket(Socket &&other):
socket_(other.socket_),
address_(std::move(other.address_)) {
    other.socket_ = -1;
}

Socket &Socket::operator=(Socket &&other) {
    socket_ = other.socket_;
    address_ = std::move(other.address_);
    other.socket_ = -1;
    return *this;
}

void Socket::bind() {
    if (address_.has_value()) {
        native_socket::BindSocket(socket_, address_.value()->getAddress(),
                                  address_.value()->getAddressLen());
    } else {
        throw std::runtime_error("attempt bind socket don`t have address struct");
    }
}

void Socket::connect() {
    if (!address_.has_value()) {
        throw std::runtime_error("attempt connect use socket without address structure");
    }

    if (address_.value()->getFamily() != AF_INET && address_.value()->getType() != SOCK_STREAM) {
        throw std::runtime_error("attempt connect for not tcp socket");
    }
    native_socket::Connect(socket_, address_.value()->getAddress(), address_.value()->getAddressLen());
}

void Socket::makeListeningQueue(size_t queueSize) {
    native_socket::MakeListenQueue(socket_, queueSize);
}

void Socket::read(io::Buffer &buffer, const size_t size) {
    io_operation::ReadFrom(socket_, buffer.data(), size);
}

void Socket::write(io::Buffer &buffer, const size_t size) {
    io_operation::WriteTo(socket_, buffer.data(), size);
}

int Socket::fd() const {
    return socket_;
}

Socket Socket::copy() const {
    Address address;
    if (address_.has_value()) {
        address = std::move(address_.value()->copy());
    }
    return Socket(fd(), std::move(address));
}

Socket Socket::accept() {
    struct sockaddr_in clientSocketAddress;
    socklen_t clientSocketAddressLen = sizeof(clientSocketAddress);
    std::memset(&clientSocketAddress, 0, clientSocketAddressLen);

    const auto clientFd = native_socket::Accept(fd(), (struct sockaddr*)&clientSocketAddress,
            &clientSocketAddressLen);
    return Socket(clientFd, std::make_unique<address::SocketAddressIpv4>(clientSocketAddress, 0));
}

const std::string_view Socket::addressStr() {
    if (!address_.has_value()) {
        return std::string_view ();
    }

    return address_.value()->getAddressStr();
}

void Socket::close() {
    native_socket::CloseSocket(socket_);
}

} // namespace net_api