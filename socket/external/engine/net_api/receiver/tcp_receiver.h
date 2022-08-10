#pragma once

#include <vector>
#include <utility>

#include "socket/socket.h"
#include "tcp_listener/tcp_listener.h"
#include "acceptor/multiplex_acceptor/tcp_multiplex_acceptor.h"

namespace net {

template<size_t MessageCapacity>
class TcpReceiver {
public:
    typedef std::pair<Socket, io::StaticBuffer<MessageCapacity>> Message;
    typedef std::vector<Message> Messages;

    TcpReceiver(tcp::TcpListener &&tcpListener, size_t senderCount = 1);
    const Messages &receive(size_t expectedMessagesSize = 1);

    void setSenderCount(size_t senderCount);

private:
    typedef acceptor::TcpMultiplexAcceptor::ClientRequestHandler RequestHandler;

    Messages messages_;
    RequestHandler requestHandler_;
    acceptor::TcpMultiplexAcceptor acceptor_;
};

template<size_t MessageCapacity>
TcpReceiver<MessageCapacity>::TcpReceiver(tcp::TcpListener &&tcpListener, const size_t senderCount):
messages_(),
requestHandler_([this](Socket &&socket){
    io::StaticBuffer<MessageCapacity> buffer;
    socket.read(buffer, buffer.capacity());
    messages_.template emplace_back(socket, std::move(buffer));
    return true;
}),
acceptor_(std::move(tcpListener), requestHandler_){
    messages_.reserve(senderCount);
}

template<size_t MessageCapacity>
const typename TcpReceiver<MessageCapacity>::Messages
&TcpReceiver<MessageCapacity>::receive(const size_t expectedMessagesSize) {
    do {
        auto readyCount = acceptor_.getCountReadyTcpSessions();
        acceptor_.handleClientsSocket(readyCount);
    } while (messages_.size() < expectedMessagesSize);
    return messages_;
}

template<size_t MessageCapacity>
void TcpReceiver<MessageCapacity>::setSenderCount(size_t senderCount) {
    messages_.resize(senderCount);
}

} // namespace net