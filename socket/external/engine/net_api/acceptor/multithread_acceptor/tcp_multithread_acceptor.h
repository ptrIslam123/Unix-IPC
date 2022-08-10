#pragma once

#include <functional>
#include <vector>
#include <thread>

#include "socket/socket.h"
#include "tcp_listener/tcp_listener.h"

namespace net {

namespace acceptor {

class TcpMultiThreadAcceptor {
public:
    typedef std::function<bool(Socket &&socket)> ClientRequestHandler;

    class TcpSession {
    public:
        TcpSession(Socket &&clientSocket, ClientRequestHandler clientRequestHandler);

        bool operator()();

    private:
        Socket clientSocket_;
        ClientRequestHandler clientRequestHandler_;
    };

    TcpMultiThreadAcceptor(tcp::TcpListener &&tcpListenerSocket,
                           ClientRequestHandler clientRequestHandler);
    ~TcpMultiThreadAcceptor() = default;
    void pollingLoop();

private:
    void createNewTcpSessionThread(Socket &&clientSocket);

    Socket listenerSocket_;
    ClientRequestHandler clientRequestHandler_;
    std::vector<std::thread> tcpSessions_;
};

} // namespace acceptor

} // namespace net