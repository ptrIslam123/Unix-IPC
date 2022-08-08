#include "native_socket_api.h"

#include <unistd.h>

#include <stdexcept>

namespace native_socket {

int MakeSocket(const int family, const int type) {
    int socketfd = socket(family, type, 0);
    if (socketfd < 0) {
        throw std::runtime_error("error when try create net_api");
    }

    return socketfd;
}

void BindSocket(const int socket, const struct sockaddr *const socketAddress,
        const socklen_t socketAddressLen) {
    int result = bind(socket, socketAddress, socketAddressLen);
    if (result < 0) {
        throw std::runtime_error("error when try bind net_api address to net_api");
    }
}

void Connect(int socket, struct sockaddr *socketAddress, socklen_t socketAddressLen) {
    int result = connect(socket, socketAddress, socketAddressLen);
    if (result < 0) {
        throw std::runtime_error("error when try connect to multiplex_server");
    }
}

void MakeListenQueue(const int socket, const size_t queueSize) {
    int result = listen(socket, queueSize);
    if (result < 0) {
        throw std::runtime_error("error when try create queue for makeListeningQueue socket");
    }
}

int Accept(const int socket, struct sockaddr *const clientAddress, socklen_t *const clientAddressLen) {
    int result = accept(socket, clientAddress, clientAddressLen);
    if (result < 0) {
        throw std::runtime_error("error when try accept new simple_tcp_client connection");
    }

    return result;
}

void CloseSocket(const int socket) {
    int result = close(socket);
    if (result < 0) {
        throw std::runtime_error("error when try close active net_api");
    }
}

} // namespace native_socket
