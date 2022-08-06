#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <poll.h>

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

typedef std::vector<struct pollfd> Clients;
Clients clients;
bool isRunning = true;

void errorHandler(int res, const std::string errorMsg) {
    if (res < 0) {
        perror(errorMsg.c_str());
        exit(-1);
    }
}

void CloseOpeningFileDescriptors() {
    std::transform(clients.cbegin(), clients.cend(), clients.begin(), [](struct pollfd pollfdItem){
        errorHandler(close(pollfdItem.fd), "close error");
        return pollfdItem;
    });
    clients.clear();
}

void handleRequest(int sockfd) {
    char buff[1024] = {0};
    ssize_t n = read(sockfd, buff, sizeof(buff));
    errorHandler(n, "read error");

    if (n == 0) {
        auto clientPollfd = std::find_if(clients.cbegin(), clients.cend(),
                                    [sockfd](const struct pollfd clientPollfd) {
            return clientPollfd.fd == sockfd;
        });
        if (clientPollfd == clients.cend()) {
            errorHandler(-1, "");
        }
        clients.erase(clientPollfd);
        close(sockfd);
        std::cout << "Close simple_client connection" << std::endl;
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

    int res = bind(listenfd, (struct sockaddr *) &serverAddress, sizeof(serverAddress));
    errorHandler(res, "bind error");

    std::cout << "Create queue for incoming connection" << std::endl;
    res = listen(listenfd, 10);
    errorHandler(res, "makeListeningQueue error");

    char clientIpAddress[INET_ADDRSTRLEN] = {0};
    struct sockaddr_in clientAddress;
    socklen_t clientAddressLen = sizeof(clientAddress);

    ssize_t nready = 0;
    int connfd = 0;
    struct pollfd listenPollfd;
    listenPollfd.fd = listenfd;
    listenPollfd.events = POLLRDNORM;
    clients.push_back(listenPollfd);

    while (isRunning) {
        nready = poll(clients.data(), clients.size(), -1);
        errorHandler(nready, "poll error");

        if (clients[0].revents == POLLRDNORM) {
            connfd = accept(listenfd, (struct sockaddr*)&clientAddress, &clientAddressLen);
            errorHandler(connfd, "accept error");

            inet_ntop(AF_INET, &clientAddress, clientIpAddress, clientAddressLen);
            std::cout << "New simple_client connection with ip address: " << clientIpAddress << std::endl;

            struct pollfd clientPollfd;
            clientPollfd.events = POLLRDNORM;
            clientPollfd.fd = connfd;
            clients.push_back(clientPollfd);

            if (nready < 2) {
                continue; // готов только дескриптор для принятия соединений
            }
        }

        for (auto i = 1; i < clients.size(); ++i) {
            if (clients[i].revents == POLLRDNORM) {
                handleRequest(clients[i].fd);
            }
        }

    }

    CloseOpeningFileDescriptors();
    std::cout << "Stop multiplex_server" << std::endl;

    return 0;
}