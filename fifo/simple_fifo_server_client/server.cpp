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

int CreateFifo(const std::string &fifoName, const mode_t mode, const int flags) {
    int fifo = mkfifo(fifoName.c_str(), mode);
    if (fifo < 0) {
        throw std::runtime_error("error when try create fifo: " + fifoName);
    }

    return OpenFile(fifoName.c_str(), flags);
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

int main(int argc, char **argv) {
    if (argc != 2) {
        throw std::runtime_error("no input arguments specified");
    }

    // Создаем новый fifo, переданный в argv[1] и открываем его на чтение в неблокирующем редиме работы
    int rfd = CreateFifo(argv[1], S_IRWXU, O_NONBLOCK | O_RDONLY);

    // Блокируемся на чтении пока какой нибудь клиент не напишет нам (пока что это активное ожидаение!).
    // Сервер предпологает что первое сообщение это название нового fifo, по которого сервер будет этому клиенту
    // слать данные, а этот клиент читать эти данные, так как одновременно читать и писать по одному и тому же fifo нельзя!
    Buffer buff = {0};
    ReadFrom(rfd, buff);

    // Здесь пытаемся открыть на чнение присланный нам от клиента fifo для передачи ему данных.
    // Важно что бы клиент к этмоу времени уже открыл данный fifo на чтение, иначе open вернет вернет ошибку.
    std::string fifoName(buff.data());
    int wfd = OpenFile(fifoName, O_NONBLOCK | O_WRONLY);

    // Отправляем обещенное сообщение клиенту по второму именованному каналу клиента.
    Buffer msg = {"Hello from server!"};
    WriteTo(wfd, msg);

    CloseFifo(argv[1]);
    CloseFile(rfd);
    CloseFile(wfd);

    return 0;
}
