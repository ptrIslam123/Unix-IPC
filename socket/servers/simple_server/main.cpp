#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <string>
#include <iostream>
#include <array>

#include "engine/net_api/socket/native_socket_api.h"
#include "engine/buffers/io_operations_api.h"

typedef std::array<char, 1024> Buffer;

bool isRunServer = true;

void errorHandler(int res, const std::string errorMsg) {
    if (res < 0) {
        perror(errorMsg.c_str());
        exit(-1);
    }
}

void handleConnect(int sockfd) {
    Buffer buff = {0};
    size_t n = 0;
    while (true) {
        n = io_operation::ReadFrom(sockfd, buff.data(), buff.size());
        if (n == 0) {
            break;
        }

        const std::string data(buff.data());
        if (data == std::string("stop\r\n")) {
            isRunServer = false;
            break;
        }

        n = io_operation::WriteTo(sockfd, buff.data(), buff.size());
        io_operation::ClearBuffer(buff.data(), buff.size());
    }
}

int main(int argc, char **argv) {
    int listenfd = native_socket::MakeSocket(AF_INET, SOCK_STREAM);

    struct sockaddr_in serverAddress;
    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    // для указания номера порта сервера с сетевым порядком байт
    // нужно использовать только функцию htons!!!
    serverAddress.sin_port = htons(std::stoi(argv[1]));

    native_socket::BindSocket(listenfd, (struct sockaddr*)&serverAddress,
                                sizeof(serverAddress));

    std::cout << "Create queue for incoming connection" << std::endl;
    listen(listenfd, 10);

    char clientIpAddress[INET_ADDRSTRLEN] = {0};
    struct sockaddr_in clientAddress;
    socklen_t clientAddressLen = sizeof(clientAddress);
    while (isRunServer) {
        memset(&clientAddress, 0, clientAddressLen);
        int connfd = accept(listenfd, (struct sockaddr*)&clientAddress, &clientAddressLen);

        inet_ntop(AF_INET, &clientAddress, clientIpAddress, clientAddressLen);
        std::cout << "New simple_client connection with ip: " << clientIpAddress << std::endl;
        handleConnect(connfd);

        native_socket::CloseSocket(connfd);
        std::cout << "Close simple_client connection" << std::endl;
    }

    native_socket::CloseSocket(listenfd);
    std::cout << "Stop multiplex_server" << std::endl;

    return 0;
}
