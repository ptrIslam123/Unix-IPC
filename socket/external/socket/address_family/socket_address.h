#pragma once

#include <sys/socket.h>

#include <memory>

#include <memory>
#include <optional>
#include <string>

#include "../socket_api/socket_api.h"
#include ".

namespace net {

namespace address {

class SocketAddress {
public:
    typedef sock_api::NativeSocketAddress Address;

    virtual int getFamily() const = 0;
    virtual int getType() const = 0;
    virtual Address *getAddress() const = 0;
    virtual socklen_t getAddressLen() const = 0;
    virtual ~SocketAddress() = default;
};

} // namespace address

} // namespace net
