#pragma once

#include <vector>
#include <utility>

#include "socket/socket.h"
#include "tcp_listener/tcp_listener.h"
#include "acceptor/multiplex_acceptor/tcp_multiplex_acceptor.h"

namespace net {

class TcpReceiver {
public:
    typedef std::pair<Socket, std::unique_ptr<io::Buffer>> Message;
    typedef std::vector<Message> Messages;

    TcpReceiver(tcp::TcpListener &&tcpListener);
    const Messages &receive(size_t expectedMessagesSize = 1);

private:
    typedef acceptor::TcpMultiplexAcceptor::ClientRequestHandler RequestHandler;

    Messages messages_;
    RequestHandler requestHandler_;
    acceptor::TcpMultiplexAcceptor acceptor_;
};

} // namespace net