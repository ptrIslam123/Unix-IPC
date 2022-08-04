#include "tcp_multiplex_acceptor.h"

namespace net {

namespace acceptor {


TcpMultiplexAcceptor::TcpMultiplexAcceptor(Socket &&listenerSocket, RequestHandler &&clientRequestHandler):
clientPollFdSet_(),
tcpSessions_(),
clientRequestHandler_(std::move(clientRequestHandler)),
listenerSocket_(std::move(listenerSocket)) {
    const struct pollfd listenerPollFd = {.fd = listenerSocket_.fd(), .events = POLLRDNORM};
    clientPollFdSet_.push_back(listenerPollFd);
}

void TcpMultiplexAcceptor::pollingLoop() {
    while (true) {
        auto readyCount = getCountReadyTcpSessions();
        handlerListenerSocket();
        for (auto i = 0; i < tcpSessions_.size(); ++i) {
            tcpSessions_[i].handleEvent(clientPollFdSet_[i + 1]);
        }
    }
}

int TcpMultiplexAcceptor::getCountReadyTcpSessions() {
    const auto result = poll(clientPollFdSet_.data(), clientPollFdSet_.size(), -1);
    if (result < 0) {
        throw std::runtime_error("error in system call poll");
    }

    return result;
}

void TcpMultiplexAcceptor::handlerListenerSocket() {
    if (clientPollFdSet_[0].revents == POLLRDNORM) {
        const int clientFd = native_socket::Accept(listenerSocket_.fd(), NULL, NULL);
        const struct pollfd clientPollFd = {.fd = clientFd, .events = POLLRDNORM};
        clientPollFdSet_.push_back(clientPollFd);

        tcpSessions_.push_back(TcpSession(Socket(clientFd), Socket::Type::ClientSocket,
                                          clientRequestHandler_));
    }
}


    TcpMultiplexAcceptor::TcpSession::TcpSession(
    Socket &&socket, Socket::Type type, TcpMultiplexAcceptor::RequestHandler requestHandler):
    socket_(std::move(socket)),
    type_(type),
    requestHandler_(requestHandler) {
}

bool TcpMultiplexAcceptor::TcpSession::handleEvent(const struct pollfd &pollFd) {
    if (pollFd.revents == POLLRDNORM) {
        requestHandler_();
    }

    return true;
}


} // namespace acceptor {

} // namespace net
