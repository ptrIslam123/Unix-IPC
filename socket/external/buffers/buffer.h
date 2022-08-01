#pragma once

#include <cstdlib>

namespace io {

class Buffer {
public:
    typedef char Byte;

    Buffer();
    virtual ~Buffer() = default;

    virtual size_t read(Byte *data) = 0;
    virtual size_t write(Byte *data, size_t size) = 0;
    virtual Byte *data() = 0;

    void clear();
    bool isGood() const;
    size_t size() const;
    size_t capacity() const;

protected:
    void setSize(size_t size);
    void setCapacity(size_t capacity);
    void setIsGood(bool isGood);

private:
    size_t size_;
    size_t capacity_;
    bool isGood_;
};

} // namespace io
