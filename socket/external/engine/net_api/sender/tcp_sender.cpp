#include "tcp_sender.h"

namespace net {

TcpSender::TcpSender(net::Socket &&socket):
socket_(std::move(socket)) {
    socket_.connect();
}

Socket::IOResult TcpSender::send(const io::Buffer &buffer) {
    return socket_.write(buffer, buffer.size());
}

} // namespace net