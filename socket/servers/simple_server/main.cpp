#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <string>
#include <iostream>

bool isRunServer = true;

void errorHandler(int res, const std::string errorMsg) {
    if (res < 0) {
        perror(errorMsg.c_str());
        exit(-1);
    }
}

void handleConnect(int sockfd) {
    char buff[1024] = {0};
    ssize_t n = 0;
    while ((n = read(sockfd, buff, sizeof(buff))) > 0) {
        const std::string data(buff);
        if (data == std::string("stop\r\n")) {
            isRunServer = false;
            break;
        }

        n = write(sockfd, buff, sizeof(buff));
        errorHandler(n, "write error");
        memset(&buff, 0, sizeof(buff));
    }

    if (n == 0) {
        return;
    } else {
        errorHandler(n, "read error");
    }
}

int main(int argc, char **argv) {
    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    errorHandler(listenfd, "socket error");

    struct sockaddr_in serverAddress;
    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    // для указания номера порта сервера с сетевым порядком байт
    // нужно использовать только функцию htons!!!
    serverAddress.sin_port = htons(std::stoi(argv[1]));

    int res = bind(listenfd, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
    errorHandler(res, "bind error");

    std::cout << "Create queue for incoming connection" << std::endl;
    res = listen(listenfd, 10);
    errorHandler(res, "listen error");

    char clientIpAddress[INET_ADDRSTRLEN] = {0};
    struct sockaddr_in clientAddress;
    socklen_t clientAddressLen = sizeof(clientAddress);
    while (isRunServer) {
        memset(&clientAddress, 0, clientAddressLen);
        int connfd = accept(listenfd, (struct sockaddr*)&clientAddress, &clientAddressLen);
        errorHandler(connfd, "accept error");

        inet_ntop(AF_INET, &clientAddress, clientIpAddress, clientAddressLen);
        std::cout << "New client connection with ip: " << clientIpAddress << std::endl;
        handleConnect(connfd);

        close(connfd);
        std::cout << "Close client connection" << std::endl;
    }

    close(listenfd);
    std::cout << "Stop server" << std::endl;

    return 0;
}
