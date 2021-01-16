#include <stdio.h>
#include <string.h>
#include <kernel/stack.h>

namespace Kernel
{

    Stack::Stack(void *stack_bottom, size_t size)
    {
        this->stack_bottom = (uint8_t *)stack_bottom;
        this->stack_top = this->stack_bottom + size;
        this->stack_ptr = this->stack_top;
    }

    Stack::~Stack()
    {
        delete[] stack_bottom;
    }

    void Stack::Push(uint32_t data)
    {
        size_t data_size = sizeof(data);
        stack_ptr -= data_size;
        memcpy(stack_ptr, &data, data_size);
    }

    void *Stack::GetStackPtr()
    {
        return stack_ptr;
    }

}; // namespace Kernel