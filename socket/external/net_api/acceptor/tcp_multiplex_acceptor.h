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
    typedef std::function<bool()> RequestHandler;

    class TcpSession {
    public:
        TcpSession(Socket &&socket, Socket::Type type, RequestHandler requestHandler);

        bool handleEvent(const struct pollfd &pollFd);

    private:
        Socket socket_;
        Socket::Type type_;
        RequestHandler requestHandler_;
    };

    TcpMultiplexAcceptor(Socket &&listenerSocket, RequestHandler &&clientRequestHandler);

    void pollingLoop();

    void handlerListenerSocket();

private:
    int getCountReadyTcpSessions();

    std::vector<struct pollfd> clientPollFdSet_;
    std::vector<TcpSession> tcpSessions_;
    RequestHandler clientRequestHandler_;
    Socket listenerSocket_;
};

} // namespace acceptor

} // namespace net