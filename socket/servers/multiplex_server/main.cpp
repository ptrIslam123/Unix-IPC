#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

std::vector<int> clients;
fd_set allset, readset;
int maxfd = 0, ready = 0, connfd = 0;
bool isRunning = true;

void errorHandler(int res, const std::string errorMsg) {
    if (res < 0) {
        perror(errorMsg.c_str());
        exit(-1);
    }
}

void updateMaxfd() {
    maxfd = *std::max_element(clients.cbegin(), clients.cend());
}

void CloseConnection(int clientfd) {
    int res = close(clientfd);
    if (res < 0) {
        std::cout << "Can`t close client session" << std::endl;
    }

    std::cout << "Close client connection" << std::endl;
}

void CloseConnections() {
    std::for_each(clients.begin(), clients.end(), CloseConnection);
    clients.clear();
}

void handleRequest(int sockfd) {
    char buff[1024] = {0};
    ssize_t n = read(sockfd, buff, sizeof(buff));
    errorHandler(n, "read error");

    if (n == 0) {
        auto clientSession = std::find(clients.begin(), clients.end(), sockfd);
        if (clientSession == clients.cend()) {
            errorHandler(-1, "");
        }
        clients.erase(clientSession);
        close(sockfd);
        FD_CLR(sockfd, &allset);
        updateMaxfd();
        std::cout << "Close client connection" << std::endl;
        return;
    }

    const std::string data(buff);
    if (data == std::string("stop\r\n")) {
        isRunning = false;
        return;
    }

    n = write(sockfd, buff, strlen(buff));
    errorHandler(n, "write error");
}

int main(int argc, char **argv) {
    if (argc != 2) {
        errorHandler(-1, "Argument not passing");
    }

    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    errorHandler(listenfd, "net_api error");

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
    errorHandler(res, "makeListeningQueue error");

    char clientIpAddress[INET_ADDRSTRLEN] = {0};
    struct sockaddr_in clientAddress;
    socklen_t clientAddressLen = sizeof(clientAddress);

    maxfd = listenfd;
    FD_ZERO(&allset);
    FD_SET(listenfd, &allset);

    while (isRunning) {
        readset = allset;
        ready = select(maxfd + 1, &readset, NULL, NULL, NULL);
        errorHandler(ready, "select error");

        if (FD_ISSET(listenfd, &readset)) {
            connfd = accept(listenfd, (struct sockaddr*)&clientAddress, &clientAddressLen);
            errorHandler(connfd, "accept error");

            inet_ntop(AF_INET, &clientAddress, clientIpAddress, clientAddressLen);
            std::cout << "New client connection with ip address: " << clientIpAddress << std::endl;
            clients.push_back(connfd);
            FD_SET(connfd, &allset);
            updateMaxfd();
        }

        for (auto i = 0; i < clients.size(); ++i) {
            connfd = clients[i];
            if (FD_ISSET(connfd, &readset)) {
                handleRequest(connfd);
            }
        }
    }

    CloseConnections();
    close(listenfd);
    std::cout << "Stop server" << std::endl;
    return 0;
}