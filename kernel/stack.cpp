#include <stdio.h>
#include <string.h>
#include <kernel/stack.h>

namespace Kernel
{

    Stack::Stack(size_t size)
    {
        stack_bottom = new uint8_t[size];
        stack_top = stack_bottom + size;
        stack_ptr = stack_top;
    }

    Stack::~Stack()
    {
        delete[] stack_bottom;
    }

    void Stack::push(uint32_t data)
    {
        size_t data_size = sizeof(data);
        stack_ptr -= data_size;
        memcpy(stack_ptr, &data, data_size);
    }

    void *Stack::get_stack_ptr()
    {
        return stack_ptr;
    }

}; // namespace Kernel