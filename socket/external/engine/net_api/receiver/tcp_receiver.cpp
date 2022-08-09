#include "tcp_receiver.h"

namespace net {

TcpReceiver::TcpReceiver(tcp::TcpListener &&tcpListener):
        messages_(),
        requestHandler_([this](Socket &&socket){
    io::StaticBuffer<1024> buffer;
    socket.read(buffer, buffer.capacity());
    auto message = std::make_pair(socket,
                                  std::unique_ptr<io::Buffer>(new io::StaticBuffer<1024>(buffer)));
    messages_.push_back(std::move(message));
    return true;
}),
        acceptor_(std::move(tcpListener), requestHandler_){
}

const TcpReceiver::Messages &TcpReceiver::receive(const size_t expectedMessagesSize) {
    do {
        auto readyCount = acceptor_.getCountReadyTcpSessions();
        acceptor_.handleClientsSocket(readyCount);
    } while (messages_.size() < expectedMessagesSize);
    return messages_;
}

} // namespace net