#ifndef _BITMAP_H
#define _BITMAP_H

#include "color.h"
#include "rectangle.h"
#include "pixelformat.h"
#include <nk/string.h>
#include <stddef.h>
#include <stdint.h>

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

public:
    Bitmap(const nk::String &path);

    Bitmap(unsigned int width, unsigned int height, PixelFormat *format);

    Bitmap(unsigned int width, unsigned int height, unsigned int stride, uint8_t *data, PixelFormat *format);

    ~Bitmap();

    inline void SetPixel(unsigned int x, unsigned int y, Color pixel);
    inline Color GetPixel(unsigned int x, unsigned int y) const;

    void Blit(const Bitmap &other, const Point &srcOffset, const Rectangle &dstRect);
    void DrawBitmap(const Bitmap &other, const Rectangle &dstRect);

private:
    inline size_t GetIndex(unsigned int x, unsigned int y) const;

    Color Blend(Color a, Color b);
};

#endif
