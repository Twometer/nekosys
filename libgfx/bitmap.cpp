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
    ClearMask();
}

Bitmap::Bitmap(unsigned int width, unsigned int height, PixelFormat *format)
    : width(width), height(height), format(format), bpp(format->GetBpp()), ownsBuffer(true), stride(width * format->GetBpp())
{
    data = new uint8_t[height * stride];
    ClearMask();
}

Bitmap::Bitmap(unsigned int width, unsigned int height, unsigned int stride, uint8_t *data, PixelFormat *format) : width(width), height(height), stride(stride), data(data), format(format), ownsBuffer(false)
{
    bpp = format->GetBpp();
    ClearMask();
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

void Bitmap::Blit(const Bitmap &other, Point position, BlendMode blendMode)
{
    Rectangle dstRect = Rectangle(position, {other.width, other.height});
    if (!mask.Intersects(dstRect))
        return;

    dstRect = dstRect.Intersection(mask);
    dstRect.x1 = MIN(dstRect.x1, width);
    dstRect.y1 = MIN(dstRect.y1, height);

    Point srcOffset = {dstRect.x0 - position.x, dstRect.y0 - position.y};

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
                if (blendMode == BlendMode::Alpha)
                {
                    auto mypx = format->GetPixel(data, dstIdx);
                    format->SetPixel(data, dstIdx, Blend(mypx, px));
                }
                else
                {
                    format->SetPixel(data, dstIdx, px);
                }
                srcIdx += other.bpp;
                dstIdx += bpp;
            }
            srcIdx += srcLineskip;
            dstIdx += dstLineskip;
        }
    }
}
void Bitmap::DrawText(const nk::String &text, const Font &font, const Point &position, const Color &color)
{
    auto ptr = text.CStr();
    unsigned int xOffset = 0;
    for (size_t i = 0; i < text.Length(); i++)
    {
        Glyph &glyph = font.glyphs[ptr[i]];
        auto glyphRect = Rectangle(position.x + xOffset, position.y, glyph.advance, font.height);

        if (mask.Intersects(glyphRect))
        {
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
        }

        xOffset += glyph.advance + 1;
    }
}

void Bitmap::FillRect(const Rectangle &rectangle, const Color &color)
{
    if (!mask.Intersects(rectangle))
        return;

    auto masked = rectangle.Intersection(mask);
    masked.x1 = MIN(masked.x1, width);
    masked.y1 = MIN(masked.y1, height);

    
    for (unsigned int y = masked.y0; y < masked.y1; y++)
    {
        for (unsigned int x = masked.x0; x < masked.x1; x++)
        {
            SetPixel(x, y, color);
        }
    }
}

void Bitmap::FillGradient(const Rectangle &rectangle, const Color &a, const Color &b)
{
    if (!mask.Intersects(rectangle))
        return;

    auto masked = rectangle.Intersection(mask);
    masked.x1 = MIN(masked.x1, width);
    masked.y1 = MIN(masked.y1, height);
    
    for (unsigned int y = masked.y0; y < masked.y1; y++)
    {
        for (unsigned int x = masked.x0; x < masked.x1; x++)
        {
            uint8_t alpha = static_cast<uint8_t>(255.0f * ((x - rectangle.x0) / (float)(rectangle.size().width)));
            SetPixel(x, y, Blend(a, {b.r, b.g, b.b, alpha}));
        }
    }
}

void Bitmap::SetMask(const Rectangle &rect)
{
    this->mask = rect;
}

void Bitmap::ClearMask()
{
    this->mask = Rectangle(0, 0, width, height);
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