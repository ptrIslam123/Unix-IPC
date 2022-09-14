#pragma once

#include "buffer.h"

#include <array>
#include <cstring>
#include <cassert>

#include "io_operations_api.h"

namespace io {

template<size_t Capacity>
class StaticBuffer : public Buffer {
public:
    typedef std::array<Buffer::Byte, Capacity> BufferType;

    StaticBuffer();
    ~StaticBuffer() = default;

    virtual Byte *data() override;
    virtual const Byte *data() const override;

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
Buffer::Byte *StaticBuffer<Capacity>::data() {
    return data_.data();
}

template<size_t Capacity>
const Buffer::Byte *StaticBuffer<Capacity>::data() const {
    return data_.data();
}

} // namespace io
