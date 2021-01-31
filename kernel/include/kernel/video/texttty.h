#ifndef _TEXT_TTY_H
#define _TEXT_TTY_H

#include <kernel/video/tty.h>

namespace Video
{

    class TextTTY : public TTY
    {
    private:
        uint16_t *buffer = (uint16_t *)0xB8000;
        uint8_t currentColor;

    public:
        void Initialize(size_t width, size_t height) override;

        void SetCursorPos(uint32_t x, uint32_t y) override;
        void SetColor(TerminalColor color) override;
        void PutChar(char c) override;
        void Write(const char *string, size_t size) override;

        void Clear() override;
        void Scroll() override;

    private:
        void PutEntry(char c, uint8_t color, size_t x, size_t y);
    };

}; // namespace Video

#endif
