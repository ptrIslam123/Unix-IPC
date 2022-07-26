#pragma once

#include <cstdlib>

namespace io {

class Buffer {
public:
    typedef char Byte;

    Buffer();
    virtual ~Buffer() = default;

    virtual size_t read(Byte *data, size_t size);
    virtual size_t write(Byte *data, size_t size);
    virtual Byte *data() = 0;
    virtual const Byte *data() const = 0;

    void clear();
    bool isGood() const;
    size_t size() const;
    size_t capacity() const;

protected:
    size_t copy(Byte *src, Byte *dst, size_t size);
    void setSize(size_t size);
    void setCapacity(size_t capacity);
    void setIsGood(bool isGood);

private:
    size_t size_;
    size_t capacity_;
    bool isGood_;
};

} // namespace io
