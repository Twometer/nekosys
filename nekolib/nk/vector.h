#ifndef _VECTOR_H
#define _VECTOR_H

#include <stdlib.h>
#include <string.h>

namespace nk
{

    template <typename T>
    class Vector
    {
    private:
        T *data = nullptr;
        size_t index = 0;
        size_t capacity = 0;

    public:
        void Add(T obj)
        {
            EnsureCapacity(index + 1);
            data[index] = obj;
            index++;
        }

        T &At(size_t index)
        {
            return data[index];
        }

        size_t Size()
        {
            return index;
        }

        size_t Capacity()
        {
            return capacity;
        }

    private:
        void EnsureCapacity(size_t min)
        {
            if (min > capacity)
            {
                size_t newCapacity = min * 2;
                T *newData = (T*) malloc(sizeof(T) * newCapacity);
                if (this->data != nullptr)
                {
                    memcpy(newData, data, sizeof(T) * index);
                }
                free((void*) this->data);
                this->data = newData;
                this->capacity = newCapacity;
            }
        }
    };

} // namespace nk

#endif