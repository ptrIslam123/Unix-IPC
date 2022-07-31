#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include <string>
#include <iostream>

void errorHandler(int res, const std::string errorMsg) {
    if (res < 0) {
        perror(errorMsg.c_str());
        exit(-1);
    }
}

int main(int argc, char **argv) {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    errorHandler(sockfd, "net_api erorr");

    struct sockaddr_in serverAddress;
    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    int res = inet_pton(AF_INET, argv[1], &serverAddress);
    errorHandler(res, "set server ip address error");
    serverAddress.sin_port = htons(std::stoi(argv[2]));

    res = connect(sockfd, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
    errorHandler(res, "connect error");

    bool isRunning = true;
    char buff[1024] = {0};
    ssize_t n = 0;
    while (isRunning) {
        std::cin >> buff;
        const size_t bufflen = strlen(buff);
        buff[bufflen] = '\r';
        buff[bufflen + 1] = '\n';

        const std::string data(buff);
        if (data == std::string("close\r\n")) {
            break;
        } else if (data == std::string("stop\r\n")) {
            isRunning = false;
        }

        n = write(sockfd, buff, strlen(buff));
        errorHandler(n, "write error");
        n = read(sockfd, buff, sizeof(buff));
        errorHandler(n, "read error");

        std::cout << "data from server: " << buff << std::endl;
        memset(&buff, 0, sizeof(buff));
    }

    close(sockfd);
    std::cout << "Close client" << std::endl;
    return 0;
}