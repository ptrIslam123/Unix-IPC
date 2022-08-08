#pragma once

#include "socket/socket.h"

namespace net {

namespace tcp {
    
class TcpListener {
public:
    TcpListener(Socket::Address &&address, size_t listenQueueSize);
    TcpListener(Socket &&listenerSocket, size_t listenQueueSize);
    Socket getSocket();

private:
    Socket::OperationResult initSocket(size_t listenQueueSize);

    Socket socket_;
};

} // namespace tcp

} // namespace net