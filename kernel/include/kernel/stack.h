#ifndef _STACK_H
#define _STACK_H

#include <stdint.h>
#include <stddef.h>

namespace Kernel
{

    class Stack
    {
    private:
        uint8_t *stack_bottom;
        uint8_t *stack_top;
        uint8_t *stack_ptr;

    public:
        Stack(size_t size);

        ~Stack();

        void push(uint32_t data);

        void *get_stack_ptr();
    };

}; // namespace Kernel

#endif