#ifndef _STACK_H
#define _STACK_H

#include <stdint.h>
#include <stddef.h>

namespace Memory
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

        uint8_t *GetStackPtr();

        uint8_t *GetStackBottom();
    };

}; // namespace Memory

#endif