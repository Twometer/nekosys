#include <stdio.h>
#include <png/lodepng.h>
#include <gfx/bitmap.h>
#include <nk/memutil.h>

Bitmap::Bitmap(const nk::String &path)
    : ownsBuffer(true), format(PixelFormat::Rgba32)
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
    unsigned error = lodepng_decode_memory(&this->data, &width, &height, data, len, LodePNGColorType::LCT_RGBA, 8);

    delete[] data;

    bpp = format->GetBpp();
    stride = width * bpp;
}

Bitmap::Bitmap(unsigned int width, unsigned int height, PixelFormat *format)
    : width(width), height(height), format(format), bpp(format->GetBpp()), ownsBuffer(true), stride(width * format->GetBpp())
{
    data = new uint8_t[height * stride];
}

Bitmap::Bitmap(unsigned int width, unsigned int height, unsigned int stride, uint8_t *data, PixelFormat *format) : width(width), height(height), stride(stride), data(data), format(format), ownsBuffer(false)
{
    bpp = format->GetBpp();
}

Bitmap::~Bitmap()
{
    if (ownsBuffer)
        delete[] data;
}

void Bitmap::SetPixel(unsigned int x, unsigned int y, Color pixel)
{
    size_t baseIdx = GetIndex(x, y);
    format->SetPixel(data, baseIdx, pixel);
}

Color Bitmap::GetPixel(unsigned int x, unsigned int y) const
{
    size_t baseIdx = GetIndex(x, y);
    return format->GetPixel(data, baseIdx);
}

inline size_t Bitmap::GetIndex(unsigned int x, unsigned int y) const
{
    return y * stride + x * bpp;
}

void Bitmap::Blit(const Bitmap &other, const Point &srcOffset, const Rectangle &dstRect)
{
    if (format == other.format)
    {
        uint8_t *src = other.data + srcOffset.y * other.stride + srcOffset.x * other.bpp;
        uint8_t *dst = data + dstRect.position.y * stride + dstRect.position.x * bpp;
        size_t blocksize = dstRect.size.width * bpp;
        for (size_t y = 0; y < dstRect.size.height; y++)
        {
            fast_copy(src, dst, blocksize);
            src += other.stride;
            dst += stride;
        }
        return;
    }
    else
    {
        size_t srcIdx = srcOffset.y * other.stride + srcOffset.x * other.bpp;
        size_t dstIdx = dstRect.position.y * stride + dstRect.position.x * bpp;
        size_t srcLineskip = other.stride - dstRect.size.width * other.bpp;
        size_t dstLineskip = stride - dstRect.size.width * bpp;

        for (size_t y = 0; y < dstRect.size.height; y++)
        {
            for (size_t x = 0; x < dstRect.size.width; x++)
            {
                auto px = other.format->GetPixel(other.data, srcIdx);
                format->SetPixel(data, dstIdx, px);
                srcIdx += other.bpp;
                dstIdx += bpp;
            }
            srcIdx += srcLineskip;
            dstIdx += dstLineskip;
        }
    }
}

void Bitmap::DrawBitmap(const Bitmap &other, const Rectangle &dstRect)
{
    size_t srcIdx = 0;
    size_t dstIdx = dstRect.position.y * stride + dstRect.position.x * bpp;
    size_t srcLineskip = other.stride - dstRect.size.width * other.bpp;
    size_t dstLineskip = stride - dstRect.size.width * bpp;

    for (size_t y = 0; y < dstRect.size.height; y++)
    {
        for (size_t x = 0; x < dstRect.size.width; x++)
        {
            auto srcpx = other.format->GetPixel(other.data, srcIdx);
            auto dstpx = format->GetPixel(data, dstIdx);
            format->SetPixel(data, dstIdx, Blend(dstpx, srcpx));
            srcIdx += other.bpp;
            dstIdx += bpp;
        }
        srcIdx += srcLineskip;
        dstIdx += dstLineskip;
    }
}

Color Bitmap::Blend(Color b, Color a)
{
    float alpha = a.a / 255.0f;
    float alphaInv = 1.0f - alpha;
    return {
        static_cast<uint8_t>(alpha * (float)a.r + alphaInv * (float)b.r),
        static_cast<uint8_t>(alpha * (float)a.g + alphaInv * (float)b.g),
        static_cast<uint8_t>(alpha * (float)a.b + alphaInv * (float)b.b),
        255};
}