#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <kernel/video/texttty.h>
#include <kernel/video/vga.h>
#include <kernel/io.h>

using namespace Kernel;
using namespace Video;

void TextTTY::Initialize(size_t width, size_t height)
{
    this->width = width;
    this->height = height;
    SetColor(TerminalColor::LightGray);
}

void TextTTY::Clear()
{
    for (size_t y = 0; y < height; y++)
    {
        for (size_t x = 0; x < width; x++)
        {
            PutEntry(' ', currentColor, x, y);
        }
    }
    SetCursorPos(0, 0);
}

void TextTTY::SetColor(TerminalColor color)
{
    currentColor = (uint8_t)color;
}

void TextTTY::PutEntry(char c, uint8_t color, size_t x, size_t y)
{
    buffer[x + (y * width)] = (color << 8) | (unsigned char)c;
}

void TextTTY::PutChar(char c)
{
    if (c == '\b')
    {
        if (cursorX > 0)
        {
            SetCursorPos(cursorX - 1, cursorY);
            PutEntry(' ', currentColor, cursorX, cursorY);
        }
        return;
    }

    bool is_newline = (c == '\n' || c == '\r');

    if (!is_newline)
        PutEntry(c, currentColor, cursorX, cursorY);

    if (++cursorX == width || is_newline)
    {
        cursorX = 0;
        if (++cursorY == height)
        {
            cursorY--;
            Scroll();
        }
    }
    SetCursorPos(cursorX, cursorY);
}

void TextTTY::Scroll()
{
    // Discard first line
    size_t new_buf_size = 2 * (height - 1) * width;
    memcpy(buffer, buffer + width, new_buf_size);

    // Clear last line
    for (size_t x = 0; x < width; x++)
    {
        PutEntry(' ', currentColor, x, cursorY);
    }
}

void TextTTY::Write(const char *data, size_t size)
{
    for (size_t i = 0; i < size; i++)
        PutChar(data[i]);
}

void TextTTY::SetCursorPos(uint32_t x, uint32_t y)
{
    cursorX = x;
    cursorY = y;
    uint16_t pos = y * width + x;

    IO::Out8(0x3D4, 0x0F);
    IO::Out8(0x3D5, (uint8_t)(pos & 0xFF));
    IO::Out8(0x3D4, 0x0E);
    IO::Out8(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}