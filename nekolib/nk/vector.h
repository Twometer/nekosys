#include <stdlib.h>

namespace nk
{

    template <typename T>
    class Vector
    {
    private:
        T *data = nullptr;
        int index = 0;
        int capacity = 0;

    public:
        Vector()
        {
            EnsureCapacity(5);
        }

        void Add(T obj)
        {
            EnsureCapacity(index + 1);
            data[index] = T;
            index++;
        }

    private:
        void EnsureCapacity(int min)
        {
            if (min > capacity)
            {
                int newCapacity = min * 2;
                T *newData = malloc(sizeof(T) * newCapacity);
                if (this->data != nullptr)
                {
                    memcpy(newData, data, sizeof(T) * index);
                }
                this->data = newData;
                this->capacity = newCapacity;
            }
        }
    };

} // namespace nk