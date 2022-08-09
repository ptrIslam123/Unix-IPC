#pragma once

#include "socket/socket.h"
#include "../buffers/buffer.h"

namespace net {

class TcpSender {
public:
    TcpSender(Socket &&socket);

    Socket::IOResult send(const io::Buffer &buffer);

private:
    Socket socket_;
};

} // namespace net