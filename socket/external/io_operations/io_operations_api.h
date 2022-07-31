#pragma once

#include <unistd.h>

namespace io_operation {

size_t ReadFrom(int fd, char *buffer, size_t size);

size_t WriteTo(int fd, const char *buffer, size_t size);

void ClearBuffer(char *buffer, size_t size);

} // namespace io_operation
