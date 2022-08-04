#include "tcp_multiplex_acceptor.h"

namespace net {

namespace acceptor {


TcpMultiplexAcceptor::TcpMultiplexAcceptor(Socket &&listenerSocket, ClientRequestHandler clientRequestHandler):
clientPollFdSet_(),
tcpSessions_() {
    const int listenerFd = listenerSocket.fd();
    const struct pollfd listenerPollFd = {.fd = listenerSocket.fd(), .events = POLLRDNORM};
    clientPollFdSet_.push_back(listenerPollFd);

    auto listenerRequestHandler = [this, listenerFd, clientRequestHandler] (const io::Buffer &) {
        const int clientFd = native_socket::Accept(listenerFd, NULL, NULL);
        const struct pollfd clientPollFd = {.fd = clientFd, .events = POLLRDNORM};
        clientPollFdSet_.push_back(clientPollFd);

        tcpSessions_.push_back(TcpSession(Socket(clientFd), Socket::Type::ClientSocket,
                                          clientRequestHandler));
        return true;
    };

    tcpSessions_.push_back(TcpSession(std::move(listenerSocket), Socket::Type::ListenerSocket,
                                      listenerRequestHandler));
}

void TcpMultiplexAcceptor::pollingLoop() {
    while (true) {
        auto readyCount = getCountReadyTcpSessions();
        handleClientsSocket(readyCount);
    }
}

int TcpMultiplexAcceptor::getCountReadyTcpSessions() {
    const auto result = poll(clientPollFdSet_.data(), clientPollFdSet_.size(), -1);
    if (result < 0) {
        throw std::runtime_error("error in system call poll");
    }

    return result;
}

void TcpMultiplexAcceptor::handleClientsSocket(int &readyCount) {
    for (auto i = 0; readyCount > 0 && i < tcpSessions_.size(); ++i) {
        handleEvent(tcpSessions_[i], clientPollFdSet_[i], readyCount);
    }
}

bool TcpMultiplexAcceptor::handleEvent(TcpSession &tcpSession, const pollfd &pollFd, int &readyCount) {
    bool result = true;
    if (pollFd.revents == POLLRDNORM) {
        result = tcpSession();
        --readyCount;
    }

    return result;
}


TcpMultiplexAcceptor::TcpSession::TcpSession(
    Socket &&socket, const Socket::Type type, TcpMultiplexAcceptor::ClientRequestHandler requestHandler):
    socket_(std::move(socket)),
    type_(type),
    requestHandler_(requestHandler) {
}

bool TcpMultiplexAcceptor::TcpSession::operator()() {
    if (type_ == Socket::Type::ClientSocket) {
        io::StaticBuffer<1024> buffer;
        socket_.receive(buffer);
        return requestHandler_(buffer);
    } else {
        io::StaticBuffer<1> buffer;
        return requestHandler_(buffer);
    }
}
} // namespace acceptor {

} // namespace net
