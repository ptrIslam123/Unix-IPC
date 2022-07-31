#include "io_operations_api.h"

#include <cstring>

#include <stdexcept>

namespace io_operation {

size_t ReadFrom(int fd, char *const buffer, const size_t size) {
    ssize_t n = 0;
    n = read(fd, buffer, size);
    if (n < 0) {
        throw std::runtime_error("error when attempt read data from file descriptor");
    }

    return n;
}

size_t  WriteTo(int fd, const char *const buffer, const size_t size) {
    ssize_t n = 0;
    n = write(fd, buffer, size);
    if (n < 0) {
        throw std::runtime_error("error when attempt read data from file descriptor");
    }

    return n;
}

void ClearBuffer(char *buffer, const size_t size) {
    std::memset(buffer, 0, size);
}

} // io_operation
