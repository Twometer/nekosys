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
        Stack(void *stack_bottom, size_t size);

        Stack(void *stack_ptr);

        void Push(uint32_t data);

        uint32_t Pop();

        void *GetStackPtr();
    };

}; // namespace Kernel

#endif