#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>

#include <string>
#include <iostream>
#include <array>
#include <cstring>

typedef std::array<char, 1024> Buffer;

void CloseFifo(const std::string &fifoName) {
    if (unlink(fifoName.c_str()) < 0) {
        throw std::runtime_error("error when try close fifo: " + fifoName);
    }
}

int OpenFile(const std::string &fileName, const int flags) {
    const int fd = open(fileName.c_str(), flags);
    if (fd < 0) {
        throw std::runtime_error("error when try open created fifo file: " + fileName);
    }

    return fd;
}

int CreateFifo(const std::string &fifoName, const mode_t mode) {
    int fifo = mkfifo(fifoName.c_str(), mode);
    if (fifo < 0) {
        throw std::runtime_error("error when try create fifo: " + fifoName);
    }

    return fifo;
}

int GetFileSize(const int fd) {
    struct stat status;
    int res = fstat(fd, &status);
    if (res < 0) {
        throw std::runtime_error("error when use system call stat");
    }

    return status.st_size;
}

void ReadFrom(const int fd, Buffer &buffer) {
    size_t size = 0;
    while (true) {
        size = read(fd, buffer.data(), buffer.size());
        if (size < 0) {
            throw std::runtime_error("error when try read from file descriptor");
        } else if (size == 0) {
            sleep(1);
            continue;
        } else {
            break;
        }
    }
}

void WriteTo(const int fd, const Buffer &buffer) {
    const int size = write(fd, buffer.data(), buffer.size());
    if (size < 0) {
        throw std::runtime_error("error when try write data to file descriptor");
    } else if (size < buffer.size()) {
        throw std::runtime_error("error when was write less then buffer size");
    }
}

void CloseFile(int fd) {
    if (close(fd) < 0) {
        throw std::runtime_error("error when close file descriptor");
    }
}


int main(int argc, char **argv) {
    if (argc != 3) {
        throw std::runtime_error("no input arguments specified");
    }

    // Открываем fifo сервера переданный через argv[1], это публичный fifo, по которому клиент подключается к серверу.
    int wfd = OpenFile(argv[1], O_NONBLOCK | O_WRONLY);
    // Создаем наш личный fifo, который мы отправим серверу в первом сообщении, что бы сервер создал 
    // личное соединение с нами, по которому мы получим сообщение от сервера.
    int rfifo = CreateFifo(argv[2], S_IRWXU);

    // Осуществляем отправку назания нашего созданного fifo серверу.
    Buffer buff = {0};
    std::memcpy(buff.data(), argv[2], strlen(argv[2]));
    WriteTo(wfd, buff);

    // Открываем наш созданный fifo на чтение.
    int rfd = OpenFile(argv[2], O_NONBLOCK | O_RDONLY);
    
    // Читаем что нам написал сервер по нашему личному каналу. 
    Buffer msg = {0};
    ReadFrom(rfd, msg);

    // Выводим сообщение от сервера на консоль.
    std::cout << msg.data() << std::endl;

    CloseFifo(argv[2]);
    CloseFile(rfd);
    CloseFile(wfd);

    return 0;
}
