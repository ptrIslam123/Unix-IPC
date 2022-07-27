#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <pthread.h>
#include <cassert>

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

/**
 * @brief Открывает файл и возвращает дескриптор на открытый файл.
 * @detail Стойт заметить особенность работы системного вызова open для fifo.
 * В блокирующем состояниий при попытке открыть fifo через open на чтнение, процесс будет заблокирован до тех пор
 * пока не появиться процесс, который откроет этот же fifo на запись. После того как другой процесс запишет данные в данный fifo,
 * первый процесс разблокируется. В режиме неблокирующего ввода/вывода системный вызов open сразу возвращает упровление как только выделит дескриптор
 * асоциированный на указанный fifo.
 **/
int OpenFile(const std::string &fileName, const int flags) {
    const int fd = open(fileName.c_str(), flags);
    if (fd < 0) {
        throw std::runtime_error("error when try open created fifo file: " + fileName);
    }

    return fd;
}

int CreateFifo(const std::string &fifoName) {
    int fifo = mkfifo(fifoName.c_str(), S_IRWXU);
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

/**
 * @briedf Читает из буфера данные, отправленые в fifo клиентом.
 * @detail Системный вызов read для дескриптора в неблокирующем состоянии будет вовзращать количество прочитанных байт
 * равным 0 если кдиент еще ничего не записал в fifo, когда данные будут готовы(клиент что-то напишет в fifo) то read вернет количество
 * прочитанных байт. Значение -1 будет говорить об ошибке при попытке чтения по этому дескриптору.
 **/
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

/**
 * @briedf Записывает данные из буфера по переданному дескриптору.
 * @detail Ничего необычного, просто вызаем системный вызов write, но есть одна тонкость.
 * При неблокирующем режиме работы дескриптора вызов write на fifo который никто сейчас не читает(имеется
 * ввиду что не читает ни один клиент на момент осуществления write сервером) то write вернет -1 как результат ошибки, что по сути
 * ошибкой не является. Поэтому необходимо синхронизация клиента и сервера при попытке записи по неблокирующему дескриптору на fifo.
 **/
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

void CopyToBuffer(const std::string &msg, Buffer &buffer) {
    std::memcpy(buffer.data(), msg.data(), msg.size());
}

struct Package {
    enum class Type {
        Message,
        NewConnection,
        ConnectionSuccess,
        ConnectionClose
    };

    Package(const int fd = 0, const Package::Type type = Type::Message, const std::string &data = std::string()):
            type_(type), data_(data), fd_(fd) {}

    int fd() const {
        return fd_;
    }

    std::string &data() {
        return data_;
    }

    Type &type() {
        return type_;
    }

    Buffer &serialize(Buffer &buffer) const {
        size_t offset = 0;

        std::memcpy(buffer.data() + offset, &type_, sizeof(Type));
        offset += sizeof(Type);

        const size_t size = data_.size();
        std::memcpy(buffer.data() + offset, &size, sizeof(size_t));
        offset += sizeof(size_t);

        std::memcpy(buffer.data() + offset, data_.data(), data_.size());
        return buffer;
    }

    Package &deserialize(const Buffer &buffer) {
        size_t offset = 0;
        std::memcpy(&type_, buffer.data() + offset, sizeof(Type));
        offset += sizeof(Type);

        size_t size = 0;
        std::memcpy(&size, buffer.data() + offset, sizeof(size_t));
        offset += sizeof(size_t);

        char *data = new char[size];
        std::memcpy(data, buffer.data() + offset, size * sizeof(char));
        data_ = std::string(data);

        delete data;
        data = nullptr;

        return *this;
    }

private:
    Type type_;
    std::string data_;
    int fd_;
};

Buffer &operator<<(Buffer& buffer, const Package &package) {
    return package.serialize(buffer);
}

Package &operator<<(Package &package, const Buffer &buffer) {
    return package.deserialize(buffer);
}

void ClearBuffer(Buffer &buffer) {
    std::memset(buffer.data(), 0, buffer.size());
}

int main(int argc, char **argv) {
    if (argc != 3) {
        throw std::runtime_error("no input arguments specified");
    }

    const std::string serverFifoName(argv[1]);
    const std::string clientFifoName(argv[2]);

    (void) CreateFifo(clientFifoName);
    int clientFd = OpenFile(clientFifoName, O_RDONLY | O_NONBLOCK);
    int serverFd = OpenFile(serverFifoName, O_WRONLY);

    const Package clientFifoNamePackage(clientFd, Package::Type::NewConnection, clientFifoName);

    Buffer buffer = {0};
    buffer << clientFifoNamePackage;
    WriteTo(serverFd, buffer);
    ClearBuffer(buffer);

    ReadFrom(clientFd, buffer);
    Package connectStatus;
    connectStatus << buffer;
    ClearBuffer(buffer);
    if (connectStatus.type() != Package::Type::ConnectionSuccess) {
        std::cout << "\t\tConnection is failure" << std::endl;
        return -1;
    }


    buffer << Package(clientFd, Package::Type::Message, "Hello world from client!");
    WriteTo(serverFd, buffer);
    ClearBuffer(buffer);

    Package response;
    ReadFrom(clientFd, buffer);
    response << buffer;
    ClearBuffer(buffer);

    std::cout << "server response: " << response.data() << std::endl;

    buffer << Package(clientFd, Package::Type::ConnectionClose);
    WriteTo(serverFd, buffer);

    CloseFifo(clientFifoName);
    // Если клиент не успевает закрыть соединение до конца сесий с сервером,
    // вызов read вызывает UB. Но нормально ведет себя если сервер открывает свой fifo в
    // блокирующем режиме
    CloseFile(serverFd);
    CloseFile(clientFd);

    return 0;
}