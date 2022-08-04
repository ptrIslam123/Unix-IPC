#pragma once

#include <sys/socket.h>

#include <memory>

#include <memory>
#include <optional>
#include <string>
#include <string_view>

namespace net {

namespace address {

class SocketAddress {
public:
    typedef struct sockaddr Address;

    virtual int getFamily() const = 0;
    virtual int getType() const = 0;
    virtual Address *getAddress() const = 0;
    virtual socklen_t getAddressLen() const = 0;
    virtual const std::string_view getAddressStr() = 0;
    virtual std::unique_ptr<SocketAddress> copy() const  = 0;
    virtual ~SocketAddress() = default;
};

} // namespace address

} // namespace net_api
