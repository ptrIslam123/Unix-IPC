#include "dynamic_buffer.h"

namespace io {

DynamicBuffer::DynamicBuffer(size_t size):
data_(size) {
}

Buffer::Byte *DynamicBuffer::data() {
    return data_.data();
}

const Buffer::Byte *DynamicBuffer::data() const {
    return data_.data();
}

} // namespace io
