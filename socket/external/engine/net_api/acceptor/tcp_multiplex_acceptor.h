#pragma once

#include <poll.h>

#include <vector>
#include <functional>

#include "socket/socket.h"
#include "../../buffers/buffer.h"
#include "../../buffers/static_buffer.h"

namespace net {

namespace acceptor {

class TcpMultiplexAcceptor {
public:
    typedef std::function<bool(Socket &&socket)> ClientRequestHandler;

    class TcpSession {
    public:
        TcpSession(Socket &&socket, ClientRequestHandler requestHandler);
        TcpSession(TcpSession &&other) noexcept;
        TcpSession &operator=(TcpSession &&other) noexcept;
        TcpSession(const TcpSession &other) = delete;
        TcpSession &operator=(const TcpSession &other) = delete;
        ~TcpSession() = default;

        void close();
        bool operator()();

    private:
        Socket socket_;
        ClientRequestHandler requestHandler_;
    };

    TcpMultiplexAcceptor(Socket &&listenerSocket, ClientRequestHandler clientRequestHandler);
    TcpMultiplexAcceptor(TcpMultiplexAcceptor &&other) noexcept;
    TcpMultiplexAcceptor &operator=(TcpMultiplexAcceptor &&other) noexcept;
    TcpMultiplexAcceptor(const TcpMultiplexAcceptor &other) = delete;
    TcpMultiplexAcceptor &operator=(const TcpMultiplexAcceptor &other) = delete;
    ~TcpMultiplexAcceptor();
    void pollingLoop();

private:
    void handleClientsSocket(int &readyCount);
    bool handleEvent(TcpSession &tcpSession, const struct pollfd &pollFd, int &readyCount);
    int getCountReadyTcpSessions();

    std::vector<struct pollfd> clientPollFdSet_;
    std::vector<TcpSession> tcpSessions_;
};

} // namespace acceptor

} // namespace net
