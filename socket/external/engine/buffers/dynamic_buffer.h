#pragma once

#include <vector>

#include "buffer.h"

namespace io {

class DynamicBuffer : public Buffer {
public:
    typedef std::vector<Buffer::Byte> BufferType;

    explicit DynamicBuffer(size_t size = 10);
    ~DynamicBuffer() = default;

    virtual Byte *data() override;
    virtual const Byte *data() const override;

private:
    BufferType data_;
};

} // namespace io