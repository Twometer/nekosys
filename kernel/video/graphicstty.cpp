#include <kernel/video/graphicstty.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <kernel/video/tty.h>
#include <kernel/video/vga.h>
#include <kernel/io.h>

#include <kernel/video/videomanager.h>
#include <kernel/video/kernelfont.h>

using namespace Kernel;
using namespace Video;

void GraphicsTTY::Initialize(size_t width, size_t height)
{
    this->width = width / KFNT_CHAR_WIDTH;
    this->height = height / KFNT_LINE_HEIGHT;
    SetColor(TerminalColor::LightGray);
}

void GraphicsTTY::SetCursorPos(uint32_t x, uint32_t y)
{
    cursorX = x;
    cursorY = y;
}

void GraphicsTTY::SetColor(TerminalColor color)
{
    switch (color)
    {
    case TerminalColor::Black:
        currentColor = 0x00000000;
        break;
    case TerminalColor::DarkGray:
        currentColor = 0x00555555;
        break;
    case TerminalColor::DarkBlue:
        currentColor = 0x000000AA;
        break;
    case TerminalColor::LightBlue:
        currentColor = 0x005555FF;
        break;
    case TerminalColor::DarkGreen:
        currentColor = 0x0000AA00;
        break;
    case TerminalColor::LightGreen:
        currentColor = 0x0055FF55;
        break;
    case TerminalColor::DarkCyan:
        currentColor = 0x0000AAAA;
        break;
    case TerminalColor::LightCyan:
        currentColor = 0x0055FFFF;
        break;
    case TerminalColor::DarkRed:
        currentColor = 0x00AA0000;
        break;
    case TerminalColor::LightRed:
        currentColor = 0x00FF5555;
        break;
    case TerminalColor::DarkMagenta:
        currentColor = 0x00AA00AA;
        break;
    case TerminalColor::LightMagenta:
        currentColor = 0x00FF55FF;
        break;
    case TerminalColor::Brown:
        currentColor = 0x00AA5500;
        break;
    case TerminalColor::Yellow:
        currentColor = 0x00FFFF55;
        break;
    case TerminalColor::LightGray:
        currentColor = 0x00AAAAAA;
        break;
    case TerminalColor::White:
        currentColor = 0x00FFFFFF;
        break;
    default:
        break;
    }
}

void GraphicsTTY::PutChar(char c)
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

    if (cursorX >= width - 1)
    {
        PutChar('\n');
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

void GraphicsTTY::Write(const char *string, size_t size)
{
    for (size_t i = 0; i < size; i++)
        PutChar(string[i]);
}

void GraphicsTTY::Clear()
{
}

void GraphicsTTY::Scroll()
{
}