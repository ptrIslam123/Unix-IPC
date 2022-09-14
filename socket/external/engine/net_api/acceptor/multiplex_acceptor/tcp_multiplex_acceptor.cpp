#include "tcp_multiplex_acceptor.h"

namespace {

class TcpListenerRequestHandler {
public:
    typedef net::acceptor::TcpMultiplexAcceptor::ClientPollFdSet ClientPollFdSet;
    typedef net::acceptor::TcpMultiplexAcceptor::TcpSessions TcpSessions;
    typedef net::acceptor::TcpMultiplexAcceptor::ClientRequestHandler ClientRequestHandler;

    explicit TcpListenerRequestHandler(ClientPollFdSet &clientPollFdSet, TcpSessions &tcpSessions,
                                       ClientRequestHandler clientRequestHandler);
    bool operator()(net::Socket &&socket);

private:
    ClientPollFdSet &clientPollFdSet_;
    TcpSessions &tcpSessions_;
    ClientRequestHandler clientRequestHandler_;
};

TcpListenerRequestHandler::TcpListenerRequestHandler(
        ClientPollFdSet &clientPollFdSet, TcpSessions &tcpSessions, ClientRequestHandler clientRequestHandler):
clientPollFdSet_(clientPollFdSet),
tcpSessions_(tcpSessions),
clientRequestHandler_(clientRequestHandler) {
}

bool TcpListenerRequestHandler::operator()(net::Socket &&listenerSocket) {
    net::Socket clientSocket(listenerSocket.accept());
    const struct pollfd clientPollFd = {.fd = clientSocket.fd(), .events = POLLRDNORM};

    clientPollFdSet_.push_back(clientPollFd);
    tcpSessions_.emplace_back(std::move(clientSocket), clientRequestHandler_);
    return true;
}

} // namespace


namespace net {

namespace acceptor {

TcpMultiplexAcceptor::TcpMultiplexAcceptor(tcp::TcpListener &&tcpListener, ClientRequestHandler clientRequestHandler):
listenerSocket_(tcpListener.getSocket()),
clientPollFdSet_(),
tcpSessions_(),
clientRequestHandler_(clientRequestHandler),
listenerHandler_(TcpListenerRequestHandler(clientPollFdSet_, tcpSessions_, clientRequestHandler)) {
    const struct pollfd listenerPollFd = {.fd = listenerSocket_.fd(), .events = POLLRDNORM};
    clientPollFdSet_.push_back(listenerPollFd);
    tcpSessions_.push_back(TcpSession(tcpListener.getSocket(), listenerHandler_));
}

TcpMultiplexAcceptor::TcpMultiplexAcceptor(TcpMultiplexAcceptor &&other):
listenerSocket_(std::move(other.listenerSocket_)),
clientPollFdSet_(std::move(other.clientPollFdSet_)),
tcpSessions_(std::move(other.tcpSessions_)),
clientRequestHandler_(std::move(other.clientRequestHandler_)),
listenerHandler_(TcpListenerRequestHandler(clientPollFdSet_, tcpSessions_, clientRequestHandler_)) {
    tcpSessions_[0].setRequestHandler(listenerHandler_);
}

TcpMultiplexAcceptor &TcpMultiplexAcceptor::operator=(TcpMultiplexAcceptor &&other) {
    listenerSocket_ = std::move(other.listenerSocket_);
    clientPollFdSet_ = std::move(other.clientPollFdSet_);
    tcpSessions_ = std::move(other.tcpSessions_);
    clientRequestHandler_ = std::move(other.clientRequestHandler_);
    listenerHandler_ = std::move(TcpListenerRequestHandler(clientPollFdSet_, tcpSessions_, clientRequestHandler_));
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
    socket_.close();
}

void TcpMultiplexAcceptor::TcpSession::setRequestHandler(
        TcpMultiplexAcceptor::ClientRequestHandler requestHandler) {
    requestHandler_ = std::move(requestHandler);
}

} // namespace acceptor {

} // namespace net
