#include "buffer.h"

#include <cstring>

namespace io {

Buffer::Buffer(Byte *const data, const size_t capacity):
data_(data),
size_(0),
capacity_(capacity),
isGood_(true) {
}

size_t Buffer::capacity() const {
    return capacity_;
}

size_t Buffer::size() const {
    return size_;
}

void Buffer::clear() {
    std::memset(data_, 0, capacity());
}

bool Buffer::isGood() const {
    return isGood_;
}

void Buffer::setIsGood(const bool isGood) {
    isGood_ = isGood;
}

void Buffer::setSize(const size_t size) {
    size_ = size;
}

} // namespace io