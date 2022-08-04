#pragma once

#include <poll.h>

#include <vector>
#include <functional>

#include "../socket/socket.h"
#include "../socket/native_socket_api.h"
#include "socket_address_ipv4.h"
#include "buffers/buffer.h"
#include "buffers/static_buffer.h"

namespace net {

namespace acceptor {

class TcpMultiplexAcceptor {
public:
    typedef std::function<bool(const io::Buffer &buffer)> ClientRequestHandler;

    class TcpSession {
    public:
        TcpSession(Socket &&socket, Socket::Type type, ClientRequestHandler requestHandler);

        bool operator()();

    private:
        Socket socket_;
        Socket::Type type_;
        ClientRequestHandler requestHandler_;
    };

    TcpMultiplexAcceptor(Socket &&listenerSocket, ClientRequestHandler clientRequestHandler);

    void pollingLoop();

    void handleClientsSocket(int &readyCount);

    bool handleEvent(TcpSession &tcpSession, const struct pollfd &pollFd, int &readyCount);

private:
    int getCountReadyTcpSessions();

    std::vector<struct pollfd> clientPollFdSet_;
    std::vector<TcpSession> tcpSessions_;
};

} // namespace acceptor

} // namespace net