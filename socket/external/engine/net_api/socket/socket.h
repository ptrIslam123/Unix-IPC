#pragma once

#include "address_family/socket_address.h"
#include "socket_state.h"
#include "../../buffers/buffer.h"
#include "../../utils/result/result.h"

#include <memory>

namespace net {

class Socket {
public:
    typedef std::shared_ptr<address::SocketAddress> Address;
    typedef util::Result<size_t, SocketState> IOResult;
    typedef util::Result<bool, std::string> OperationResult;

    enum class Type {
        Listener,
        Usual
    };

    Socket(Address &&address, Type type);
    Socket(int fd, const Address &address, Type type);
    Socket(const Socket &other);
    Socket(Socket &&other) noexcept ;
    Socket &operator=(const Socket &other);
    Socket &operator=(Socket &&other);
    ~Socket();

    void bind();
    void connect();
    void makeListeningQueue(size_t queueSize);
    Socket accept();

    IOResult read(io::Buffer &buffer, size_t size);
    IOResult write(const io::Buffer &buffer, size_t size);

    int fd() const;
    std::optional<Address> getAddress() const;
    const std::string_view addressStr();
    void close();

private:
    OperationResult init();

    int socket_;
    Type type_;
    std::optional<Address> address_;
};

} // namespace net_api
