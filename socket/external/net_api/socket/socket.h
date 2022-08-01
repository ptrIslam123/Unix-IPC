#pragma once

#include "socket_address.h"
#include "buffers/buffer.h"

#include <memory>

namespace net {

class Socket {
public:
    Socket(std::unique_ptr<address::SocketAddress> &&address);
    ~Socket();

    void bind();
    void connect();
    void makeListeningQueue(size_t queueSize);

    void receive(io::Buffer &buffer);
    void send(io::Buffer &buffer);

    void close()    ;

    int fd() const;

private:
    int socket_;
    std::unique_ptr<address::SocketAddress> address_;
    bool isClosed_;
};

} // namespace net_api