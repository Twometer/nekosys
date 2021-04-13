#ifndef _BITMAP_H
#define _BITMAP_H

#include "color.h"
#include "rectangle.h"
#include <nk/string.h>
#include <stddef.h>
#include <stdint.h>

enum class PixelFormat
{
    Bgr32,
    Rgb24
};

class Bitmap
{
public:
    unsigned int width;
    unsigned int height;
    unsigned int stride;
    unsigned int bpp;
    PixelFormat format;

private:
    uint8_t *data;
    bool ownsBuffer;

public:
    Bitmap(const nk::String &path);

    Bitmap(unsigned int width, unsigned int height, unsigned int stride, uint8_t *data, PixelFormat format);

    ~Bitmap();

    void SetPixel(unsigned int x, unsigned int y, Color pixel);
    Color GetPixel(unsigned int x, unsigned int y) const;

    void Blit(const Bitmap &other, const Rectangle &dstRect);

private:
    inline size_t GetIndex(unsigned int x, unsigned int y) const;
};

#endif
