#pragma once

#include <sys/socket.h>

namespace native_socket {

int MakeSocket(int family, int type);

void BindSocket(int socket, const struct sockaddr *socketAddress, socklen_t socketAddressLen);

void Connect(int socket, struct sockaddr *socketAddress, socklen_t socketAddressLen);

void MakeListenQueue(int socket, size_t queueSize);

int Accept(int socket, struct sockaddr *clientAddress, socklen_t *clientAddressLen);

void CloseSocket(int socket);

} // namespace native_socket