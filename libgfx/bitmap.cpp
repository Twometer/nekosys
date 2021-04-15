#include <stdio.h>
#include <png/lodepng.h>
#include <gfx/bitmap.h>
#include <gfx/util.h>
#include <nk/memutil.h>

Bitmap::Bitmap(const nk::String &path)
    : ownsBuffer(true), format(PixelFormat::Rgba32)
{
    size_t len;
    auto data = read_file(path.CStr(), &len);

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

void Bitmap::Blit(const Bitmap &other, const Point &srcOffset, Rectangle dstRect)
{
    dstRect.x1 = MIN(dstRect.x1, width);
    dstRect.y1 = MIN(dstRect.y1, height);

    if (format == other.format)
    {
        uint8_t *src = other.data + srcOffset.y * other.stride + srcOffset.x * other.bpp;
        uint8_t *dst = data + dstRect.position().y * stride + dstRect.position().x * bpp;
        size_t blocksize = dstRect.size().width * bpp;
        for (size_t y = 0; y < dstRect.size().height; y++)
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
        size_t dstIdx = dstRect.position().y * stride + dstRect.position().x * bpp;
        size_t srcLineskip = other.stride - dstRect.size().width * other.bpp;
        size_t dstLineskip = stride - dstRect.size().width * bpp;

        for (size_t y = 0; y < dstRect.size().height; y++)
        {
            for (size_t x = 0; x < dstRect.size().width; x++)
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

void Bitmap::DrawBitmap(const Bitmap &other, Rectangle dstRect)
{
    dstRect.x1 = MIN(dstRect.x1, width);
    dstRect.y1 = MIN(dstRect.y1, height);

    size_t srcIdx = 0;
    size_t dstIdx = dstRect.position().y * stride + dstRect.position().x * bpp;
    size_t srcLineskip = other.stride - dstRect.size().width * other.bpp;
    size_t dstLineskip = stride - dstRect.size().width * bpp;

    for (size_t y = 0; y < dstRect.size().height; y++)
    {
        for (size_t x = 0; x < dstRect.size().width; x++)
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

void Bitmap::DrawText(const nk::String &text, const Font &font, const Point &position, const Color &color)
{
    auto ptr = text.CStr();
    unsigned int xOffset = 0;
    for (size_t i = 0; i < text.Length(); i++)
    {
        Glyph &glyph = font.glyphs[ptr[i]];

        for (unsigned int y = 0; y < font.height; y++)
        {
            uint16_t rowData = glyph.rows[y];
            for (unsigned int x = 0; x < font.width; x++)
            {
                if (rowData & (1 << x))
                {
                    SetPixel(x + position.x + xOffset, y + position.y, color);
                }
            }
        }

        xOffset += glyph.advance + 1;
    }
}

void Bitmap::FillRect(const Rectangle &rectangle, const Color &color)
{
    for (unsigned int i = rectangle.x0; i < rectangle.x1; i++)
    {
        for (unsigned int j = rectangle.y0; j < rectangle.y1; j++)
        {
            SetPixel(i, j, color);
        }
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