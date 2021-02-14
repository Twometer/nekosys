#include <png/lodepng.h>
#include <nekosys.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char **argv)
{
    if (argv == 0)
    {
        printf("Syntax: imgview <path>\b");
        return 1;
    }

    FILE *fd = fopen(argv[0], "r");
    if (!fd)
    {
        printf("imgview: error: Could not load image\n");
        return 1;
    }

    fseek(fd, 0, SEEK_END);
    int len = ftell(fd);
    fseek(fd, 0, SEEK_SET);

    uint8_t *data = new uint8_t[len];
    fread(data, 1, len, fd);
    fclose(fd);

    unsigned char *image;
    unsigned int width, height;

    unsigned error = lodepng_decode_memory(&image, &width, &height, data, len, LodePNGColorType::LCT_RGB, 8);

    if (error)
    {
        printf("decoder error: %s\n", lodepng_error_text(error));
        return 1;
    }

    FRAMEBUF buf;
    framebuf_acquire(&buf);

    size_t dOffset = 0;
    size_t sOffset = 0;
    for (size_t row = 0; row < height; row++)
    {
        for (size_t col = 0; col < width; col++)
        {
            buf.buffer[dOffset + col * 4 + 2] = image[sOffset + col * 3 + 0];
            buf.buffer[dOffset + col * 4 + 1] = image[sOffset + col * 3 + 1];
            buf.buffer[dOffset + col * 4 + 0] = image[sOffset + col * 3 + 2];
        }
        dOffset += buf.pitch;
        sOffset += width * 3;
    }

    framebuf_flush_all();
    framebuf_release();

    delete[] data;
    free(image);
}