#include "tcp_multithread_acceptor.h"

#include <thread>

namespace net {

namespace acceptor {

TcpMultiThreadAcceptor::TcpMultiThreadAcceptor(tcp::TcpListener &&tcpListenerSocket,
                                               ClientRequestHandler clientRequestHandler):
listenerSocket_(std::move(tcpListenerSocket.getSocket())),
clientRequestHandler_(clientRequestHandler),
tcpSessions_() {}

void TcpMultiThreadAcceptor::pollingLoop() {
    while (true) {
        auto clientSocket = listenerSocket_.accept();
        createNewTcpSessionThread(std::move(clientSocket));
    }
}

void TcpMultiThreadAcceptor::createNewTcpSessionThread(Socket &&clientSocket) {
    TcpSession tcpSession(std::move(clientSocket), clientRequestHandler_);
    std::thread task(std::move(tcpSession));
    task.detach();

    tcpSessions_.push_back(std::move(task));
}

TcpMultiThreadAcceptor::TcpSession::TcpSession(Socket &&clientSocket, ClientRequestHandler clientRequestHandler):
clientSocket_(std::move(clientSocket)),
clientRequestHandler_(clientRequestHandler) {
}

bool TcpMultiThreadAcceptor::TcpSession::operator()() {
    return clientRequestHandler_(Socket(clientSocket_));
}
} // namespace acceptor

} // namespace net
