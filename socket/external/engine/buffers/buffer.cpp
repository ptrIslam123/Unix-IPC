#include "buffer.h"

#include <cstring>
#include <cassert>

namespace io {

Buffer::Buffer():
size_(0),
capacity_(0),
isGood_(true) {
}

size_t Buffer::capacity() const {
    return capacity_;
}

size_t Buffer::size() const {
    return size_;
}

void Buffer::clear() {
    std::memset(data(), 0, capacity());
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

void Buffer::setCapacity(const size_t capacity) {
    capacity_ = capacity;
}

size_t Buffer::read(Buffer::Byte *const data, const size_t size) {
    assert(size <= capacity());
    return copy(data(), data, size);
}

size_t Buffer::write(Buffer::Byte *const data, const size_t size) {
    assert(size <= capacity());
    return copy(data, data(), size);
}

size_t Buffer::copy(Buffer::Byte *const src, Buffer::Byte *const dst, size_t size) {
    for (auto i = 0; i < size; ++i) {
        dst[i] = src[i];
    }
    return size;
}

} // namespace io