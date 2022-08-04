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
    Socket(int fd, Address &&address);
    Socket(Socket &&other);
    Socket &operator=(Socket &&other);

    Socket(const Socket &other) = delete;
    Socket &operator=(const Socket &other) = delete;
    ~Socket() = default;

    void bind();
    void connect();
    void makeListeningQueue(size_t queueSize);

    void read(io::Buffer &buffer, size_t size);
    void write(io::Buffer &buffer, size_t size);

    int fd() const;
    const std::string_view addressStr();
    Socket copy() const;

    Socket accept();

private:
    int socket_;
    std::optional<Address> address_;
};

} // namespace net_api