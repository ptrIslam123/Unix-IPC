#pragma once

#include <utility>

#include "socket/socket.h"
#include "address_family/socket_address_ipv4.h"
#include "tcp_listener/tcp_listener.h"
#include "receiver/tcp_receiver.h"
#include "sender/tcp_sender.h"
#include "../buffers/static_buffer.h"
#include "../utils/result/result.h"

namespace net {

template<size_t MessageCapacity>
using TcpChannel = std::pair<TcpReceiver<MessageCapacity>, TcpSender>;

template<size_t MessageCapacity>
TcpChannel<MessageCapacity> MakeChannel(const short port) {
    TcpSender sender(Socket(
            std::make_unique<net::address::SocketAddressIpv4>("localhost", port)));

    //! TcpMultiplexAcceptor внутри TcpReceiver некопируемый и не перемещаемый,
    //! нужно сначало решить эту проблему
    net::tcp::TcpListener listener(Socket(
            std::make_unique<net::address::SocketAddressIpv4>(std::nullopt, port)), 10);
    TcpReceiver<MessageCapacity> receiver(std::move(listener), 1);

    return std::make_pair(std::move(receiver), std::move(sender));
}

} // namespace net