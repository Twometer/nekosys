#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <kernel/tty.h>
#include <kernel/vga.h>
#include <kernel/io.h>

using namespace Kernel;

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;

static size_t term_row = 0;
static size_t term_column = 0;
static size_t term_color = VGA_COLOR_WHITE;
static uint16_t *term_buffer = (uint16_t *)0xB8000;

void TTY::Clear()
{
    for (size_t y = 0; y < VGA_HEIGHT; y++)
    {
        for (size_t x = 0; x < VGA_WIDTH; x++)
        {
            PutEntry(' ', term_color, x, y);
        }
    }
    SetCursorPos(0, 0);
}

void TTY::SetColor(uint8_t color)
{
    term_color = color;
}

void TTY::PutEntry(char c, uint8_t color, size_t x, size_t y)
{
    term_buffer[x + (y * VGA_WIDTH)] = (color << 8) | (unsigned char)c;
}

void TTY::PutChar(char c)
{
    if (c == '\b')
    {
        if (term_column > 0)
        {
            SetCursorPos(term_column - 1, term_row);
            PutEntry(' ', term_color, term_column, term_row);
        }
        return;
    }

    bool is_newline = (c == '\n' || c == '\r');

    if (!is_newline)
        PutEntry(c, term_color, term_column, term_row);

    if (++term_column == VGA_WIDTH || is_newline)
    {
        term_column = 0;
        if (++term_row == VGA_HEIGHT)
        {
            term_row--;
            Scroll();
        }
    }
    SetCursorPos(term_column, term_row);
}

void TTY::Scroll()
{
    // Discard first line
    size_t new_buf_size = 2 * (VGA_HEIGHT - 1) * VGA_WIDTH;
    memcpy(term_buffer, term_buffer + VGA_WIDTH, new_buf_size);

    // Clear last line
    for (size_t x = 0; x < VGA_WIDTH; x++)
    {
        PutEntry(' ', term_color, x, term_row);
    }
}

void TTY::Write(const char *data, size_t size)
{
    for (size_t i = 0; i < size; i++)
        PutChar(data[i]);
}

void TTY::Print(const char *string)
{
    Write(string, strlen(string));
}

void TTY::SetCursorPos(size_t x, size_t y)
{
    term_column = x;
    term_row = y;
    uint16_t pos = y * VGA_WIDTH + x;

    
    IO::Out8(0x3D4, 0x0F);
    IO::Out8(0x3D5, (uint8_t)(pos & 0xFF));
    IO::Out8(0x3D4, 0x0E);
    IO::Out8(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}