#ifndef _PIXEL_FORMAT_H
#define _PIXEL_FORMAT_H

#include "color.h"
#include <stddef.h>
#include <stdint.h>

class PixelFormat
{
public:
    static PixelFormat *Bgr32;
    static PixelFormat *Rgba32;
    static PixelFormat *Rgb24;

    virtual void SetPixel(uint8_t *data, size_t baseIdx, Color pixel) = 0;

    virtual Color GetPixel(uint8_t *data, size_t baseIdx) = 0;

    virtual inline int GetBpp() = 0;
};

class PixFmtBgr32 : public PixelFormat
{
    void SetPixel(uint8_t *data, size_t baseIdx, Color pixel) override
    {
        *(uint32_t *)(data + baseIdx) = (pixel.b | pixel.g << 8u | pixel.r << 16u);
    }

    Color GetPixel(uint8_t *data, size_t baseIdx) override
    {
        uint32_t value = *(uint32_t *)(data + baseIdx);
        return {(value >> 16u) & 0xFF, (value >> 8u) & 0xFF, value & 0xFF, 255};
    }

    inline int GetBpp() override
    {
        return 4;
    }
};

class PixFmtRgba32 : public PixelFormat
{
    void SetPixel(uint8_t *data, size_t baseIdx, Color pixel) override
    {
        uint32_t *ptr = (uint32_t *)(data + baseIdx);
        *ptr = (pixel.r | pixel.g << 8 | pixel.b << 16 || pixel.a << 24);
    }

    Color GetPixel(uint8_t *data, size_t baseIdx) override
    {
        return {data[baseIdx + 0], data[baseIdx + 1], data[baseIdx + 2], data[baseIdx + 3]};
    }

    inline int GetBpp() override
    {
        return 4;
    }
};

class PixFmtRgb24 : public PixelFormat
{
    void SetPixel(uint8_t *data, size_t baseIdx, Color pixel) override
    {
        data[baseIdx + 0] = pixel.r;
        data[baseIdx + 1] = pixel.g;
        data[baseIdx + 2] = pixel.b;
    }

    Color GetPixel(uint8_t *data, size_t baseIdx) override
    {
        return {data[baseIdx + 0], data[baseIdx + 1], data[baseIdx + 2], 1};
    }

    inline int GetBpp() override
    {
        return 3;
    }
};

#endif
