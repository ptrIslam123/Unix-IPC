#pragma once

#include <cstdlib>

namespace io {

class Buffer {
public:
    typedef char Byte;
    Buffer(Byte *data, size_t capacity);
    virtual ~Buffer() = default;

    virtual size_t read(Byte *data) = 0;
    virtual size_t write(const Byte *data) = 0;

    void clear();
    bool isGood() const;
    size_t size() const;
    size_t capacity() const;

protected:
    void setSize(size_t size);
    void setIsGood(bool isGood);

private:
    Byte *data_;
    size_t size_;
    size_t capacity_;
    bool isGood_;
};

} // namespace io
