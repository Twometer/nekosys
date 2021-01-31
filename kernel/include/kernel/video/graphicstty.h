#ifndef _GRAPHICS_TTY_H
#define _GRAPHICS_TTY_H

#include <kernel/video/tty.h>

namespace Video
{

    class GraphicsTTY : public TTY
    {
    private:
        uint32_t currentColor;

    public:
        void Initialize(size_t width, size_t height) override;

        void SetCursorPos(uint32_t x, uint32_t y) override;
        void SetColor(TerminalColor color) override;
        void PutChar(char c) override;
        void Write(const char *string, size_t size) override;

        void Clear() override;
        void Scroll() override;
    };

}; // namespace Video

#endif
