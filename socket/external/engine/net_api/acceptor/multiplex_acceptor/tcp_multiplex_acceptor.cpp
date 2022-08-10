#include "tcp_multiplex_acceptor.h"

#include "socket/native_socket_api.h"

namespace net {

namespace acceptor {
    
TcpMultiplexAcceptor::TcpMultiplexAcceptor(tcp::TcpListener &&tcpListener, ClientRequestHandler clientRequestHandler):
listenerSocket_(tcpListener.getSocket()),
clientPollFdSet_(),
tcpSessions_(),
clientRequestHandler_(clientRequestHandler),
listenerHandler_([this](Socket &&listenerSocket){
    Socket clientSocket(listenerSocket.accept());
    const struct pollfd clientPollFd = {.fd = clientSocket.fd(), .events = POLLRDNORM};

    clientPollFdSet_.push_back(clientPollFd);
    tcpSessions_.emplace_back(std::move(clientSocket), clientRequestHandler_);
    return true;
}) {
    const struct pollfd listenerPollFd = {.fd = listenerSocket_.fd(), .events = POLLRDNORM};
    clientPollFdSet_.push_back(listenerPollFd);
    tcpSessions_.push_back(TcpSession(tcpListener.getSocket(), listenerHandler_));
}

TcpMultiplexAcceptor::TcpMultiplexAcceptor(TcpMultiplexAcceptor &&other):
listenerSocket_(std::move(other.listenerSocket_)),
clientPollFdSet_(std::move(other.clientPollFdSet_)),
tcpSessions_(std::move(other.tcpSessions_)),
clientRequestHandler_(std::move(other.clientRequestHandler_)),
listenerHandler_([this](Socket &&listenerSocket){
    Socket clientSocket(listenerSocket.accept());
    const struct pollfd clientPollFd = {.fd = clientSocket.fd(), .events = POLLRDNORM};

    clientPollFdSet_.push_back(clientPollFd);
    tcpSessions_.emplace_back(std::move(clientSocket), clientRequestHandler_);
    return true;
}) {
    tcpSessions_[0].setRequestHandler(listenerHandler_);
}

TcpMultiplexAcceptor &TcpMultiplexAcceptor::operator=(TcpMultiplexAcceptor &&other) {
    listenerSocket_ = std::move(other.listenerSocket_);
    clientPollFdSet_ = std::move(other.clientPollFdSet_);
    tcpSessions_ = std::move(other.tcpSessions_);
    clientRequestHandler_ = std::move(other.clientRequestHandler_);
    listenerHandler_ = std::move([this](Socket &&listenerSocket){
        Socket clientSocket(listenerSocket.accept());
        const struct pollfd clientPollFd = {.fd = clientSocket.fd(), .events = POLLRDNORM};

        clientPollFdSet_.push_back(clientPollFd);
        tcpSessions_.emplace_back(std::move(clientSocket), clientRequestHandler_);
        return true;
    });
    return *this;
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

void TcpMultiplexAcceptor::TcpSession::setRequestHandler(
        TcpMultiplexAcceptor::ClientRequestHandler requestHandler) {
    requestHandler_ = std::move(requestHandler);
}

} // namespace acceptor {

} // namespace net
