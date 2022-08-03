#pragma once

#include "socket_address.h"
#include "buffers/buffer.h"

#include <memory>

namespace net {

class Socket {
public:

    typedef std::unique_ptr<address::SocketAddress> Address;

    enum class Type {
        ListenerSocket,
        ClientSocket
    };

    Socket(Address &&address);
    Socket(int fd);
    Socket(Socket &&other);
    Socket(const Socket &other) = delete;
    Socket operator=(Socket &&other);
    Socket &operator=(const Socket &other) = delete;
    ~Socket();

    void bind();
    void connect();
    void makeListeningQueue(size_t queueSize);

    void receive(io::Buffer &buffer);
    void send(io::Buffer &buffer);

    void close();

    int fd() const;

private:
    int socket_;
    std::optional<Address> address_;
    bool isClosed_;
};

} // namespace net_api