#include "socket.h"

#include "native_socket_api.h"

namespace net {

Socket::Socket(std::unique_ptr<address::SocketAddress> &&address):
socket_(0),
address_(std::move(address)),
isClosed_(true) {
    socket_ = native_socket::MakeSocket(address_->getFamily(), address_->getType());
    isClosed_ = false;
}

Socket::~Socket() {
    if (!isClosed_) {
        close();
    }
}

void Socket::bind() {
    native_socket::BindSocket(socket_, address_->getAddress(), address_->getAddressLen());
}

void Socket::connect() {
    native_socket::Connect(socket_, address_->getAddress(), address_->getAddressLen());
}

void Socket::makeListeningQueue(size_t queueSize) {
    native_socket::MakeListenQueue(socket_, queueSize);
}

void Socket::receive(const io::Buffer &buffer) {
}

void Socket::send(io::Buffer &buffer) {
}

void Socket::close() {
    native_socket::CloseSocket(socket_);
    isClosed_ = true;
}

} // namespace net_api