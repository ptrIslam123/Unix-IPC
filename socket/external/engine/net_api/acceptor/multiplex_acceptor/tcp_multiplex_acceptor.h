#pragma once

#include <poll.h>

#include <vector>
#include <functional>

#include "socket/socket.h"
#include "tcp_listener/tcp_listener.h"
#include "../../buffers/buffer.h"
#include "../../buffers/static_buffer.h"

namespace net {

namespace acceptor {

class TcpMultiplexAcceptor {
public:
    class TcpSession;
    typedef std::function<bool(Socket &&socket)> ClientRequestHandler;
    typedef std::vector<struct pollfd> ClientPollFdSet;
    typedef std::vector<TcpSession> TcpSessions;

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

        void setRequestHandler(ClientRequestHandler requestHandler);

    private:
        Socket socket_;
        ClientRequestHandler requestHandler_;
    };

    TcpMultiplexAcceptor(tcp::TcpListener &&tcpListener, ClientRequestHandler clientRequestHandler);
    TcpMultiplexAcceptor(TcpMultiplexAcceptor &&other);
    TcpMultiplexAcceptor &operator=(TcpMultiplexAcceptor &&other);
    TcpMultiplexAcceptor(const TcpMultiplexAcceptor &other) = delete;
    TcpMultiplexAcceptor &operator=(const TcpMultiplexAcceptor &other) = delete;
    ~TcpMultiplexAcceptor();
    void handleClientsSocket(int &readyCount);
    int getCountReadyTcpSessions();
    void pollingLoop();

private:
    typedef ClientRequestHandler ListenerHandler;

    bool handleEvent(TcpSession &tcpSession, const struct pollfd &pollFd, int &readyCount);

    Socket listenerSocket_;
    ClientPollFdSet clientPollFdSet_;
    TcpSessions tcpSessions_;
    ClientRequestHandler clientRequestHandler_;
    ListenerHandler listenerHandler_;
};

} // namespace acceptor

} // namespace net
