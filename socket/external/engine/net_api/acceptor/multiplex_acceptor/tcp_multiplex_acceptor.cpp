#include "tcp_multiplex_acceptor.h"

#include "socket/native_socket_api.h"

namespace net {

namespace acceptor {


TcpMultiplexAcceptor::TcpMultiplexAcceptor(tcp::TcpListener &&listenerSocket, ClientRequestHandler clientRequestHandler):
clientPollFdSet_(),
tcpSessions_() {
    const struct pollfd listenerPollFd = {.fd = listenerSocket.getSocket().fd(), .events = POLLRDNORM};
    clientPollFdSet_.push_back(listenerPollFd);

    //! Здесь будут проблемы если этот класс будет перемещен, так как при перемещении this уже будет
    //! другой, тогда получиться что в лямбде содержиться this, который указывает на уже опустошенный
    //! экземпляр класса. Следовательно данный класс нельзя копировать и перемещать.
    auto listenerRequestHandler = [this, clientRequestHandler] (Socket &&listenerSocket) {
        Socket clientSocket(listenerSocket.accept());
        const struct pollfd clientPollFd = {.fd = clientSocket.fd(), .events = POLLRDNORM};

        clientPollFdSet_.push_back(clientPollFd);
        tcpSessions_.push_back(TcpSession(std::move(clientSocket), clientRequestHandler));
        return true;
    };

    tcpSessions_.push_back(TcpSession(listenerSocket.getSocket(), listenerRequestHandler));
}

TcpMultiplexAcceptor::~TcpMultiplexAcceptor() {
    for (auto i = 0; i < tcpSessions_.size(); ++i) {
        tcpSessions_[i].close();
    }

    clientPollFdSet_.clear();
    tcpSessions_.clear();
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


TcpMultiplexAcceptor::TcpSession::TcpSession(Socket &&socket, ClientRequestHandler requestHandler):
socket_(std::move(socket)),
requestHandler_(requestHandler) {
}

TcpMultiplexAcceptor::TcpSession::TcpSession(TcpMultiplexAcceptor::TcpSession &&other) noexcept :
socket_(std::move(other.socket_)),
requestHandler_(std::move(other.requestHandler_)) {
}

TcpMultiplexAcceptor::TcpSession
&TcpMultiplexAcceptor::TcpSession::operator=(TcpMultiplexAcceptor::TcpSession &&other) noexcept {
    socket_ = std::move(other.socket_);
    requestHandler_ = std::move(other.requestHandler_);
    return *this;
}

bool TcpMultiplexAcceptor::TcpSession::operator()() {
    return requestHandler_(Socket(socket_));
}

void TcpMultiplexAcceptor::TcpSession::close() {
    native_socket::CloseSocket(socket_.fd());
}

} // namespace acceptor {

} // namespace net
