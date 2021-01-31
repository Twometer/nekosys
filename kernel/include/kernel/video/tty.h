#ifndef _TTY_H
#define _TTY_H

#include <stddef.h>
#include <stdint.h>

namespace Video
{
    enum class TerminalColor
    {
        Black = 0,
        DarkGray = 8,
        DarkBlue = 1,
        LightBlue = 9,
        DarkGreen = 2,
        LightGreen = 10,
        DarkCyan = 3,
        LightCyan = 11,
        DarkRed = 4,
        LightRed = 12,
        DarkMagenta = 5,
        LightMagenta = 13,
        Brown = 6,
        Yellow = 14,
        LightGray = 7,
        White = 15
    };

    class TTY
    {
    protected:
        size_t width;
        size_t height;

        uint32_t cursorX;
        uint32_t cursorY;

    public:
        virtual ~TTY() = default;

        virtual void Initialize(size_t width, size_t height) = 0;

        virtual void SetCursorPos(uint32_t x, uint32_t y) = 0;
        virtual void SetColor(TerminalColor color) = 0;
        virtual void PutChar(char c) = 0;
        virtual void Write(const char *string, size_t size) = 0;

        virtual void Clear() = 0;
        virtual void Scroll() = 0;
    };

}; // namespace Kernel

#endif
