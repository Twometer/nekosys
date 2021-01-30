#ifndef _TTY_H
#define _TTY_H

#include <stddef.h>
#include <stdint.h>

namespace Kernel
{

    class TTY
    {
    private:
        static uint32_t cursorX;
        static uint32_t cursorY;
        static uint32_t currentColor;

    public:
        static void SetCursorPos(size_t x, size_t y);
        static void SetColor(uint32_t color);
        static void PutChar(char c);
        static void Write(const char *string, size_t size);

        static void Clear();
        static void Scroll();
    };

}; // namespace Kernel

#endif
