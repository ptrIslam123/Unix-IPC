#pragma once

#include "socket_address.h"

#include <netinet/in.h>

#include <string>
#include <optional>

namespace net {

namespace address {

class SocketAddressIpv4 : public SocketAddress {
public:
    typedef std::optional<std::string> IpAddress;

    SocketAddressIpv4(const struct sockaddr_in &socketAddress, short port);
    SocketAddressIpv4(const IpAddress &ipaddress, short port);

protected:
       virtual int getFamily() const override;
       virtual int getType() const override;
       virtual SocketAddress::Address *getAddress() const override;
       virtual socklen_t getAddressLen() const override;
       virtual const std::string_view getAddressStr() override;
       virtual std::unique_ptr<SocketAddress> copy() const override;

private:
    void fellAddress();

    IpAddress ipAddress_;
    short port_;
    struct sockaddr_in socketAddress_;
};

} // namespace address

} // namespace net_api
