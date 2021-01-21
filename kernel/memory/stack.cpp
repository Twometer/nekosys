#include <stdio.h>
#include <string.h>
#include <memory/stack.h>

namespace Memory
{

    Stack::Stack(void *stack_bottom, size_t size)
    {
        this->stack_bottom = (uint8_t *)stack_bottom;
        this->stack_top = this->stack_bottom + size;
        this->stack_ptr = this->stack_top;
    }

    Stack::Stack(void *stack_ptr)
    {
        this->stack_bottom = nullptr;
        this->stack_top = nullptr;
        this->stack_ptr = (uint8_t *)stack_ptr;
    }

    void Stack::Push(uint32_t data)
    {
        size_t data_size = sizeof(data);
        stack_ptr -= data_size;
        memcpy(stack_ptr, &data, data_size);
    }

    uint32_t Stack::Pop()
    {
        uint32_t *ptr = (uint32_t *)stack_ptr;
        stack_ptr += sizeof(uint32_t);
        return *ptr;
    }

    void *Stack::GetStackPtr()
    {
        return stack_ptr;
    }

}; // namespace Memory