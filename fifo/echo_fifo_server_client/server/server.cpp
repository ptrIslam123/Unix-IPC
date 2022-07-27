#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <pthread.h>

#include <string>
#include <iostream>
#include <array>
#include <vector>
#include <iterator>
#include <algorithm>
#include <cstring>

typedef std::array<char, 1024> Buffer;

struct Package {
    enum class Type {
        Message,
        NewConnection,
        ConnectionSuccess,
        ConectionClose
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

void CloseFifo(const std::string &fifoName) {
    if (unlink(fifoName.c_str()) < 0) {
        throw std::runtime_error("error when try close fifo: " + fifoName);
    }
}

typedef std::vector<int> Connections;
Connections connections(1024);

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
            std::cout << "\t\tReadFrom::Getting data from client: " << buffer.data() << std::endl;
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

void ClearBuffer(Buffer &buffer) {
    std::memset(buffer.data(), 0, buffer.size());
}

int Accept(const int serverFd) {
    Buffer buffer = {0};
    ReadFrom(serverFd, buffer);
    Package package;
    package << buffer;
    ClearBuffer(buffer);

    if (package.type() == Package::Type::NewConnection) {
        std::cout << "\t\tAccept::Attempt open new private session with client fifo: "
                  << package.data() << std::endl;

        const int clientFd = OpenFile(package.data(), O_WRONLY | O_NONBLOCK);
        connections.push_back(clientFd);

        buffer << Package(serverFd, Package::Type::ConnectionSuccess);
        WriteTo(clientFd, buffer);

        return clientFd;
    } else if (package.type() == Package::Type::Message) {
        const auto result = std::find(connections.begin(), connections.end(), package.fd());
        if (result != connections.end()) {
            return *result;
        }
    }

    return -1;
}

void SessionClose(const int clientFd) {
    auto result = std::find(connections.cbegin(), connections.cend(), clientFd);
    if (result != connections.cend()) {
        connections.erase(result);
        CloseFile(clientFd);
        std::cout << "\t\tSessionClose::Close private session with client\n" << std::endl;
    } else {
        std::cout << "\t\tSessionClose::Attempt close don`t exist client session" << std::endl;
    }
}

void CreateNewSession(const int clientFd, const int serverFd) {
    Package package;
    Buffer buffer = {0};
    ReadFrom(serverFd, buffer);
    package << buffer;

    if (package.type() == Package::Type::ConectionClose) {
        SessionClose(clientFd);
        return;
    }

    std::cout << "client message: " << package.data() << std::endl;
    WriteTo(clientFd, buffer);
}

int main(int argc, char **argv) {
    if (argc != 2) {
        throw std::runtime_error("no input arguments specified");
    }
    // Протокол подключения к серверу:
    // Сервер стартует и создает публичный fifo(READ ONLY) для принятия входящих сообщений от клиентов
    // Полученое сообщние от клиента должно содержать название его приватного fifo, который будет открыт сервером
    // только на чтение, а клиентов только на запись. Это подразумевает что клиент должен сам создать
    // приватную очередь и перед отправкой сообщения он должен открыть эту очередь.

    const std::string serverFifoName(argv[1]);
    (void) CreateFifo(serverFifoName);
    int serverFd = OpenFile(serverFifoName, O_RDONLY);

    while (true) {
        const int clientFd = Accept(serverFd);
        if (clientFd < 0) {
            std::cout << "\t\tmain::Accept error" << std::endl;
            break;
        }

        std::cout << "\t\tmain::New connection: " << clientFd << std::endl;
        CreateNewSession(clientFd, serverFd);
    }

    CloseFifo(serverFifoName);
    CloseFile(serverFd);

    return 0;
}
