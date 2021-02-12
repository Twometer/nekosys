#include <png/lodepng.h>
#include <nekosys.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char **argv)
{
    FILE *fd = fopen("/etc/logo.png", "r");
    if (!fd)
    {
        printf("sakura: error: Could not open bitmap\n");
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
    }
    else
    {
        printf("decoded successfully");
    }

    FRAMEBUF buf;
    framebuf_acquire(&buf);

    size_t dOffset = 0;
    size_t sOffset = 0;
    for (size_t row = 0; row < height; row++)
    {
        for (size_t col = 0; col < width; col ++)
        {
            //dstbuf[dOffset + col] = (srcbuf[sOffset + col] >> 8) & 0x00FFFFFF;
            //buf.buffer[dOffset + col*4] = 0;
            buf.buffer[dOffset + col*4 + 2] = image[sOffset + col*3 + 0];
            buf.buffer[dOffset + col*4 + 1] = image[sOffset + col*3 + 1];
            buf.buffer[dOffset + col*4 + 0] = image[sOffset + col*3 + 2];
        }
        dOffset += buf.pitch;
        sOffset += width * 3;
    }

    framebuf_flush_all();
    framebuf_release();

    printf("image size: %d x %d\n", width, height);

    delete[] data;
    free(image);
}