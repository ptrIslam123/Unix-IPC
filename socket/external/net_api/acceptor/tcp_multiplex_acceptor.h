#pragma once

#include <poll.h>

#include <vector>

#include "../socket/socket.h"
#include "../socket/native_socket_api.h"
#include "socket_address_ipv4.h"
#include "buffers/static_buffer.h"

namespace net {

namespace acceptor {

template<typename ClientRequestHandler>
class TcpMultiplexAcceptor {
public:
    template<typename EventHandler>
    class TcpSession {
    public:
        TcpSession(Socket &&socket, Socket::Type type, const struct pollfd *pollFd,
                   const EventHandler &eventHandler);

        bool handleEvent();

    private:
        Socket socket_;
        Socket::Type type_;
        const struct pollfd *pollFd_;
        EventHandler eventHandler_;
    };

    TcpMultiplexAcceptor(Socket &&listener);

    void loop();

private:
    int inputOutputEventOnSessions();

    struct pollfd makePollFd(int socket, short event) const;

    std::vector<struct pollfd> pollFdSet_;
    std::vector<TcpSession<bool(io::Buffer&)>> tcpSessions_;
};

template<typename ClientRequestHandler>
TcpMultiplexAcceptor<ClientRequestHandler>::TcpMultiplexAcceptor(Socket &&listener):
pollFdSet_(),
tcpSessions_() {
    const auto listenerPollFd = makePollFd(listener.fd(), POLLRDNORM);
    pollFdSet_.push_back(listenerPollFd);
    tcpSessions_.push_back(TcpSession(std::move(listener), Socket::Type::ListenerSocket, [this](io::Buffer &buffer) {
        int clientfd = 0;
        buffer.read((io::Buffer::Byte*)&clientfd);

        const Socket clientSocket(clientfd);
        const struct pollfd clientPollFd = {.fd = clientfd, .events = POLLRDNORM};

        pollFdSet_.push_back(clientPollFd);
        tcpSessions_.push_back(TcpSession<ClientRequestHandler>(
            std::move(clientSocket), Socket::Type::ClientSocket, &pollFdSet_.back()
        ));
    }));
}

template<typename ClientRequestHandler>
int TcpMultiplexAcceptor<ClientRequestHandler>::inputOutputEventOnSessions() {
    auto result = poll(pollFdSet_.data(), pollFdSet_.size(), -1);
    if (result < 0) {
        throw std::runtime_error("error in working poll system call");
    }

    return result;
}

template<typename ClientRequestHandler>
void TcpMultiplexAcceptor<ClientRequestHandler>::loop() {
    auto readyCount = inputOutputEventOnSessions();
    for (auto i = 0; readyCount > 0 || i < tcpSessions_.size(); ++i, --readyCount) {
        tcpSessions_[i].handleEvent();
    }
}

template<typename ClientRequestHandler>
struct pollfd TcpMultiplexAcceptor<ClientRequestHandler>::makePollFd(const int socket, const short event) const {
    struct pollfd pollFd;
    pollFd.fd = socket;
    pollFd.events = event;
    return pollFd;
}

    template<typename ClientRequestHandler>
template<typename EventHandler>
TcpMultiplexAcceptor<ClientRequestHandler>::
        TcpSession<EventHandler>::TcpSession(Socket &&socket, const Socket::Type type,
                                             const struct pollfd *pollFd, const EventHandler &eventHandler):
        socket_(std::move(socket)),
        type_(type),
        pollFd_(pollFd),
        eventHandler_(std::forward<EventHandler>(eventHandler)) {
}

template<typename ClientRequestHandler>
template<typename EventHandler>
bool TcpMultiplexAcceptor<ClientRequestHandler>::TcpSession<EventHandler>::handleEvent() {
    if (pollFd_->revents == POLLRDNORM) {
        if (type_ == Socket::Type::ClientSocket) {
            io::StaticBuffer<1024> buff;
            socket_.receive(buff);
            eventHandler_(buff);
        } else {
            const int clientfd = native_socket::Accept(socket_.fd(), NULL, NULL);
            io::StaticBuffer<sizeof(clientfd)> buff;
            buff.write((io::Buffer::Byte*)&clientfd, sizeof(clientfd));
            eventHandler_(buff);
        }
    }

    return true;
}

} // namespace acceptor

} // namespace net