#include "socket.h"

#include "native_socket_api.h"
#include "buffers/io_operations_api.h"

namespace net {

Socket::Socket(Address &&address):
socket_(0),
address_(std::move(address)),
isClosed_(true) {
    socket_ = native_socket::MakeSocket(address_.value()->getFamily(), address_.value()->getType());
    isClosed_ = false;
}

Socket::Socket(const int fd):
socket_(fd),
address_(std::nullopt),
isClosed_(true) {
}

Socket::Socket(Socket &&other):
socket_(other.socket_),
address_(std::move(other.address_)),
isClosed_(other.isClosed_) {
    other.socket_ = -1;
    other.isClosed_ = true;
}

Socket &Socket::operator=(Socket &&other) {
    socket_ = other.socket_;
    address_ = std::move(other.address_);
    isClosed_ = other.isClosed_;

    other.socket_ = -1;
    other.isClosed_ = true;
    return *this;
}

Socket::~Socket() {
    if (!isClosed_) {
        close();
    }
}

void Socket::bind() {
    if (address_.has_value()) {
        native_socket::BindSocket(socket_, address_.value()->getAddress(), address_.value()->getAddressLen());
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

void Socket::receive(io::Buffer &buffer) {
    io_operation::ReadFrom(socket_, buffer.data(), buffer.capacity());
}

void Socket::send(io::Buffer &buffer) {
    io_operation::WriteTo(socket_, buffer.data(), buffer.capacity());
}

int Socket::fd() const {
    return socket_;
}

void Socket::close() {
    native_socket::CloseSocket(socket_);
    isClosed_ = true;
}

} // namespace net_api