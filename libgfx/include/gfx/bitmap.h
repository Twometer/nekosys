#ifndef _BITMAP_H
#define _BITMAP_H

#include "font.h"
#include "color.h"
#include "rectangle.h"
#include "pixelformat.h"
#include <nk/string.h>
#include <stddef.h>
#include <stdint.h>

enum class BlendMode
{
    Default,
    Alpha
};

class Bitmap
{
public:
    unsigned int width;
    unsigned int height;
    unsigned int stride;
    unsigned int bpp;
    PixelFormat *format;

private:
    uint8_t *data;
    bool ownsBuffer;
    Rectangle mask = Rectangle(0, 0, 0, 0);

public:
    Bitmap(const nk::String &path);

    Bitmap(unsigned int width, unsigned int height, PixelFormat *format);

    Bitmap(unsigned int width, unsigned int height, unsigned int stride, uint8_t *data, PixelFormat *format);

    ~Bitmap();

    inline void SetPixel(unsigned int x, unsigned int y, Color pixel);
    inline Color GetPixel(unsigned int x, unsigned int y) const;

    void Blit(const Bitmap &other, Point position, BlendMode blendMode = BlendMode::Default);
    void DrawText(const nk::String &text, const Font &font, const Point &position, const Color &color);
    void FillRect(const Rectangle &rect, const Color &color);
    void FillGradient(const Rectangle &rect, const Color &a, const Color &b);

    void SetMask(const Rectangle &rect);
    void ClearMask();

private:
    inline size_t GetIndex(unsigned int x, unsigned int y) const;

    Color Blend(Color a, Color b);
};

#endif
