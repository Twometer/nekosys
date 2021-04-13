#include <stdio.h>
#include <png/lodepng.h>
#include <gfx/bitmap.h>

Bitmap::Bitmap(const nk::String &path)
    : ownsBuffer(true), format(PixelFormat::Rgb24), bpp(3)
{
    FILE *fd = fopen(path.CStr(), "r");
    if (!fd)
        return; // TODO: Error

    fseek(fd, 0, SEEK_END);
    size_t len = ftell(fd);
    fseek(fd, 0, SEEK_SET);

    uint8_t *data = new uint8_t[len];
    fread(data, 1, len, fd);
    fclose(fd);

    unsigned error = lodepng_decode_memory(&this->data, &width, &height, data, len, LodePNGColorType::LCT_RGB, 8);
    stride = width * bpp;
}

Bitmap::Bitmap(unsigned int width, unsigned int height, unsigned int stride, uint8_t *data, PixelFormat format)
    : width(width), height(height), stride(stride), data(data), format(format), ownsBuffer(false)
{
    bpp = format == PixelFormat::Bgr32 ? 4 : 3;
}

Bitmap::~Bitmap()
{
    if (ownsBuffer)
        delete[] data;
}

void Bitmap::SetPixel(unsigned int x, unsigned int y, Color pixel)
{
    size_t baseIdx = GetIndex(x, y);
    if (format == PixelFormat::Rgb24)
    {
        data[baseIdx + 0] = pixel.r;
        data[baseIdx + 1] = pixel.g;
        data[baseIdx + 2] = pixel.b;
    }
    else if (format == PixelFormat::Bgr32)
    {
        data[baseIdx + 2] = pixel.r;
        data[baseIdx + 1] = pixel.g;
        data[baseIdx + 0] = pixel.b;
    }
}

Color Bitmap::GetPixel(unsigned int x, unsigned int y) const
{
    size_t baseIdx = GetIndex(x, y);
    if (format == PixelFormat::Rgb24)
    {
        return {data[baseIdx + 0], data[baseIdx + 1], data[baseIdx + 2], 1};
    }
    else if (format == PixelFormat::Bgr32)
    {
        return {data[baseIdx + 2], data[baseIdx + 1], data[baseIdx + 0], 1};
    }
}

inline size_t Bitmap::GetIndex(unsigned int x, unsigned int y) const
{
    return y * stride + x * bpp;
}

void Bitmap::Blit(const Bitmap &other, const Rectangle &dstRect)
{
    for (size_t x = 0; x < dstRect.size.width; x++)
    {
        for (size_t y = 0; y < dstRect.size.height; y++)
        {
            auto px = other.GetPixel(x, y);
            SetPixel(x + dstRect.position.x, y + dstRect.position.y, px);
        }
    }
}