#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <kernel/tty.h>
#include <kernel/vga.h>
#include <kernel/io.h>

#include <kernel/video/videomanager.h>
#include <kernel/video/kernelfont.h>

using namespace Kernel;
using namespace Video;

uint32_t TTY::cursorX = 0;
uint32_t TTY::cursorY = 0;
uint32_t TTY::currentColor = 0xAAAAAAAA;

void TTY::SetCursorPos(size_t x, size_t y)
{
    cursorX = x;
    cursorY = y;
}

void TTY::SetColor(uint32_t color)
{
    currentColor = color;
}

void TTY::PutChar(char c)
{
    if (!VideoManager::GetInstance()->KernelControlsFramebuffer())
        return;

    if (c == '\n')
    {
        cursorX = 0;
        cursorY++;
        return;
    }

    if (c == '\b')
    {
        cursorX--;
    }

    int x = cursorX * KFNT_CHAR_WIDTH;
    int y = cursorY * KFNT_LINE_HEIGHT;

    if (c == '\b')
    {
        for (size_t i = 0; i < KFNT_CHAR_WIDTH; i++)
            for (size_t j = 0; j < KFNT_LINE_HEIGHT; j++)
                VideoManager::GetInstance()->SetPixel(x + i, y + j, 0);

        VideoManager::GetInstance()->FlushBlock(x, y, KFNT_CHAR_WIDTH, KFNT_LINE_HEIGHT);
        return;
    }

    uint16_t *data = KFNT[(int)c];
    for (size_t r = 0; r < 16; r++)
    {
        uint16_t row = data[r];
        for (size_t c = 0; c < 16; c++)
        {
            if (row & (1 << c))
                VideoManager::GetInstance()->SetPixel(x + c, y + r, currentColor);
        }
    }
    VideoManager::GetInstance()->FlushBlock(x, y, KFNT_CHAR_WIDTH, KFNT_LINE_HEIGHT);
    cursorX++;
}

void TTY::Write(const char *string, size_t size)
{
    for (size_t i = 0; i < size; i++)
        PutChar(string[i]);
}

void TTY::Clear()
{
}

void TTY::Scroll()
{
}