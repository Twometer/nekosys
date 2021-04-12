#ifndef _NK_CIRCULARQUEUE_H
#define _NK_CIRCULARQUEUE_H

#include <stddef.h>

namespace nk
{

    template <typename T>
    class CircularQueue
    {
    private:
        int tail;
        int head;
        size_t size;
        T *data;

    public:
        CircularQueue(size_t size)
            : size(size),
              tail(-1),
              head(-1),
              data(new T[size])
        {
        }

        ~CircularQueue()
        {
            delete[] data;
        }

        void Enqueue(T value)
        {
            if (head == -1 && tail == -1)
                head = tail = 0;
            else
                tail = (tail + 1) % size;

            data[tail] = value;
        }

        T Dequeue()
        {
            auto elem = data[head];
            if (head == tail)
                head = tail = -1;
            else
                head = (head + 1) % size;
            return elem;
        }

        bool IsEmpty()
        {
            return head == -1 && tail == -1;
        }
    };

};

#endif