#include "tcp_listener.h"

namespace net {

namespace tcp {

TcpListener::TcpListener(Socket::Address &&address, const size_t listenQueueSize):
socket_(std::move(address), Socket::Type::Listener) {
    initSocket(listenQueueSize);
}

TcpListener::TcpListener(net::Socket &&listenerSocket, const size_t listenQueueSize):
socket_(std::move(listenerSocket)) {
    initSocket(listenQueueSize);
}

Socket::OperationResult TcpListener::initSocket(const size_t listenQueueSize) {
    socket_.bind();
    socket_.makeListeningQueue(listenQueueSize);
    return true;
}

Socket TcpListener::getSocket() {
    return socket_;
}

} // namespace tcp

} // namespace ne
