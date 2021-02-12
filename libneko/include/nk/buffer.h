#ifndef _NK_BUFFER_H
#define _NK_BUFFER_H

#include <stdint.h>
#include <stddef.h>
#include <string.h>

namespace nk
{

    class Buffer
    {
    private:
        uint8_t *data;
        size_t size;

    public:
        explicit Buffer(uint8_t *data, size_t size)
            : data(data), size(size) {}

        uint8_t *At(size_t offset)
        {
            return data + offset;
        }

        template <typename T>
        T Read(size_t offset)
        {
            T data;
            memcpy(&data, this->data + offset, sizeof(T));
            return data;
        }

        template <typename T>
        void Write(size_t offset, const T &data)
        {
            memcpy(data + offset, &data, sizeof(T));
        }
    };

} // namespace nk

#endif