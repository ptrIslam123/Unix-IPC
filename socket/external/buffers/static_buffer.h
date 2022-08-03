#pragma once

#include "buffer.h"

#include <array>
#include <cstring>

#include "io_operations_api.h"

namespace io {

template<size_t Capacity>
class StaticBuffer : public Buffer {
public:
    typedef std::array<Buffer::Byte, Capacity> BufferType;

    StaticBuffer();
    ~StaticBuffer() = default;

    virtual size_t read(Byte *data) override;
    virtual size_t write(Byte *data, size_t size) override;
    virtual Byte *data() override;

private:
    BufferType data_;
};

template<size_t Capacity>
StaticBuffer<Capacity>::StaticBuffer():
Buffer(),
data_() {
    setSize(0);
    setCapacity(Capacity);
}

template<size_t Capacity>
size_t StaticBuffer<Capacity>::read(Byte *data) {
    for (auto i = 0; i < size(); ++i) {
        data[i] = data_[i];
    }
    return size();
}

template<size_t Capacity>
size_t StaticBuffer<Capacity>::write(Byte *const data, const size_t size) {
    for (auto i = 0; i < size; ++i) {
        data_[i] = data[i];
    }
    setSize(size);
    return size;
}

template<size_t Capacity>
Buffer::Byte *StaticBuffer<Capacity>::data() {
    return data_.data();
}

} // namespace io
