#include <stdio.h>
#include <nekosys.h>
#include <string.h>
#include <stdint.h>

int main(int argc, char **argv)
{
	FRAMEBUF buf;
	framebuf_acquire(&buf);
	memset(buf.buffer, 0xFF, buf.pitch * buf.height * buf.pixelStride);
	framebuf_flush_all();
	
	framebuf_release();
	printf("Framebuffer: %d x %d %x\n", buf.width, buf.height, buf.buffer);
	return 0;
}
