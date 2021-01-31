#include <stdio.h>
#include <nekosys.h>
#include <string.h>
#include <stdint.h>

struct BITMAPFILEHEADER
{
	uint16_t bfType;
	uint32_t bfSize;
	uint32_t bfReserved;
	uint32_t bfOffBits;
} __attribute__((packed));

struct BITMAPINFO
{
	uint32_t biSize;
	int32_t biWidth;
	int32_t biHeight;
	uint16_t biPlanes;
	uint16_t biBitCount;
	uint32_t biCompression;
} __attribute__((packed));

int main(int argc, char **argv)
{
	FILE *fd = fopen("/home/neko/wall.bmp", "r");
	if (!fd)
	{
		printf("sakura: error: Could not open bitmap\n");
		return 1;
	}

	fseek(fd, 0, SEEK_END);
	int len = ftell(fd);
	fseek(fd, 0, SEEK_SET);

	printf("bitmap length: %d\n", len);

	uint8_t *data = new uint8_t[len];
	fread(data, 1, len, fd);
	fclose(fd);

	BITMAPFILEHEADER *header = (BITMAPFILEHEADER *)data;
	if (header->bfType != 0x4d42)
	{
		printf("sakura: error: Invalid bitmap signature %x\n", header->bfType);
		return 1;
	}

	BITMAPINFO *info = (BITMAPINFO *)(data + sizeof(BITMAPFILEHEADER));

	printf("Bitmap data starts at %d\n", header->bfOffBits);
	

	FRAMEBUF buf;
	framebuf_acquire(&buf);

	size_t dstStride = buf.width * buf.pixelStride;
	size_t srcStride = info->biWidth * (info->biBitCount) / 8;

	size_t srcOffset = len - header->bfOffBits;
	size_t dstOffset = 0;

	for (size_t line = 0; line < info->biHeight; line++)
	{
		int col = 0;
		for (size_t s = 0, d = 0; s < srcStride && d < dstStride; s++, d++)
		{
			col++;
			*(uint8_t *)(buf.buffer + dstOffset + d) = *(data + srcOffset + s);
			if (col == 3)
			{
				col = 0;
				d++;
			}
		}
		dstOffset += buf.pitch;
		srcOffset -= srcStride;
	}

	framebuf_flush_all();
	framebuf_release();

	printf("Width: %d, Height: %d, BPP: %d, Compression: %d\n", info->biWidth, info->biHeight, info->biBitCount, info->biCompression);

	return 0;
}
