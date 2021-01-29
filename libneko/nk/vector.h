#ifndef _NK_VECTOR_H
#define _NK_VECTOR_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

namespace nk
{

    template <typename T>
    class Vector
    {
    private:
        T *data = nullptr;
        size_t size = 0;
        size_t capacity = 0;

    public:
        void Add(T obj)
        {
            EnsureCapacity(size + 1);
            data[size] = obj;
            size++;
        }

        void Remove(size_t idx)
        {
            for (size_t i = idx + 1; i < size; i++)
            {
                data[i - 1] = data[i];
            }

            size--;
        }

        void RemoveBlock(size_t idx, size_t size)
        {
            memcpy(&data[idx], &data[idx + size], (this->size - (idx + size)) * sizeof(T));
            this->size -= size;
        }

        void Clear()
        {
            size = 0;
        }

        const T &At(size_t index) const
        {
            return data[index];
        }

        T &At(size_t index)
        {
            return data[index];
        }

        T &operator[](size_t index) const
        {
            return data[index];
        }

        size_t Size() const
        {
            return size;
        }

        size_t Capacity() const
        {
            return capacity;
        }

        int IndexOf(const T &value) const
        {
            for (size_t i = 0; i < size; i++)
                if (data[i] == value)
                    return i;
            return -1;
        }

        bool Contains(const T &value) const
        {
            return IndexOf(value) != -1;
        }

        ~Vector()
        {
            free((void *)data);
        }

        void Reserve(size_t min)
        {
            EnsureCapacity(min);
            if (min > size)
                size = min;
        }

    private:
        void EnsureCapacity(size_t min)
        {
            if (min > capacity)
            {
                size_t newCapacity = min * 2;
                T *newData = (T *)malloc(sizeof(T) * newCapacity);
                memset(newData, 0, sizeof(T) * newCapacity);
                if (this->data != nullptr)
                {
                    memcpy(newData, data, sizeof(T) * size);
                }
                free((void *)this->data);
                this->data = newData;
                this->capacity = newCapacity;
            }
        }
    };

} // namespace nk

#endif