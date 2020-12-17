#ifndef _TTY_H
#define _TTY_H

#include <stddef.h>
#include <stdint.h>

namespace Kernel
{
    class TTY
    {
    public:
        static void Clear();
        static void SetColor(uint8_t color);
        static void PutChar(char c);
        static void Scroll();
        static void Write(const char *data, size_t size);
        static void Print(const char *string);
        static void SetCursorPos(size_t x, size_t y);

    private:
        static void PutEntry(char c, uint8_t color, size_t x, size_t y);
    };

}; // namespace Kernel

#endif
