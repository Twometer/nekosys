#include <gfx/fontloader.h>
#include <gfx/util.h>
#include <string.h>

Font *FontLoader::LoadFont(const nk::String &path)
{
    size_t len;
    auto data = read_file(path.CStr(), &len);
    if (data == nullptr)
        return nullptr;

    if (data[0] != 'F' || data[1] != 'N' || data[2] != 'T')
        return nullptr;

    Font *font = new Font();
    font->file = data;
    font->glyphs = new Glyph[256];
    font->name = (const char *)(data + 3);

    auto idx = 3 + strlen(font->name) + 1;
    font->width = data[idx];
    idx++;
    font->height = data[idx];
    idx++;

    uint32_t *glyphBase = (uint32_t *)(data + idx);
    for (size_t i = 0; i < 256; i++)
    {
        Glyph glyph;
        glyph.rows = glyphBase;
        glyph.advance = *((uint8_t*)(glyphBase + font->height) + 1);
        font->glyphs[i] = glyph;
        glyphBase += font->height + 1;
    }

    return font;
}